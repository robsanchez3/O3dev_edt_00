/*============================================================================
 * gen_updater.h -- High-level orchestrator for generator firmware update.
 *
 *  Detects "1:/GEN_UPDATE/TargetApp.hex" on the USB flash at startup,
 *  shows the GenUpdate screen, and programs the PSoC CY8C29466 control
 *  board via the IAP bootloader protocol over USART3 (57600 8N1).
 *===========================================================================*/
#ifndef GEN_UPDATER_H
#define GEN_UPDATER_H

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GEN_UPD_IDLE            = 0,
    GEN_UPD_WAIT_CONFIRM    = 1,  /* screen shown, waiting for OK/Cancel      */
    GEN_UPD_IN_PROGRESS     = 2,  /* update running                           */
    GEN_UPD_SUCCESS         = 3,  /* done, system will reset                  */
    GEN_UPD_FAIL_RESOURCES  = 4,  /* TargetApp.hex missing or unreadable      */
    GEN_UPD_FAIL_COMM       = 5,  /* bootloader did not respond               */
    GEN_UPD_FAIL_FLASH      = 6,  /* IAP write/checksum/validate error        */
} gen_upd_state_t;

/* Semaphore created in app_freertos.c; released by gen_upd_start().         */
extern osSemaphoreId_t hGenUpdStart;

/*--- Startup ----------------------------------------------------------------*/

/* Call from ConfigLoaderTask after USB is mounted.
 * Checks for "1:/GEN_UPDATE" directory; sets the pending flag if found.    */
void gen_upd_scan_usb(void);

/* Returns 1 if GEN_UPDATE folder was found; Model::tick() uses this to
 * navigate to the GenUpdate screen once at startup.                         */
uint8_t gen_upd_is_pending(void);

/*--- State (read from any task; volatile, no mutex) -------------------------*/

gen_upd_state_t gen_upd_get_state(void);
uint8_t         gen_upd_get_progress(void);    /* 0–100 %                    */
const char*     gen_upd_get_msg(void);         /* last status line           */
const char*     gen_upd_get_hex_name(void);    /* e.g. "firmware.hex"       */

/*--- Commands (from GenUpdatePresenter) -------------------------------------*/

/* Validates resources and releases hGenUpdStart to start the task.          */
void gen_upd_start(void);

/* Clears pending flag so Model::tick() resumes normal boot flow.            */
void gen_upd_cancel(void);

/* Triggers HAL_NVIC_SystemReset(); called from presenter after success.     */
void gen_upd_confirm_reset(void);

/*--- Task entry point -------------------------------------------------------*/

/* Runs inside GenUpdaterTask (infinite loop, blocks on hGenUpdStart).       */
void gen_upd_task_fn(void);

#ifdef __cplusplus
}
#endif

#endif /* GEN_UPDATER_H */
