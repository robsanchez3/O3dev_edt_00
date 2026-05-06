/*============================================================================
 * iap_hex_fatfs.h -- Intel HEX loader for embedded targets (FatFS + FreeRTOS).
 *
 *  Port of hex_reader.h/c for STM32 + FatFS + pvPortMalloc.
 *  Parses a PSoC Designer-generated .hex file from the USB/SD filesystem into
 *  an array of 64-byte flash blocks and computes the 8-bit XOR checksum.
 *  Compatible with iap_block_src_t callback required by iap_run().
 *===========================================================================*/
#ifndef IAP_HEX_FATFS_H
#define IAP_HEX_FATFS_H

#include <stdint.h>
#include <stddef.h>
#include "iap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t block_id;
    uint8_t  data[IAP_BLOCK_SIZE];
} iap_hex_block_t;

typedef struct {
    iap_hex_block_t* blocks;    /* sorted ascending by block_id, pvPortMalloc'd */
    size_t           count;     /* number of populated blocks                   */
    size_t           gap_blocks;/* blocks in app range with no data (xor=0xFF)  */
    uint8_t          xor_all;   /* XOR over full app range, gaps counted as 0xFF*/
} iap_hex_image_t;

/* Load a .hex file from FatFS path (e.g. "1:/GEN_UPDATE/TargetApp.hex").
 * Records outside [0x05C0, 0x7E00) are silently dropped.
 * Returns 0 on success, non-zero on parse/IO error (diagnostic via printf).
 * On error *img is zeroed and no free is required.                          */
int  iap_hex_fatfs_load(iap_hex_image_t* img, const char* path);

/* Idempotent: frees img->blocks and zeroes the struct.                      */
void iap_hex_fatfs_free(iap_hex_image_t* img);

/*--- Cursor (iap_block_src_t compatible) ------------------------------------*/
typedef struct {
    const iap_hex_image_t* img;
    size_t                 next;
} iap_hex_cursor_t;

void iap_hex_cursor_init(iap_hex_cursor_t* cur, const iap_hex_image_t* img);

/* iap_block_src_t-compatible callback: ctx must be iap_hex_cursor_t*.      */
int  iap_hex_cursor_next(void*     ctx,
                         uint16_t* block_id,
                         uint8_t   out[IAP_BLOCK_SIZE],
                         int*      is_last);

#ifdef __cplusplus
}
#endif

#endif /* IAP_HEX_FATFS_H */
