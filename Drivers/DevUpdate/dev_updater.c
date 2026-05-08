/*============================================================================
 * dev_updater.c -- Generic device update state and service config import.
 *===========================================================================*/
#include "dev_updater.h"
#include "cmsis_os2.h"
#include <string.h>

extern osThreadId_t defaultTaskHandle;

/*--- Shared state (written from gen_updater.c, read from TouchGFX task) ----*/

static volatile uint8_t          s_pending  = 0;
static volatile dev_upd_state_t  s_state    = DEV_UPD_IDLE;
static volatile uint8_t          s_progress = 0;
static char                      s_msg[96]  = "";

/*--- Service config import confirmation -------------------------------------*/

static osSemaphoreId_t   s_hImportConfirm = NULL;
static volatile uint8_t  s_import_active  = 0;
static volatile uint8_t  s_import_pending = 0;
static volatile uint8_t  s_import_result  = 0;
static char              s_import_dir[32] = "";

/*--- Generic state ----------------------------------------------------------*/

uint8_t         dev_upd_is_pending(void)   { return s_pending;  }
dev_upd_state_t dev_upd_get_state(void)    { return s_state;    }
uint8_t         dev_upd_get_progress(void) { return s_progress; }
const char*     dev_upd_get_msg(void)      { return s_msg;      }

void dev_upd_cancel(void)
{
    s_pending = 0;
    s_state   = DEV_UPD_IDLE;
    osThreadResume(defaultTaskHandle);
}

/*--- Internal setters -------------------------------------------------------*/

void dev_upd_set_pending(uint8_t v)        { s_pending  = v;  }
void dev_upd_set_state(dev_upd_state_t st) { s_state    = st; }
void dev_upd_set_progress(uint8_t pct)     { s_progress = pct; }

void dev_upd_set_msg(const char* m)
{
    strncpy(s_msg, m, sizeof s_msg - 1);
    s_msg[sizeof s_msg - 1] = '\0';
}

/*--- Service config import confirmation -------------------------------------*/

void dev_upd_import_begin(void)
{
    if (s_hImportConfirm == NULL)
        s_hImportConfirm = osSemaphoreNew(1, 0, NULL);
    s_import_active = 1;
}

void dev_upd_import_end(void)
{
    s_import_pending = 0;
    s_import_active  = 0;
}

int dev_upd_import_request(const char* dir_name)
{
    strncpy(s_import_dir, dir_name, sizeof s_import_dir - 1);
    s_import_dir[sizeof s_import_dir - 1] = '\0';
    s_import_result  = 0;
    s_import_pending = 1;  /* LAST: signals UI that prompt is ready */
    osSemaphoreAcquire(s_hImportConfirm, osWaitForever);
    return (int)s_import_result;
}

void dev_upd_import_confirm(void)
{
    s_import_result  = 1;
    s_import_pending = 0;
    osSemaphoreRelease(s_hImportConfirm);
}

void dev_upd_import_skip(void)
{
    s_import_result  = 0;
    s_import_pending = 0;
    osSemaphoreRelease(s_hImportConfirm);
}

uint8_t     dev_upd_import_is_active(void)  { return s_import_active;  }
uint8_t     dev_upd_import_is_pending(void) { return s_import_pending; }
const char* dev_upd_import_get_dir(void)    { return s_import_dir;     }

/*--- Log export confirmation ------------------------------------------------*/

static osSemaphoreId_t   s_hExportConfirm = NULL;
static volatile uint8_t  s_export_active  = 0;
static volatile uint8_t  s_export_pending = 0;
static volatile uint8_t  s_export_result  = 0;
static volatile uint8_t  s_export_notify  = 0;
static char              s_export_msg[64] = "";

void dev_upd_export_begin(void)
{
    if (s_hExportConfirm == NULL)
        s_hExportConfirm = osSemaphoreNew(1, 0, NULL);
    s_export_active = 1;
}

void dev_upd_export_end(void)
{
    s_export_pending = 0;
    s_export_active  = 0;
}

int dev_upd_export_request(const char* msg)
{
    strncpy(s_export_msg, msg, sizeof s_export_msg - 1);
    s_export_msg[sizeof s_export_msg - 1] = '\0';
    s_export_result  = 0;
    s_export_pending = 1;  /* LAST: signals UI that prompt is ready */
    osSemaphoreAcquire(s_hExportConfirm, osWaitForever);
    return (int)s_export_result;
}

void dev_upd_export_confirm(void)
{
    s_export_result  = 1;
    s_export_pending = 0;
    osSemaphoreRelease(s_hExportConfirm);
}

void dev_upd_export_skip(void)
{
    s_export_result  = 0;
    s_export_pending = 0;
    osSemaphoreRelease(s_hExportConfirm);
}

void dev_upd_export_notify(const char* msg)
{
    strncpy(s_export_msg, msg, sizeof s_export_msg - 1);
    s_export_msg[sizeof s_export_msg - 1] = '\0';
    s_export_notify  = 1;
    s_export_pending = 1;  /* LAST: signals UI that prompt is ready */
    osSemaphoreAcquire(s_hExportConfirm, osWaitForever);
    s_export_notify  = 0;
}

uint8_t     dev_upd_export_is_active(void)  { return s_export_active;  }
uint8_t     dev_upd_export_is_pending(void) { return s_export_pending; }
uint8_t     dev_upd_export_is_notify(void)  { return s_export_notify;  }
const char* dev_upd_export_get_msg(void)    { return s_export_msg;     }
