/**
  ******************************************************************************  
  * File Name          : edt_bsp_uart.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides a set of functions needed to manage 
  *                RS232, RS485 and UART driver on smart embedded display board.  
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
/* Includes ------------------------------------------------------------------*/
#include "edt_bsp_uart.h" 
#include <stdio.h>

#if defined (RS485_DEBUG)||defined(RS232_DEBUG)
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
#endif

// rsm test UART_HandleTypeDef hRs485;
// rsm test UART_HandleTypeDef hRs232;
UART_HandleTypeDef hRs485 = {
  .Instance = RS485_INSTANCE,
};

UART_HandleTypeDef hRs232 = {
  .Instance = RS232_INSTANCE,
};
/*****************************************************************************
  * @brief  EDT_USART_Init
  * @param  uarthandle  uart handler
  * @param  USART_MODE  RS485/ RS232
  * @param  USART_BAUD  uart BaudRate
  * @retval None
******************************************************************************/
void EDT_USART_Init(UART_HandleTypeDef *uarthandle, uint8_t USART_MODE ,uint32_t USART_BAUD)
{ 
  if ( USART_MODE == USED_USART_RS485 ) 
  {
    EDT_USART_DeInit(uarthandle);
    uarthandle->Instance = hRs485.Instance;     //USART_RS485_INSTANCE;
    uarthandle->Init.BaudRate = USART_BAUD;  
    uarthandle->Init.WordLength = UART_WORDLENGTH_8B;
    uarthandle->Init.StopBits = UART_STOPBITS_1;
    uarthandle->Init.Parity = UART_PARITY_NONE;
    uarthandle->Init.Mode = UART_MODE_TX_RX;
    uarthandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uarthandle->Init.OverSampling = UART_OVERSAMPLING_16;
    uarthandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uarthandle->AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_NO_INIT;
    if (HAL_RS485Ex_Init(uarthandle, UART_DE_POLARITY_HIGH, 0 , 0) != HAL_OK) {
    
     }
    HAL_NVIC_SetPriority(RS485_IRQn,RS485_IRQ_SetPriority, 0);
    HAL_NVIC_EnableIRQ(RS485_IRQn);
  }
  else if ( USART_MODE == USED_USART_RS232 ) {
    EDT_USART_DeInit(uarthandle);
    uarthandle->Instance = hRs232.Instance;     // USART_RS232_INSTANCE;
    uarthandle->Init.BaudRate = USART_BAUD;  
    uarthandle->Init.WordLength = UART_WORDLENGTH_8B;
    uarthandle->Init.StopBits = UART_STOPBITS_1;
    uarthandle->Init.Parity = UART_PARITY_NONE;
    uarthandle->Init.Mode = UART_MODE_TX_RX;
    uarthandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uarthandle->Init.OverSampling = UART_OVERSAMPLING_16;
    uarthandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uarthandle->AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(uarthandle) != HAL_OK)
    {
     
    }
    HAL_NVIC_SetPriority(RS232_IRQn, RS232_IRQ_SetPriority ,0);
    HAL_NVIC_EnableIRQ(RS232_IRQn);
  }
}

/*****************************************************************************
  * @brief  EDT_USART_DeInit
  * @param  uarthandle  uart handler
  * @param  USART_MODE  RS485/ RS232
  * @param  USART_BAUD  uart BaudRate
  * @retval None
******************************************************************************/
void EDT_USART_DeInit(UART_HandleTypeDef *uarthandle)
{ 
    if (HAL_UART_DeInit(uarthandle) != HAL_OK)
    {
    }
}

/*****************************************************************************
  * @brief UART EDT USER USED Tramsmite Interrupt
  *        This function frees the hardware resources used in this example:
  * @param huart   : UART handle pointer
  *        pdata   : UART Tramsmite Data pointer
  *        Size    : UART Tramsmite Data Length
  *        Timeout : UART Tramsmite wait time out
  * @retval HAL_StatusTypeDef
******************************************************************************/
__weak HAL_StatusTypeDef EDT_UART_Transmit_IT(UART_HandleTypeDef *huart , uint8_t *pData ,uint16_t Size)
{ 
       HAL_StatusTypeDef  Status  =  HAL_ERROR;  
       Status = HAL_UART_Transmit_IT(huart,pData,Size);
       return Status;
}
/*****************************************************************************
  * @brief UART EDT USER USED Receive Interrupt
  *        This function frees the hardware resources used in this example:
  * @param huart   : UART handle pointer
  *        pdata   : UART Receive Data pointer
  *        Size    : UART Receive Data Length 
  *        Timeout : UART Receive wait time out
  * @retval HAL_StatusTypeDef
******************************************************************************/
__weak HAL_StatusTypeDef EDT_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
       HAL_StatusTypeDef  Status  =  HAL_ERROR;  
       HAL_UART_Receive_IT(huart,pData,Size);
       return Status;
}
/*****************************************************************************
  * @brief UART EDT USER USED Tramsmite
  *        This function frees the hardware resources used in this example:
  * @param huart   : UART handle pointer
  *        pdata   : UART Tramsmite Data pointer
  *        Size    : UART Tramsmite Data Length 
  *        Timeout : UART Tramsmite wait time out
  * @retval HAL_StatusTypeDef
******************************************************************************/
HAL_StatusTypeDef EDT_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  return HAL_UART_Transmit(huart, pData, Size, Timeout);
}

/*****************************************************************************
  * @brief UART EDT USER USED Receive Interrupt
  *        This function frees the hardware resources used in this example:
  * @param huart   : UART handle pointer
  *        pdata   : UART Receive Data pointer
  *        Size    : UART Receive Data Length 
  *        Timeout : UART Receive wait time out
  * @retval HAL_StatusTypeDef
******************************************************************************/
HAL_StatusTypeDef EDT_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  return HAL_UART_Receive(huart, pData,  Size,  Timeout);
}
/*****************************************************************************
  USED RS232 IRQHandler  FUNCTION
*****************************************************************************/
//void RS232_IRQHandler(void)
//{ 
//    EDT_RS232_IRQHandler();
//}
///*****************************************************************************
//  USED RS485 IRQHandler  FUNCTION 
//*****************************************************************************/
// void RS485_IRQHandler(void)   
//{ 
//  EDT_RS485_IRQHandler();
//}
/*****************************************************************************
 USED BYTE Transmit Finlish IrqCallBack 
*****************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart==&hRs485){
    EDT_RS485_TxCpltCallback();
  }
  if(huart==&hRs232){
    EDT_RS232_TxCpltCallback();
  }
}
/*****************************************************************************
  USED BYTE Receive Finlish IrqCallBack 
*****************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{  
  /* USER CODE IRQn HAL_UART_RxCpltCallback*/
 if(huart==&hRs485){
    EDT_RS485_RxCpltCallback();
  }
  if(huart==&hRs232){
    EDT_RS232_RxCpltCallback();
  }
}
/*****************************************************************************
  USER CODE IRQn HAL_UART_ErrorCallback 
*****************************************************************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{  
 if(huart==&hRs485){
   EDT_RS485_ErrorCallback();
  }
  if(huart==&hRs232){  
   EDT_RS232_ErrorCallback();
  }
}

/*****************************************************************************
  RS232_IRQ_Callback  FUNCTION
*****************************************************************************/
__weak void EDT_RS232_IRQHandler(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the RS232_IRQ_Callback can be implemented in the user file
   */
  HAL_UART_IRQHandler(&hRs232);  
}
/*****************************************************************************
  RS485_IRQ_Callback  FUNCTION
*****************************************************************************/
__weak void EDT_RS485_IRQHandler(void)
{ 
  /* NOTE : This function should not be modified, when the callback is needed,
            the RS485_IRQ_Callback can be implemented in the user file 
  */
   HAL_UART_IRQHandler(&hRs485);  
}
/*****************************************************************************
  USER RS232_Transmite_Callback FUNCTION
*****************************************************************************/
__weak void EDT_RS232_TxCpltCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the EDT_RS232_TxCpltCallback can be implemented in the user file
   */
}
/*****************************************************************************
  USER RS485_IRQ_Callback FUNCTION
*****************************************************************************/
__weak void EDT_RS485_TxCpltCallback(void)
{  
  /* NOTE : This function should not be modified, when the callback is needed,
            the EDT_RS485_TxCpltCallback can be implemented in the user file
   */
}
/*****************************************************************************
  RS232_IRQ_Callback  FUNCTION
*****************************************************************************/
__weak void EDT_RS232_RxCpltCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the EDT_RS232_RxCpltCallback can be implemented in the user file
   */
}

/*****************************************************************************
  RS485_IRQ_Callback  FUNCTION
*****************************************************************************/
__weak void EDT_RS485_RxCpltCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the EDT_RS485_RxCpltCallback can be implemented in the user file
   */
}
/*****************************************************************************
  EDT_RS232_ErrorCallback  FUNCTION
*****************************************************************************/
__weak void EDT_RS232_ErrorCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the RS232_UART_ErrorCallback can be implemented in the user file
            Add ORE FLAG Process
   */
  uint8_t i = 0;
    if(__HAL_UART_GET_FLAG(&hRs232,UART_FLAG_ORE) != RESET) 
    {
        __HAL_UART_CLEAR_OREFLAG(&hRs232);
        HAL_UART_Receive_IT(&hRs232,(uint8_t *)&i,1);
    }
  
}
/*****************************************************************************
  EDT_RS485_ErrorCallback  FUNCTION
*****************************************************************************/
__weak void EDT_RS485_ErrorCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the EDT_RS485_RxCpltCallback can be implemented in the user file
             Add ORE FLAG Process
   */
   uint8_t i = 0;
   
    if(__HAL_UART_GET_FLAG(&hRs485,UART_FLAG_ORE) != RESET) 
    {
        __HAL_UART_CLEAR_OREFLAG(&hRs485);
        HAL_UART_Receive_IT(&hRs485,(uint8_t *)&i,1);
    }
}
/*****************************************************************************
  EDT UART DEBUG PRINTF
*****************************************************************************/
#if defined (RS485_DEBUG)||defined(RS232_DEBUG)
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character and Loop until the end of transmission */
#if defined (RS485_DEBUG)
  EDT_UART_Transmit(&hRs485,(uint8_t *)&ch, 1,50);
#else
  EDT_UART_Transmit(&hRs232,(uint8_t *)&ch, 1,50);
#endif
  return ch;
}

GETCHAR_PROTOTYPE
{
  uint8_t ch = 0;
  /* Wait for reception of a character on the USART RX line and echo this
   * character on console */
#if defined (RS485_DEBUG)
    /* Clear the Overrun flag just before receiving the first character */
  __HAL_UART_CLEAR_OREFLAG(&hRs485);
  EDT_UART_Receive(&hRs485,(uint8_t *)&ch, 1,50);
    EDT_UART_Transmit(&hRs485,(uint8_t *)&ch, 1,50);    //test
#else
      __HAL_UART_CLEAR_OREFLAG(&hRs232);
  EDT_UART_Receive(&hRs232,(uint8_t *)&ch, 1,50);
  EDT_UART_Transmit(&hRs232,(uint8_t *)&ch, 1,50);    //test
#endif
    return ch;
}
#endif
#ifdef __cplusplus
}
#endif

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/


