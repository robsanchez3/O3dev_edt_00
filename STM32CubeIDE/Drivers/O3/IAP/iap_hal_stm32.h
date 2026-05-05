/*============================================================================
 * iap_hal_stm32.h -- iap_hal_t implementation over USART3 (huart3, 57600 8N1).
 *
 *  Uses blocking HAL_UART_Transmit / HAL_UART_Receive.
 *  Call iap_hal_stm32_get() to obtain the singleton iap_hal_t pointer.
 *===========================================================================*/
#ifndef IAP_HAL_STM32_H
#define IAP_HAL_STM32_H

#include "iap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Returns pointer to the singleton iap_hal_t wired to huart3. */
const iap_hal_t* iap_hal_stm32_get(void);

#ifdef __cplusplus
}
#endif

#endif /* IAP_HAL_STM32_H */
