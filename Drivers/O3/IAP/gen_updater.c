/*============================================================================
 * gen_updater.c -- Generator firmware update orchestrator.
 *
 *  Full update sequence:
 *    1. Load "1:/GEN_UPDATE/TargetApp.hex" via FatFS (pvPortMalloc)
 *    2. dep_o3_com_iap_takeover() — abort USART3 IT reception
 *    3. Frame RST en crudo (1 disparo sin ACK) + 0xDD inmediato
 *    4. Send 0xDD trigger until 0xAA ACK (up to 5 s), then drain stale bytes
 *    5. iap_run() — ENTER → INVALIDATE → WRITE×N → CHECKSUM → VALIDATE → RESET
 *    6. On success: HAL_NVIC_SystemReset() after 2 s
 *    7. On failure: set error state, keep device in BL (safe)
 *===========================================================================*/
#include "gen_updater.h"
#include "iap.h"
#include "iap_hex_fatfs.h"
#include "iap_hal_stm32.h"
#include "../Dependencies/dep_o3.h"
#include "fatfs.h"
#include "stm32u5xx_hal.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <string.h>

extern osThreadId_t defaultTaskHandle;

/*--- Shared state (written from GenUpdaterTask, read from TouchGFX task) ----*/

osSemaphoreId_t hGenUpdStart = NULL;

static volatile uint8_t          s_pending   = 0;
static volatile gen_upd_state_t  s_state     = GEN_UPD_IDLE;
static volatile uint8_t          s_progress  = 0;
static volatile size_t           s_blk_total = 0;
static volatile size_t           s_blk_done  = 0;
static char                      s_msg[96]   = "";
static char                      s_hex_name[64] = "";

/*--- Internal helpers -------------------------------------------------------*/

static void set_msg(const char* m)
{
    strncpy(s_msg, m, sizeof s_msg - 1);
    s_msg[sizeof s_msg - 1] = '\0';
}

/* iap_block_src_t callback: advances cursor and updates progress display.  */
static int progress_src(void*     ctx,
                        uint16_t* block_id,
                        uint8_t   out[IAP_BLOCK_SIZE],
                        int*      is_last)
{
    iap_hex_cursor_t* cur = (iap_hex_cursor_t*)ctx;
    int rc = iap_hex_cursor_next(cur, block_id, out, is_last);
    if (rc == 0) {
        s_blk_done++;
        if (s_blk_total > 0)
            s_progress = (uint8_t)(s_blk_done * 100u / s_blk_total);

        char tmp[80];
        snprintf(tmp, sizeof tmp, "Block %u/%u  id=0x%04X",
                 (unsigned)s_blk_done, (unsigned)s_blk_total, (unsigned)*block_id);
        set_msg(tmp);
        printf("[IAP] %s\n", tmp);
    }
    return rc;
}

/* Send 0xDD trigger repeatedly until 0xAA ACK or timeout_ms elapsed.       */
static uint8_t send_trigger_until_ack(const iap_hal_t* hal, uint32_t timeout_ms)
{
    uint32_t t0 = HAL_GetTick();
    while (HAL_GetTick() - t0 < timeout_ms) {
        uint8_t b = 0xDDu;
        hal->serial_write(&b, 1);

        uint8_t ack = 0;
        if (hal->serial_read(&ack, 1, 200u) == 1 && ack == IAP_ACK)
            return 1;
    }
    return 0;
}

/* Flush any stale bytes left in the RX buffer.                              */
static void drain_rx(const iap_hal_t* hal)
{
    uint8_t trash[16];
    int i;
    for (i = 0; i < 8; i++) {
        if (hal->serial_read(trash, sizeof trash, 50u) <= 0) break;
    }
}

/*--- Public API -------------------------------------------------------------*/

void gen_upd_scan_usb(void)
{
    DIR dir;
    FILINFO fno;

    if (f_opendir(&dir, "1:/GEN_UPDATE") != FR_OK) {
        printf("No generator update (GEN_UPDATE not found)\n");
        return;
    }
    while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
        size_t n = strlen(fno.fname);
        if (n > 4 && fno.fname[n-4] == '.' &&
            (fno.fname[n-3]=='h'||fno.fname[n-3]=='H') &&
            (fno.fname[n-2]=='e'||fno.fname[n-2]=='E') &&
            (fno.fname[n-1]=='x'||fno.fname[n-1]=='X')) {
            strncpy(s_hex_name, fno.fname, sizeof s_hex_name - 1);
            s_hex_name[sizeof s_hex_name - 1] = '\0';
            break;
        }
    }
    f_closedir(&dir);

    if (!s_hex_name[0]) {
        printf("GEN_UPDATE folder found but no .hex file inside\n");
        return;
    }

    printf("GEN_UPDATE: found %s, update pending\n", s_hex_name);
    s_pending = 1;
}

uint8_t gen_upd_is_pending(void)        { return s_pending;   }
gen_upd_state_t gen_upd_get_state(void) { return s_state;     }
uint8_t         gen_upd_get_progress(void){ return s_progress; }
const char*     gen_upd_get_msg(void)   { return s_msg;       }
const char*     gen_upd_get_hex_name(void) { return s_hex_name; }

void gen_upd_start(void)
{
    s_state    = GEN_UPD_IN_PROGRESS;
    s_progress = 0;
    s_blk_done = 0;
    set_msg("Starting...");
    osSemaphoreRelease(hGenUpdStart);
}

void gen_upd_cancel(void)
{
    s_pending = 0;
    s_state   = GEN_UPD_IDLE;
    osThreadResume(defaultTaskHandle);
}

void gen_upd_confirm_reset(void)
{
    HAL_NVIC_SystemReset();
}

/*--- Task -------------------------------------------------------------------*/

void gen_upd_task_fn(void)
{
    for (;;) {
        osSemaphoreAcquire(hGenUpdStart, osWaitForever);

        s_state    = GEN_UPD_IN_PROGRESS;
        s_progress = 0;
        s_blk_done = 0;

        /* ---- 1. Find and load first .hex file in GEN_UPDATE ------------ */
        set_msg("Loading firmware...");

        char hex_path[64] = "";
        {
            DIR     dir;
            FILINFO fno;
            if (f_opendir(&dir, "1:/GEN_UPDATE") == FR_OK) {
                while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
                    size_t n = strlen(fno.fname);
                    if (n > 4 &&
                        (fno.fname[n-4] == '.') &&
                        (fno.fname[n-3] == 'h' || fno.fname[n-3] == 'H') &&
                        (fno.fname[n-2] == 'e' || fno.fname[n-2] == 'E') &&
                        (fno.fname[n-1] == 'x' || fno.fname[n-1] == 'X')) {
                        snprintf(hex_path, sizeof hex_path,
                                 "1:/GEN_UPDATE/%s", fno.fname);
                        break;
                    }
                }
                f_closedir(&dir);
            }
        }

        if (hex_path[0] == '\0') {
            set_msg("ERROR: .hex not found");
            printf("[GEN_UPD] FAIL_RESOURCES: no .hex in GEN_UPDATE\n");
            s_state = GEN_UPD_FAIL_RESOURCES;
            continue;
        }

        printf("[GEN_UPD] Loading %s\n", hex_path);

        iap_hex_image_t img;
        memset(&img, 0, sizeof img);

        if (iap_hex_fatfs_load(&img, hex_path) != 0) {
            set_msg("ERROR: cannot read .hex");
            printf("[GEN_UPD] FAIL_RESOURCES\n");
            s_state = GEN_UPD_FAIL_RESOURCES;
            continue;
        }

        s_blk_total = img.count;
        printf("[GEN_UPD] %u blocks, xor=0x%02X\n",
               (unsigned)img.count, (unsigned)img.xor_all);

        /* ---- 2. Take over USART3 ---------------------------------------- */
        dep_o3_com_iap_takeover();

        const iap_hal_t* hal = iap_hal_stm32_get();

        /* ---- 3. RST en crudo + trigger inmediato -------------------------
         * Frame RST del protocolo O3 hardcodeado (CRC=02 determinista).
         * Un solo disparo sin esperar respuesta para no perder la ventana
         * del bootloader (~100 ms tras el reset del generador).            */
        set_msg("Resetting generator...");
        printf("[GEN_UPD] RST raw + 0xDD\n");
        static const uint8_t rst_frame[] = {'{','R','S','T',',','0','2','\r'};
        hal->serial_write(rst_frame, sizeof rst_frame);
        osDelay(10u);

        /* ---- 4. Trigger: send 0xDD until 0xAA (5 s window) ------------- */
        set_msg("Waiting for bootloader...");


        if (!send_trigger_until_ack(hal, 5000u)) {
            /* Device may already be in BL (0xDD is discarded silently).
             * Attempt iap_enter() directly before declaring failure.       */
            printf("[GEN_UPD] No ACK to 0xDD, trying iap_enter directly\n");
            set_msg("No ACK, trying direct ENTER...");
        }

        drain_rx(hal);

        /* ---- 5. IAP cycle ----------------------------------------------- */
        set_msg("Programming firmware...");
        printf("[GEN_UPD] Starting iap_run (%u blocks)\n", (unsigned)img.count);

        iap_hex_cursor_t cursor;
        iap_hex_cursor_init(&cursor, &img);

        iap_rc_t rc = iap_run(hal, progress_src, &cursor, img.xor_all);

        iap_hex_fatfs_free(&img);

        /* ---- 6. Result -------------------------------------------------- */
        if (rc == IAP_OK) {
            set_msg("Done! Remove USB drive and press Confirm to restart.");
            printf("[GEN_UPD] SUCCESS — waiting for user confirmation\n");
            s_progress = 100u;
            s_state    = GEN_UPD_SUCCESS;
        } else {
            char err[80];
            snprintf(err, sizeof err, "ERROR: %s", iap_strerror(rc));
            set_msg(err);
            printf("[GEN_UPD] FAIL_FLASH: %s\n", iap_strerror(rc));
            s_state = GEN_UPD_FAIL_FLASH;
            /* Device is left in BL (INVALIDATE flag set — safe to retry). */
        }
    }
}
