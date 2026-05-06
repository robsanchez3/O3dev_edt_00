/*============================================================================
 * gen_updater.h -- Generator firmware update (PSoC CY8C29466 via IAP/USART3).
 *
 *  Full update sequence:
 *    1. gen_upd_scan_usb()     — detect "1:/GEN_UPDATE/*.hex" at startup
 *    2. gen_upd_start()        — released by user OK on DevUpdate screen
 *    3. gen_upd_task_fn()      — load hex → RST → 0xDD handshake → iap_run()
 *    4. gen_upd_confirm_reset()— HAL_NVIC_SystemReset() after user confirms
 *===========================================================================*/
#ifndef GEN_UPDATER_H
#define GEN_UPDATER_H

#include "../DevUpdate/dev_updater.h"   /* re-exports dev_upd_state_t and generic API     */

#ifdef __cplusplus
extern "C" {
#endif

/* Semaphore created in app_freertos.c; released by gen_upd_start() on OK.  */
extern osSemaphoreId_t hGenUpdStart;

/* Call from configLoaderTask after USB is mounted.
 * Checks for "1:/GEN_UPDATE" directory; sets the pending flag if found.    */
void gen_upd_scan_usb(void);

/* Returns the .hex filename found in GEN_UPDATE (e.g. "TargetApp.hex").    */
const char* gen_upd_get_hex_name(void);

/* Validates resources and releases hGenUpdStart to start the task.         */
void gen_upd_start(void);

/* Triggers HAL_NVIC_SystemReset(); called from presenter after success.    */
void gen_upd_confirm_reset(void);

/* Runs inside GenUpdaterTask (infinite loop, blocks on hGenUpdStart).      */
void gen_upd_task_fn(void);

#ifdef __cplusplus
}
#endif

#endif /* GEN_UPDATER_H */
