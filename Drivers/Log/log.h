/*
 * log.h
 *
 * Session logs stored on the SD card (0:/log/):
 *   last_therapy.log — therapy session, all deb_printf output.
 *   last_start.log   — boot process, STATE_INIT_CHECK_1 → WAITING_FOR_PROTOCOL/ERROR.
 *
 *  Created on: May 2026
 *      Author: Roberto.Sanchez
 */

#ifndef DRIVERS_LOG_LOG_H_
#define DRIVERS_LOG_LOG_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Therapy log (0:/log/last_therapy.log) ---- */

typedef enum {
    LOG_RESULT_OK = 0,
    LOG_RESULT_USER_CANCEL,
    LOG_RESULT_ERROR
} LogResult_t;

void    log_set_enabled(bool enabled);
bool    log_is_enabled(void);

int8_t  log_start(const char *therapy_name);
void    log_finish(LogResult_t result, int32_t error_code);
void    log_raw_line(const char *line);

/* ---- Boot process log (0:/log/last_start.log) ---- */

void    start_log_set_enabled(bool enabled);
bool    start_log_is_enabled(void);

#define START_LOG_OK    true
#define START_LOG_ERROR false

int8_t  start_log_start(void);
void    start_log_finish(bool success, int32_t error_code);
void    start_log_raw_line(const char *line);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_LOG_LOG_H_ */
