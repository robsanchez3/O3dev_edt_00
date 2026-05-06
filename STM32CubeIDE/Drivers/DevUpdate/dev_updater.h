/*============================================================================
 * dev_updater.h -- Generic device update state and service config import
 *                  confirmation via the DevUpdate screen.
 *
 *  Shared between the generator firmware updater (gen_updater.c) and the
 *  service config import flow in configLoaderTask (Model.cpp).
 *===========================================================================*/
#ifndef DEV_UPDATER_H
#define DEV_UPDATER_H

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DEV_UPD_IDLE            = 0,
    DEV_UPD_WAIT_CONFIRM    = 1,  /* screen shown, waiting for OK/Cancel      */
    DEV_UPD_IN_PROGRESS     = 2,  /* update running                           */
    DEV_UPD_SUCCESS         = 3,  /* done, system will reset                  */
    DEV_UPD_FAIL_RESOURCES  = 4,  /* TargetApp.hex missing or unreadable      */
    DEV_UPD_FAIL_COMM       = 5,  /* bootloader did not respond               */
    DEV_UPD_FAIL_FLASH      = 6,  /* IAP write/checksum/validate error        */
    DEV_UPD_IMPORT_CONFIRM  = 7,  /* waiting for user to confirm/skip a config import */
} dev_upd_state_t;

/*--- Generic state (read from any task; volatile, no mutex) -----------------*/

uint8_t         dev_upd_is_pending(void);
dev_upd_state_t dev_upd_get_state(void);
uint8_t         dev_upd_get_progress(void);    /* 0–100 %                    */
const char*     dev_upd_get_msg(void);         /* last status line           */

/* Clears pending flag so Model::tick() resumes normal boot flow.            */
void dev_upd_cancel(void);

/*--- Internal setters (called only by gen_updater.c) -----------------------*/

void dev_upd_set_pending(uint8_t v);
void dev_upd_set_state(dev_upd_state_t st);
void dev_upd_set_progress(uint8_t pct);
void dev_upd_set_msg(const char* m);

/*--- Service config import confirmation ------------------------------------*/

/* Call before the first dev_upd_import_request() to activate DevUpdate.    */
void    dev_upd_import_begin(void);

/* Call after the last dev_upd_import_request() to release DevUpdate.       */
void    dev_upd_import_end(void);

/* Blocks until the user presses OK (returns 1) or Cancel (returns 0).
 * dir_name: short name shown in the prompt (e.g. "Hw", "Menu").            */
int     dev_upd_import_request(const char* dir_name);

/* Called by DevUpdatePresenter OK button while import is active.            */
void    dev_upd_import_confirm(void);

/* Called by DevUpdatePresenter Cancel button while import is active.        */
void    dev_upd_import_skip(void);

uint8_t     dev_upd_import_is_active(void);
uint8_t     dev_upd_import_is_pending(void);
const char* dev_upd_import_get_dir(void);

#ifdef __cplusplus
}
#endif

#endif /* DEV_UPDATER_H */
