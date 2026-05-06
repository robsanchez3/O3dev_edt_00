/*============================================================================
 * iap_hal_stm32.c -- iap_hal_t wired to USART3 (huart3, 57600 8N1).
 *
 *  serial_write : HAL_UART_Transmit  (blocking, 1 s timeout)
 *  serial_read  : HAL_UART_Receive   byte-by-byte with osDelay(1) between
 *                 attempts so the FreeRTOS scheduler is not starved during
 *                 the long flash-write timeouts (up to 1 s per block).
 *  millis       : HAL_GetTick()
 *  log          : printf to SWV ITM
 *===========================================================================*/
#include "iap_hal_stm32.h"
#include "usart.h"
#include "stm32u5xx_hal.h"
#include "cmsis_os2.h"
#include <stdio.h>

extern UART_HandleTypeDef huart3;

/*---------------------------------------------------------------------------*/

static int stm32_serial_write(const uint8_t* buf, size_t n)
{
    HAL_StatusTypeDef s = HAL_UART_Transmit(&huart3, (uint8_t*)buf, (uint16_t)n, 1000);
    return (s == HAL_OK) ? (int)n : -1;
}

static int stm32_serial_read(uint8_t* buf, size_t n, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    size_t   got   = 0;

    while (got < n) {
        if (HAL_GetTick() - start >= timeout_ms) break;

        HAL_StatusTypeDef s = HAL_UART_Receive(&huart3, buf + got, 1, 2u);
        if (s == HAL_OK) {
            got++;
        } else {
            osDelay(1);  /* yield to other tasks while waiting for next byte */
        }
    }
    return (int)got;
}

static uint32_t stm32_millis(void)
{
    return HAL_GetTick();
}

static void stm32_log(const char* tag, const char* msg)
{
    printf("[%s] %s\n", tag, msg);
}

/*---------------------------------------------------------------------------*/

static const iap_hal_t s_hal = {
    .serial_write = stm32_serial_write,
    .serial_read  = stm32_serial_read,
    .millis       = stm32_millis,
    .log          = stm32_log,
};

const iap_hal_t* iap_hal_stm32_get(void)
{
    return &s_hal;
}
