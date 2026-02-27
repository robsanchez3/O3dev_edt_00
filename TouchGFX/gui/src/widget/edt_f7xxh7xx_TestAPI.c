/**
  ******************************************************************************  
  * File Name          : edt_f7xxh7xx_TESTAPI.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the TESTAPI instances.  
  * @version           : V1.0.1
  * @date              : 29-Oct-2020
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 EDT</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
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

#ifndef SIMULATOR
  #include "gui/widget/edt_f7xxh7xx_TESTAPI.h"    
  #include "gui/widget/edt_f7xxh7xx_USARTAPI.h"    
  #include "usbd_cdc_if.h"
  #include "cmsis_os.h"
  #if defined(STM32H750xx)
    #include "tcpecho.h"
  #endif

#include <stdio.h>
#include <string.h>


extern UART_HandleTypeDef hRs485;
extern UART_HandleTypeDef hRs232;

uint8_t spiBuf[2];
uint8_t canBuf[2];
uint8_t i2cBuf[2];

USARTRevSt  _RS232RevSt;
USARTRevSt  _RS485RevSt;

#if defined(STM32H750xx)
/*********************************************************************************
  * @brief   CheckEthnernetReceive USED STM32H750 EVK 
  * @param   None
  * @retval  true:Ethernet Receive Data Ready / false:Ethernet Receive Data Not Ready
  * channing for enthernet 20190906
**********************************************************************************/
bool CheckEthnernetReceive(void)
{ 
  if(Eth_St.ETH_EN==true)
  {
    Eth_St.ETH_EN =false;
    if((Eth_St.ETH_Buf[0]==0xaa)&&(Eth_St.ETH_Buf[1]==0x55))
    {      
      return true;
    } 
  }
  return false;
}
#endif
/******************************************************************************
  * @brief  TestAPI_ThreadInit (Create RS232/RS485 Task and Receive Interrupt)
  * @param  None
  * @retval None
*******************************************************************************/
void TestAPI_ThreadInit(void)
{ 
    __HAL_UART_ENABLE_IT(&hRs485, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&hRs232, UART_IT_RXNE);
    
    USARTAPI_Init(&hRs485);
    USARTAPI_Init(&hRs232);
    
    CreateUsart_Task_Init();
}
/******************************************************************************
  * @brief   Transmit_AskTestMode
  * @param   None
  * @retval  None
  *****************************************************************************/
void Transmit_AskTestMode(void)
{
   USART_FORMAT formatTx;
  
           formatTx.head    = BOOT_HEAD;
           formatTx.len     = 4;
           formatTx.nodeid  = BOOT_ID;
           formatTx.command = 0xA0;
           formatTx._xor =UsartXOR(formatTx);
           formatTx._sum =UsartSUM(formatTx);
           
           SendUartData(&hRs485,formatTx);
}
/******************************************************************************
  * @brief  CheckRS232Receive
  * @param  None
  * @retval true:RS232 Receive Data Ready / false:RS232 Receive Data Not Ready
*******************************************************************************/
bool CheckRS232Receive(void)
{
 if(pRs232 != NULL)
 {
   if(_RS232RevSt.RevF==true)
    {
    _RS232RevSt.RevF=false;
    if((_RS232RevSt.pdata[0]==0xaa)&&(_RS232RevSt.pdata[1]==0x55))
    {
      memset(&_RS232RevSt,0,sizeof(_RS232RevSt));
      return true;
    }    
   }
  }
  return false;
}
/******************************************************************************
  * @brief  CheckCDCUSBReceive
  * @param  None
  * @retval true:CDC USB Receive Data Ready /false :CDC USB Receive Data Not Ready
  * channing
*******************************************************************************/
bool CheckCDCUSBReceive(void)
{ 
  if(USBCDCHandleSt.CDC_HandleDataEn)
  {
    USBCDCHandleSt.CDC_HandleDataEn =false;
    if((USBCDCHandleSt.CDC_HandleData[0]==0xaa)&&(USBCDCHandleSt.CDC_HandleData[1]==0x55))
    {
      memset(USBCDCHandleSt.CDC_HandleData,0,sizeof(USBCDCHandleSt.CDC_HandleData));
      USBCDCHandleSt.CDC_HandleDataSize=0;
      return true;
    } 
  }
  return false;
}
/******************************************************************************
  * @brief  CheckI2CReceive
  * @param  None
  * @retval true:I2C Receive Data Ready/false:I2C Receive Data Not Ready
*******************************************************************************/
bool CheckI2CReceive(void)
{
   EDT_I2C_Master_Receive(&I2cHandle, 0xa5, (uint8_t*)i2cBuf, 2, 10);
   if((i2cBuf[0] ==0xaa)&&(i2cBuf[1]==0x55))
   {
      memset(i2cBuf,0,2);
      return true;
   }
   return false;
}
/******************************************************************************
  * @brief  CheckCANBUSReceive
  * @param  None
  * @retval true:Can Bus Receive Data Ready/false:Can Bus Receive Data Not Ready
*******************************************************************************/
bool CheckCANBUSReceive(void)
{
#if defined(STM32H750xx)
   EDT_FDCANx_Receive_Msg(&hfdcan1,(uint8_t*)canBuf ,2 );
#else
   EDT_CAN_Receive(&CanHandle,(uint8_t*)canBuf ,2 );
#endif
   if((canBuf[0] ==0xaa)&&(canBuf[1]==0x55))
   {
       memset(canBuf,0,2);
      return true;
   }
   return false;
}
/******************************************************************************
  * @brief CheckSPIReceive
  * @param  None
  * @retval true:SPI Receive Data Ready/false:SPI Receive Data Not Ready
*****************************************************************************/
bool CheckSPIReceive(void)
{ 
   if(EDT_SPI_Receive(&hspi1, (uint8_t*)spiBuf, 2 ,50)==HAL_OK)
   {
     if((spiBuf[0]==0xaa)&&(spiBuf[1]==0x55))
  //   memset(spiBuf,0,2);
       return true;
   }
   return false;
}
/************************ (C) COPYRIGHT EDT *****END OF FILE****/

#endif
