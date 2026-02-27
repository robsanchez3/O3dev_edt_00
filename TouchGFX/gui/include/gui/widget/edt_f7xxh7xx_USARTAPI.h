/**
  ******************************************************************************  
  * File Name          : edt_f7xxh7xx_USARTAPI.h
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the TestAPI instances.  
  * @version           : V1.0.0
  * @date              : 29-JULY-2019
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 EDT</center></h2>
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
#ifndef __EDT_F7XXH7XX_USARTAPI_H
#define __EDT_F7XXH7XX_USARTAPI_H

#ifndef SIMULATOR

#ifdef __cplusplus
 extern "C" {
#endif
   
#include "stdbool.h"    
#if defined(STM32H750xx)
 #include "stm32h7xx_hal.h"
#else
 #include "stm32f7xx_hal.h"
#endif
   
typedef struct{
 bool  RxEn;                //USART Queue Rx Enabled
 uint8_t Data[64];         //USART Queue Rx Data
 uint32_t DataSize;         //USART Queue RX Data Size 
}USART_HandleData;

typedef struct
{
  uint8_t head;     //HEAD    0x5E       
  uint8_t len;      //format  id -> sum
  uint8_t nodeid;   //NodeID  0xAA
  uint8_t command;  //Cmd    
  uint8_t data[64] ;   //data len - 2
  uint8_t _xor;     //Length ^ ID ^ Cmd ^(pData[0]^..pData[n]) ^ 0xff
  uint8_t _sum;     //(Length + ID + Cmd + (pData[0]+..pData[n])+Xor) & 0xff
}USART_FORMAT;

#define SMT_CMD 0xAA   
#define TCM_CMD 0x55   
#define TPM_CMD 0xf0   
#define RA_CMD  0x0f   
#define QC_CMD  0xf0   

typedef struct                              
{
        bool     ModeRun;                                
        uint8_t  Mode;                                
	uint8_t  Btn ;
        uint16_t RACnt;
}TesterFun;
extern TesterFun TesterSt;

extern UART_HandleTypeDef *pRs485;
extern UART_HandleTypeDef *pRs232;

#define USART_REC_LEN 255

#define BOOT_HEAD 0x5E
#define BOOT_ID   0x01

#define FUNC_CNT 3

 #define CHOOSE_MODE           0xA0                  // 55H  Entry Boot Mode No Response ( APP USE )
 #define CHOOSE_MODE_LEN       6                       
                

#define  READ_RACOUNT_CMD           0xA1                  // 55H  Entry Boot Mode No Response ( APP USE )
 #define READ_RACOUNT_CMD_LEN       6                       
                
 USART_FORMAT Read_RACnt_Fn(uint8_t *pdata);   
 USART_FORMAT Entry_Mode_Fn(uint8_t *pdata);     

/**************************RS4853q¢XT£g2oc*******************/
typedef USART_FORMAT (*UsartApi)(uint8_t*);      

typedef struct                              
{
        uint8_t cmd;                                
	uint16_t Rev_len;                           
	UsartApi BootFun;                       
}USARTCMD;

static USARTCMD usart_cmd[FUNC_CNT] =            
    {     
		 {CHOOSE_MODE             , CHOOSE_MODE_LEN              , Entry_Mode_Fn              , },  
                 {READ_RACOUNT_CMD             , READ_RACOUNT_CMD_LEN              , Read_RACnt_Fn                   , },  
    };

uint8_t UsartXOR(USART_FORMAT data);
uint8_t UsartSUM(USART_FORMAT data);

void USARTAPI_Init(UART_HandleTypeDef *usart);
void CreateUsart_Task_Init(void);
void USART_ReceiveData(UART_HandleTypeDef *huart , uint8_t *pdata , uint32_t len);
void SendUartData(UART_HandleTypeDef *huart, USART_FORMAT data)  ;


#ifdef __cplusplus
}
#endif

#endif

#endif /*__EDT_F7XXH7XX_USARTAPI_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT EDT *****END OF FILE****/
