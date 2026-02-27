/**
  ******************************************************************************  
  * File Name          : edt_f7xxh7xx_TestAPI.h
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the TestAPI instances.  
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

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EDT_F7XXH7XX_TESTAPI_H
#define __EDT_F7XXH7XX_TESTAPI_H

#ifndef SIMULATOR

#include "stdbool.h"    
#if defined(STM32H750xx)
#include "stm32h7xx_hal.h"
#else
#include "stm32f7xx_hal.h"
#endif
    
#ifdef __cplusplus
 extern "C" {
#endif
   
typedef struct _I2CRevSt_
{
  bool RevF;
  uint8_t pdata[128];
  uint8_t size;
}I2CRevSt;

typedef struct _USARTRevSt_
{
  bool RevF;
  uint8_t pdata[128];
  uint8_t size;
}USARTRevSt;

typedef struct _SPIRevSt_
{
  bool RevF;
  uint8_t pdata[128];
  uint8_t size;
}SPIRevSt;

typedef struct _CANRevSt_
{
  bool RevF;
  uint8_t pdata[128];
  uint8_t size;
}CANRevSt;

typedef struct _USBCDCRevSt_
{
  bool RevF;
  uint8_t pdata[128];
  uint8_t size;
}USBCDCRevSt;
   
extern I2CRevSt    _I2CRevSt;
extern USARTRevSt  _RS232RevSt;
extern USARTRevSt  _RS485RevSt;
extern SPIRevSt    _SPIRevSt;
extern CANRevSt    _ICANRevSt;
extern USBCDCRevSt _USBCDCRevSt;


void TestAPI_ThreadInit(void);
void Transmit_AskTestMode(void);
bool CheckRS232Receive(void);
bool CheckCDCUSBReceive(void);
bool CheckSPIReceive(void);
bool CheckCANBUSReceive(void);
bool CheckI2CReceive(void);
#if defined(STM32H750xx)
bool CheckEthnernetReceive(void);
#endif
#ifdef __cplusplus
}
#endif
#endif
#endif /*__EDT_F7XXH7XX_TESTAPI_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT EDT *****END OF FILE****/
