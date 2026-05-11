/*
 * log.h
 *
 * Therapy session log — stores last therapy header + all deb_printf output
 * to 0:/log/last_therapy.log on the SD card.
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

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_LOG_LOG_H_ */
