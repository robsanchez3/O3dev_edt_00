/**
  ******************************************************************************  
  * File Name          : edt_bsp_can.h
  * @author            : EDT Embedded Application Team
  * Description        : This file contains the common defines and functions 
  *                      prototypes for the edt_bsp_can.c driver.
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
#ifndef __EDT_BSP_CAN_H
#define __EDT_BSP_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include "main.h"  
#include <stdbool.h>
   
typedef struct{
 bool CAN_HandleDataEn;               //CANBUS Receive IRQ Callback Enable
 uint32_t IDentify;                   //CANBUS identifier type
 uint32_t CAN_HandleDataSize;         //CANBUS Receive IRQ Data length
 uint8_t msg[16];
}CAN_HandleData;

typedef enum {
  FDCan_BaudRate1M,
  FDCan_BaudRate500K,
  FDCan_BaudRate250K,
  FDCan_BaudRate125K,
}FDCanBaud ;

typedef enum { 
  FDCan_Fileter_Range,               /*!< Range filter from FilterID1 to FilterID2                        */
  FDCan_Fileter_Dual,                /*!<  Dual ID filter for FilterID1 or FilterID2                      */
  FDCan_Filter_Mask,                 /*!< Dual ID filter for FilterID1 or FilterID2                       */
  FDCan_Filter_Range_No_Eidm,        /*!< Classic filter: FilterID1 = filter, FilterID2 = mask            */  
}FDCanFilterMode;

extern FDCAN_HandleTypeDef hfdcanx;
extern CAN_HandleData fdcanSt;

void    EDT_CANx_Init(FDCAN_HandleTypeDef *hfdcan);
void    EDT_FDCANx_Init(FDCAN_HandleTypeDef *hfdcan, bool FD_mode, FDCanBaud baud);
void    EDT_CANx_DeInit(FDCAN_HandleTypeDef *hfdcan);
void    EDT_FDCANx_SettingBaudRate(FDCAN_HandleTypeDef * hfdcan, FDCanBaud baud);
void    EDT_FDCANx_SettingIDFilterMode(FDCAN_HandleTypeDef *hfdcan, uint8_t filterMode, uint16_t ID1  , uint16_t ID2 );
uint8_t EDT_FDCANx_Send_Msg(FDCAN_HandleTypeDef *hfdcan , uint32_t ID , uint8_t* msg , uint32_t length);
uint8_t EDT_FDCANx_Receive_Msg(FDCAN_HandleTypeDef *hfdcan , uint8_t *msg,  uint8_t length);
void EDT_CANx_Config(FDCAN_HandleTypeDef *hfdcan);
#ifdef __cplusplus
}
#endif

#endif /* __EDT_BSP_CAN_H */

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/


