/**
  ******************************************************************************  
  * File Name          : edt_bsp_uart.h
  * @author            : EDT Embedded Application Team
  * Description        : This file contains the common defines and functions 
  *                      prototypes for the edt_bsp_uart.c driver.
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT(c) 2020 Emerging Display Technologies Corp.
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Emerging Display Technologies Corp. nor the names of 
  *      its contributors may be used to endorse or promote products derived from 
  *      this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EDT_BSP_UART_H
#define __EDT_BSP_UART_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"  
#include "main.h"   

enum 
{
  USED_USART_NONE =0,
  USED_USART_RS485,
  USED_USART_RS232,
};

extern UART_HandleTypeDef hRs485;
extern UART_HandleTypeDef hRs232;

void EDT_USART_Init(UART_HandleTypeDef *uarthandle, uint8_t USART_MODE ,uint32_t USART_BAUD);
void EDT_USART_DeInit(UART_HandleTypeDef *uarthandle);

HAL_StatusTypeDef EDT_UART_Transmit_IT(UART_HandleTypeDef *huart , uint8_t *pData ,uint16_t Size) ;  
HAL_StatusTypeDef EDT_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef EDT_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef EDT_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/*EDT Handler Can ReDefine Function for USER*/
void EDT_RS232_IRQHandler(void);
void EDT_RS232_TxCpltCallback(void);
void EDT_RS232_RxCpltCallback(void);
void EDT_RS232_ErrorCallback(void);
void EDT_RS485_IRQHandler(void);
void EDT_RS485_TxCpltCallback(void);
void EDT_RS485_RxCpltCallback(void);
void EDT_RS485_ErrorCallback(void); 
   
#ifdef __cplusplus
}
#endif

#endif /* __EDT_BSP_UART_H */

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/
