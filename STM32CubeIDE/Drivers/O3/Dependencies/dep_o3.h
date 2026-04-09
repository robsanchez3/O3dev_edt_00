/*
 * dep_o3.h
 *
 * O3 library platform dependencies interface.
 * Consolidates COM and Delay abstractions.
 * Each platform provides its own dep_o3_<platform>.c implementing these functions.
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */

#ifndef DEP_O3_H_
#define DEP_O3_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---- COM ---- */

void dep_o3_com_init(void);
void dep_o3_com_clear(void);
void dep_o3_com_sendConst(const char *);
void dep_o3_com_send(uint8_t *);
void dep_o3_com_putString(uint8_t *Data);
void dep_o3_com_pollRx(void);
uint8_t dep_o3_com_getChar(uint8_t *);
uint8_t dep_o3_com_dataAvailable(void);

/* ---- Delay ---- */

void dep_o3_delay_ms(uint32_t ms);

/* ---- Debug output ---- */

#include <stdarg.h>

void dep_o3_printf(const char *fmt, ...);
void dep_o3_vprintf(const char *fmt, va_list args);

/* ---- Platform ---- */

void dep_o3_softwareReset(void);
void dep_o3_beep(void);

#ifdef __cplusplus
}
#endif

#endif /* DEP_O3_H_ */
