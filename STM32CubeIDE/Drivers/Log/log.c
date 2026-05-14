/*
 * log.c
 *
 * Session logs stored on the SD card (0:/log/):
 *   last_therapy.log — therapy session, all deb_printf output.
 *   last_start.log   — start boot process, STATE_INIT_CHECK_1 → WAITING_FOR_PROTOCOL/ERROR.
 *
 *  Created on: May 2026
 *      Author: Roberto.Sanchez
 */

#include "log.h"
#include "../O3/Fsm_o3/fsm_o3_operation.h"
#include "ff.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>

/* ================================================================== */
/* Last therapy log — 0:/log/last_therapy.log                         */
/* ================================================================== */
#define LOG_FILE_PATH   "0:/log/last_therapy.log"
#define LOG_MAX_BYTES   (512UL * 1024UL)

extern FSM_O3_OPERATION_T GLB_fsm_o3;

static volatile bool  tl_enabled   = false;
static FIL            tl_file;
static uint8_t        tl_file_open = 0;
static osMutexId_t    tl_mutex     = NULL;


static FRESULT ensure_dir(const char *path)
{
    DIR dir;
    FRESULT fr = f_opendir(&dir, path);
    if (fr == FR_OK) { f_closedir(&dir); return FR_OK; }
    if (fr == FR_NO_PATH) fr = f_mkdir(path);
    return fr;
}

static void write_str(const char *s)
{
    UINT bw;
    f_write(&tl_file, s, strlen(s), &bw);
}

static bool size_limit_reached(void)
{
    if (f_size(&tl_file) < LOG_MAX_BYTES) return false;
    write_str("!!! LOG SIZE LIMIT REACHED - LOGGING STOPPED !!!\r\n");
    f_sync(&tl_file);
    f_close(&tl_file);
    tl_file_open = 0;
    tl_enabled   = false;
    return true;
}


void log_set_enabled(bool enabled) { tl_enabled = enabled; }
bool log_is_enabled(void)          { return tl_enabled; }

int8_t log_start(const char *therapy_name)
{
    if (!tl_enabled)  return 0;
    if (tl_file_open) return 0;  /* already running — idempotent */

    if (tl_mutex == NULL)
        tl_mutex = osMutexNew(NULL);

    ensure_dir("0:/log");

    FRESULT res = f_open(&tl_file, LOG_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return -1;
    tl_file_open = 1;

    char line[128];

    write_str("=== THERAPY LOG ===\r\n");

    snprintf(line, sizeof(line), "Therapy    : %s\r\n",     therapy_name);
    write_str(line);
    snprintf(line, sizeof(line), "O3 Conc    : %u ug/ml\r\n",  GLB_fsm_o3.ConfiguredO3Concentration);
    write_str(line);
    snprintf(line, sizeof(line), "Flow       : %u l/h\r\n",    GLB_fsm_o3.ConfiguredFlow);
    write_str(line);
    snprintf(line, sizeof(line), "Time       : %u min\r\n",    GLB_fsm_o3.ConfiguredTime);
    write_str(line);
    snprintf(line, sizeof(line), "Volume     : %lu ml\r\n",    (unsigned long)GLB_fsm_o3.ConfiguredVolume);
    write_str(line);
    snprintf(line, sizeof(line), "Dose       : %u ug\r\n",     GLB_fsm_o3.ConfiguredDose);
    write_str(line);
    snprintf(line, sizeof(line), "Pressure   : %u mbar\r\n",   GLB_fsm_o3.ConfiguredPressure);
    write_str(line);
    snprintf(line, sizeof(line), "Vacuum Time: %u s\r\n",      GLB_fsm_o3.ConfiguredVacuumTime);
    write_str(line);
    snprintf(line, sizeof(line), "Vacuum Pres: %u mbar\r\n",   GLB_fsm_o3.ConfiguredVacuumPressure);
    write_str(line);

    write_str("---\r\n");
    f_sync(&tl_file);

    return 0;
}

void log_finish(LogResult_t result, int32_t error_code)
{
    if (!tl_file_open) return;

    osMutexAcquire(tl_mutex, osWaitForever);

    write_str("---\r\n");

    char line[64];
    switch (result) {
    case LOG_RESULT_OK:
        write_str("Result: ok\r\n");
        break;
    case LOG_RESULT_USER_CANCEL:
        write_str("Result: user cancel\r\n");
        break;
    default:
        snprintf(line, sizeof(line), "Result: error %ld\r\n", error_code);
        write_str(line);
        break;
    }

    f_sync(&tl_file);
    f_close(&tl_file);
    tl_file_open = 0;
    tl_enabled   = false;

    osMutexRelease(tl_mutex);
}

void log_raw_line(const char *line)
{
    if (!tl_enabled || !tl_file_open || tl_mutex == NULL) return;

    osMutexAcquire(tl_mutex, osWaitForever);

    if (!size_limit_reached())
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') len--;  /* strip trailing \n */

        UINT bw;
        f_write(&tl_file, line, len, &bw);
//      write_str("\r\n");
    }

    osMutexRelease(tl_mutex);
}

/* ================================================================== */
/* Start boot process log — 0:/log/last_start.log                     */
/* ================================================================== */

#define SL_FILE_PATH    "0:/log/last_start.log"
#define SL_MAX_BYTES    (512UL * 1024UL)

static volatile bool  sl_enabled   = false;
static FIL            sl_file;
static uint8_t        sl_file_open = 0;
static osMutexId_t    sl_mutex     = NULL;

static void sl_write_str(const char *s)
{
    UINT bw;
    f_write(&sl_file, s, strlen(s), &bw);
}

static bool sl_size_limit_reached(void)
{
    if (f_size(&sl_file) < SL_MAX_BYTES) return false;
    sl_write_str("!!! LOG SIZE LIMIT REACHED - LOGGING STOPPED !!!\r\n");
    f_sync(&sl_file);
    f_close(&sl_file);
    sl_file_open = 0;
    sl_enabled   = false;
    return true;
}

void start_log_set_enabled(bool enabled) { sl_enabled = enabled; }
bool start_log_is_enabled(void)          { return sl_enabled; }

int8_t start_log_start(void)
{
    if (!sl_enabled)  return 0;
    if (sl_file_open) return 0;  /* idempotent — called every tick during STATE_INIT_CHECK_1 */

    if (sl_mutex == NULL)
        sl_mutex = osMutexNew(NULL);

    ensure_dir("0:/log");

    FRESULT res = f_open(&sl_file, SL_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return -1;
    sl_file_open = 1;

    sl_write_str("=== LAST START BOOT LOG ===\r\n");
    sl_write_str("---\r\n");
    f_sync(&sl_file);

    return 0;
}

void start_log_finish(bool success, int32_t error_code)
{
    if (!sl_file_open) return;  /* idempotent */

    osMutexAcquire(sl_mutex, osWaitForever);

    sl_write_str("---\r\n");

    if (success) {
        sl_write_str("Result: ok\r\n");
    } else {
        char line[64];
        snprintf(line, sizeof(line), "Result: error %ld\r\n", error_code);
        sl_write_str(line);
    }

    f_sync(&sl_file);
    f_close(&sl_file);
    sl_file_open = 0;
    sl_enabled   = false;

    osMutexRelease(sl_mutex);
}

void start_log_raw_line(const char *line)
{
    if (!sl_enabled || !sl_file_open || sl_mutex == NULL) return;

    osMutexAcquire(sl_mutex, osWaitForever);

    if (!sl_size_limit_reached())
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') len--;  /* strip trailing \n */

        UINT bw;
        f_write(&sl_file, line, len, &bw);
        //sl_write_str("\r\n");
    }

    osMutexRelease(sl_mutex);
}
