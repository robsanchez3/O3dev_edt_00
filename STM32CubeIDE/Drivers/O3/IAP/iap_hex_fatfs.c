/*============================================================================
 * iap_hex_fatfs.c -- Intel HEX loader for embedded targets.
 *
 *  Port of hex_reader.c (IAP/pc/) adapted for STM32:
 *    fopen/fgets/fclose  →  f_open/f_gets/f_close  (FatFS)
 *    malloc/free         →  pvPortMalloc/vPortFree  (FreeRTOS heap)
 *    fprintf(stderr)     →  printf                  (SWV ITM)
 *
 *  Memory budget: ~30784 bytes (flat app buf) + ~3849 bytes (written bitmap)
 *  + (count * 66 bytes) for the block array.  Requires ~36 KB free heap.
 *===========================================================================*/
#include "iap_hex_fatfs.h"
#include "fatfs.h"
#include "FreeRTOS.h"

#include <string.h>
#include <stdio.h>

#define APP_FIRST_BYTE  ((uint32_t)IAP_APP_FIRST_BLK * IAP_BLOCK_SIZE)  /* 0x0500 */
#define APP_LAST_BYTE   ((uint32_t)IAP_APP_LAST_BLK  * IAP_BLOCK_SIZE)  /* 0x7E40 */
#define APP_SIZE        (APP_LAST_BYTE - APP_FIRST_BYTE)                 /* 31040  */
#define BLOCKS_IN_APP   ((size_t)(APP_SIZE / IAP_BLOCK_SIZE))            /* 485    */

/*--- Hex-char helpers -------------------------------------------------------*/

static int nibble(int c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int hex_byte(const char* s)
{
    int hi = nibble((unsigned char)s[0]);
    int lo = nibble((unsigned char)s[1]);
    if (hi < 0 || lo < 0) return -1;
    return (hi << 4) | lo;
}

/*--- Parser -----------------------------------------------------------------*/

int iap_hex_fatfs_load(iap_hex_image_t* img, const char* path)
{
    memset(img, 0, sizeof *img);

    FIL f;
    if (f_open(&f, path, FA_READ) != FR_OK) {
        printf("[HEX] cannot open %s\n", path);
        return -1;
    }

    uint8_t* buf     = (uint8_t*)pvPortMalloc(APP_SIZE);
    uint8_t* written = (uint8_t*)pvPortMalloc((APP_SIZE + 7u) / 8u);
    if (!buf || !written) {
        printf("[HEX] out of heap\n");
        vPortFree(buf);
        vPortFree(written);
        f_close(&f);
        return -1;
    }
    memset(buf, 0xFF, APP_SIZE);
    memset(written, 0, (APP_SIZE + 7u) / 8u);

    uint32_t linear_base = 0;
    char     line[256];
    int      lineno      = 0;
    int      rc          = 0;
    int      eof_seen    = 0;

    while (f_gets(line, sizeof line, &f)) {
        lineno++;
        if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r') continue;
        if (line[0] != ':') {
            printf("[HEX] line %d: missing ':'\n", lineno);
            rc = -1; break;
        }

        int len     = hex_byte(line + 1);
        int addr_hi = hex_byte(line + 3);
        int addr_lo = hex_byte(line + 5);
        int type    = hex_byte(line + 7);
        if (len < 0 || addr_hi < 0 || addr_lo < 0 || type < 0) {
            printf("[HEX] line %d: bad header\n", lineno);
            rc = -1; break;
        }

        uint16_t addr16 = (uint16_t)((addr_hi << 8) | addr_lo);
        uint8_t  sum    = (uint8_t)(len + addr_hi + addr_lo + type);
        uint8_t  data[256];

        int i;
        for (i = 0; i < len; i++) {
            int b = hex_byte(line + 9 + i * 2);
            if (b < 0) {
                printf("[HEX] line %d: bad data byte %d\n", lineno, i);
                rc = -1; break;
            }
            data[i] = (uint8_t)b;
            sum    += (uint8_t)b;
        }
        if (rc) break;

        int ck = hex_byte(line + 9 + len * 2);
        if (ck < 0 || (uint8_t)(sum + ck) != 0) {
            printf("[HEX] line %d: checksum mismatch\n", lineno);
            rc = -1; break;
        }

        switch (type) {
        case 0x00: {
            uint32_t byte_addr = linear_base + addr16;
            for (i = 0; i < len; i++) {
                uint32_t a = byte_addr + (uint32_t)i;
                if (a >= APP_FIRST_BYTE && a < APP_LAST_BYTE) {
                    uint32_t off = a - APP_FIRST_BYTE;
                    buf[off]              = data[i];
                    written[off >> 3]    |= (uint8_t)(1u << (off & 7u));
                }
            }
            break;
        }
        case 0x01:
            eof_seen = 1;
            break;
        case 0x02:
            if (len != 2) { printf("[HEX] line %d: type 02 len=%d\n", lineno, len); rc = -1; }
            else linear_base = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4);
            break;
        case 0x04:
            if (len != 2) { printf("[HEX] line %d: type 04 len=%d\n", lineno, len); rc = -1; }
            else linear_base = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16);
            break;
        case 0x03:
        case 0x05:
            break;
        default:
            printf("[HEX] line %d: unknown type 0x%02X (skipped)\n", lineno, type);
            break;
        }
        if (rc) break;
    }

    f_close(&f);

    if (rc == 0 && !eof_seen)
        printf("[HEX] warning: no EOF record\n");

    if (rc == 0) {
        uint8_t xor_all = 0;
        size_t off;
        for (off = 0; off < APP_SIZE; off++) xor_all ^= buf[off];

        /* Build compact block-level bitmap on the stack, then free written. */
        uint8_t has_data[BLOCKS_IN_APP];
        size_t count = 0, gaps = 0;
        size_t blk;
        for (blk = 0; blk < BLOCKS_IN_APP; blk++) {
            size_t base = blk * IAP_BLOCK_SIZE;
            has_data[blk] = 0;
            size_t k;
            for (k = 0; k < IAP_BLOCK_SIZE; k++) {
                size_t o = base + k;
                if (written[o >> 3] & (1u << (o & 7u))) { has_data[blk] = 1; break; }
            }
            if (has_data[blk]) count++; else gaps++;
        }
        vPortFree(written);
        written = NULL;

        iap_hex_block_t* blocks = (iap_hex_block_t*)pvPortMalloc(count * sizeof *blocks);
        if (count > 0 && !blocks) {
            printf("[HEX] out of heap for blocks\n");
            rc = -1;
        } else {
            size_t idx = 0;
            for (blk = 0; blk < BLOCKS_IN_APP; blk++) {
                if (has_data[blk]) {
                    blocks[idx].block_id = (uint16_t)(IAP_APP_FIRST_BLK + blk);
                    memcpy(blocks[idx].data, buf + blk * IAP_BLOCK_SIZE, IAP_BLOCK_SIZE);
                    idx++;
                }
            }
            img->blocks     = blocks;
            img->count      = count;
            img->gap_blocks = gaps;
            img->xor_all    = xor_all;
        }
    }

    vPortFree(buf);
    vPortFree(written);
    return rc;
}

void iap_hex_fatfs_free(iap_hex_image_t* img)
{
    if (!img) return;
    vPortFree(img->blocks);
    memset(img, 0, sizeof *img);
}

/*--- Cursor -----------------------------------------------------------------*/

void iap_hex_cursor_init(iap_hex_cursor_t* cur, const iap_hex_image_t* img)
{
    cur->img  = img;
    cur->next = 0;
}

int iap_hex_cursor_next(void*     ctx,
                        uint16_t* block_id,
                        uint8_t   out[IAP_BLOCK_SIZE],
                        int*      is_last)
{
    iap_hex_cursor_t* cur = (iap_hex_cursor_t*)ctx;
    if (cur->next >= cur->img->count) return -1;

    const iap_hex_block_t* b = &cur->img->blocks[cur->next];
    *block_id = b->block_id;
    memcpy(out, b->data, IAP_BLOCK_SIZE);
    cur->next++;
    *is_last = (cur->next >= cur->img->count) ? 1 : 0;
    return 0;
}
