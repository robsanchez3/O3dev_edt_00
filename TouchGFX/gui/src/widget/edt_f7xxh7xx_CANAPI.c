/**
  ******************************************************************************  
  * File Name          : edt_f7xxh7xx_CANAPI.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the CAN instances to stm32f746/stm32f750/stm32f767.   
  * @version           : V1.1.0
  * @date              : 19-Jan-2021
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 EDT</center></h2>
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
#ifndef SIMULATOR
  #include "edt_f7xx_can.h"


uint8_t CRxData[8];

extern CAN_RxHeaderTypeDef   RxHeader;
extern bool CANHOST;

/*****************************************************************************
  *  * @brief CAN MX USER USED Can Bus EDT_CAN_RxFifo0MsgPendingCallback
  *        This function frees the hardware resources used in this example:
  * @param hcan    : CAN handle pointer
  * @retval HAL_StatusTypeDef
******************************************************************************/
void EDT_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CRxData) != HAL_OK)
  {

  }
  /* Process Struct from CAN Bus */
if (CANHOST)
{
  if ((RxHeader.StdId == CANBUS_HOSTID) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC > 0))
  {
    CanHandleSt.CAN_HandleDataEn=true;
    CanHandleSt.CAN_HandleDataSize = RxHeader.DLC;
  }
}
else
  if ((RxHeader.StdId == CANBUS_REMOTEID) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC > 0))
  {
    CanHandleSt.CAN_HandleDataEn=true;
    CanHandleSt.CAN_HandleDataSize = RxHeader.DLC;
  }
}
#endif