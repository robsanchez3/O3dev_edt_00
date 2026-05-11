/*
 * log.c
 *
 * Therapy session log — stores last therapy header + all deb_printf output
 * to 0:/log/last_therapy.log on the SD card.
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

#define LOG_FILE_PATH   "0:/log/last_therapy.log"
#define LOG_MAX_BYTES   (512UL * 1024UL)

extern FSM_O3_OPERATION_T GLB_fsm_o3;

static volatile bool  s_enabled   = false;
static FIL            s_file;
static uint8_t        s_file_open = 0;
static osMutexId_t    s_mutex     = NULL;

/* ------------------------------------------------------------------ */

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
    f_write(&s_file, s, strlen(s), &bw);
}

static bool size_limit_reached(void)
{
    if (f_size(&s_file) < LOG_MAX_BYTES) return false;
    write_str("!!! LOG SIZE LIMIT REACHED - LOGGING STOPPED !!!\r\n");
    f_sync(&s_file);
    f_close(&s_file);
    s_file_open = 0;
    s_enabled   = false;
    return true;
}

/* ------------------------------------------------------------------ */

void log_set_enabled(bool enabled) { s_enabled = enabled; }
bool log_is_enabled(void)          { return s_enabled; }

int8_t log_start(const char *therapy_name)
{
    if (!s_enabled)  return 0;
    if (s_file_open) return 0;  /* already running — idempotent */

    if (s_mutex == NULL)
        s_mutex = osMutexNew(NULL);

    ensure_dir("0:/log");

    FRESULT res = f_open(&s_file, LOG_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return -1;
    s_file_open = 1;

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
    f_sync(&s_file);

    return 0;
}

void log_finish(LogResult_t result, int32_t error_code)
{
    if (!s_file_open) return;

    osMutexAcquire(s_mutex, osWaitForever);

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

    f_sync(&s_file);
    f_close(&s_file);
    s_file_open = 0;
    s_enabled   = false;

    osMutexRelease(s_mutex);
}

void log_raw_line(const char *line)
{
    if (!s_enabled || !s_file_open || s_mutex == NULL) return;

    osMutexAcquire(s_mutex, osWaitForever);

    if (!size_limit_reached())
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') len--;  /* strip trailing \n */

        UINT bw;
        f_write(&s_file, line, len, &bw);
        write_str("\r\n");
    }

    osMutexRelease(s_mutex);
}
