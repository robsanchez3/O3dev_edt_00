/**
  ******************************************************************************  
  * File Name          : edt_bsp_can.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides a set of firmware functions to CAN 
  *                      communicate with  external devices available on smart 
  *                      embedded display board. 
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT(c) 2023 Emerging Display Technologies Corp.
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
#include "edt_bsp_can.h"
#include <string.h>
#include <stdbool.h>

static uint8_t RxData[64];
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
CAN_HandleData fdcanSt;

FDCAN_HandleTypeDef hfdcanx;

static uint32_t GetCanBufDLC(uint8_t length);
static uint32_t GetRxCanBufDLC(uint32_t lengthidx);
/*****************************************************************************
  * @brief  Can bus 2 Initial
  * @param  
  * @retval None
******************************************************************************/
void EDT_CANx_Init(FDCAN_HandleTypeDef *hfdcan)
{
 /*                Bit time configuration:
    Bit time parameter         | Nominal      |  Data       |  Nominal       |  Data
    ---------------------------|--------------|-------------|----------------|------------
    fdcan_ker_ck               | 20 MHz       | 20 MHz      | 20 MHz         | 20 MHz
    Time_quantum (tq)          | 50 ns        | 50 ns       | 50 ns          | 50 ns
    Synchronization_segment    | 1 tq         | 1 tq        | 1 tq           | 1 tq
    Propagation_segment        | 23 tq        | 11 tq       | 23 tq          | 11 tq
    Phase_segment_1            | 8 tq         | 4 tq        | 8 tq           | 4 tq
    Phase_segment_2            | 8 tq         | 4 tq        | 8 tq           | 4 tq
    Synchronization_Jump_width | 8 tq         | 4 tq        | 8 tq           | 4 tq
    Bit_length                 | 40 tq *4     | 20 tq       | 40 tq          | 20 tq   
    Bit_rate                   | 125 KBit/s   | 1 MBit/s    | 500 KBit/s     | 1M KBit/s
  */  
  EDT_CANx_DeInit(hfdcan);
    
  hfdcan->Instance                  = FDCANx_INSTANCE;
 #if defined(USEFDCAN)
  hfdcan->Init.FrameFormat          = FDCAN_FRAME_FD_BRS; 
 #else
  hfdcan->Init.FrameFormat          = FDCAN_FRAME_CLASSIC;  
 #endif
  hfdcan->Init.Mode                 = FDCAN_MODE_NORMAL; 
  hfdcan->Init.AutoRetransmission   = DISABLE; //close auto re transmission
  hfdcan->Init.TransmitPause        = DISABLE;
  hfdcan->Init.ProtocolException    = ENABLE;  
  
#if defined(CAN_BAUDRATE125K)
  hfdcan->Init.NominalPrescaler     = 4;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
  hfdcan->Init.NominalSyncJumpWidth = 8; 
  hfdcan->Init.NominalTimeSeg1      = 31 ;  /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan->Init.NominalTimeSeg2      = 8;  
#elif defined(CAN_BAUDRATE250K)
  hfdcan->Init.NominalPrescaler     = 2;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
  hfdcan->Init.NominalSyncJumpWidth = 8; 
  hfdcan->Init.NominalTimeSeg1      = 31 ;  /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan->Init.NominalTimeSeg2      = 8;  
#elif defined(CAN_BAUDRATE500K)
  hfdcan->Init.NominalPrescaler     = 1;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 50*/
  hfdcan->Init.NominalSyncJumpWidth = 8; 
  hfdcan->Init.NominalTimeSeg1      = 31;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan->Init.NominalTimeSeg2      = 8;  
#elif defined(CAN_BAUDRATE1M)
  hfdcan->Init.NominalPrescaler     = 1;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 50 */
  hfdcan->Init.NominalSyncJumpWidth = 4; 
  hfdcan->Init.NominalTimeSeg1      = 15;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan->Init.NominalTimeSeg2      = 4;  
#else
  hfdcan->Init.NominalPrescaler     = 4;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
  hfdcan->Init.NominalSyncJumpWidth = 8; 
  hfdcan->Init.NominalTimeSeg1      = 31;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan->Init.NominalTimeSeg2      = 8;  
#endif
  
//  hfdcan->Init.MessageRAMOffset     = 0;
  hfdcan->Init.StdFiltersNbr        = 1;
  hfdcan->Init.ExtFiltersNbr        = 0;
//  hfdcan->Init.RxFifo0ElmtsNbr      = 2;
//  hfdcan->Init.RxFifo0ElmtSize      = FDCAN_DATA_BYTES_64;
//  hfdcan->Init.RxFifo1ElmtsNbr      = 0;
//  hfdcan->Init.RxBuffersNbr         = 64;
//  hfdcan->Init.TxEventsNbr          = 0;
//  hfdcan->Init.TxBuffersNbr         = 64;
//  hfdcan->Init.TxFifoQueueElmtsNbr  = 2;
  hfdcan->Init.TxFifoQueueMode      = FDCAN_TX_FIFO_OPERATION;
//  hfdcan->Init.TxElmtSize           = FDCAN_DATA_BYTES_64;
  
  HAL_FDCAN_MspInit(hfdcan);
  HAL_FDCAN_Init(hfdcan);
 
  EDT_CANx_Config(hfdcan);
}
void EDT_FDCANx_Init( FDCAN_HandleTypeDef *hfdcan, bool FD_mode, FDCanBaud baud )
{
  EDT_CANx_DeInit ( hfdcan );
    
  hfdcan->Instance                  = FDCANx_INSTANCE;
  if ( !FD_mode ) {
    hfdcan->Init.FrameFormat          = FDCAN_FRAME_CLASSIC;
  } else {
    hfdcan->Init.FrameFormat          = FDCAN_FRAME_FD_BRS;
  }

  hfdcan->Init.Mode                 = FDCAN_MODE_NORMAL; 
  hfdcan->Init.AutoRetransmission   = DISABLE; //close auto re transmission
  hfdcan->Init.TransmitPause        = DISABLE;
  hfdcan->Init.ProtocolException    = ENABLE;  
  
  EDT_FDCANx_SettingBaudRate( hfdcan, baud );
  
//  hfdcan->Init.MessageRAMOffset     = 0;
  hfdcan->Init.StdFiltersNbr        = 1;
  hfdcan->Init.ExtFiltersNbr        = 0;
//  hfdcan->Init.RxFifo0ElmtsNbr      = 2;
//  hfdcan->Init.RxFifo0ElmtSize      = FDCAN_DATA_BYTES_64;
//  hfdcan->Init.RxFifo1ElmtsNbr      = 0;
//  hfdcan->Init.RxBuffersNbr         = 64;
//  hfdcan->Init.TxEventsNbr          = 0;
//  hfdcan->Init.TxBuffersNbr         = 64;
//  hfdcan->Init.TxFifoQueueElmtsNbr  = 2;
  hfdcan->Init.TxFifoQueueMode      = FDCAN_TX_FIFO_OPERATION;
//  hfdcan->Init.TxElmtSize           = FDCAN_DATA_BYTES_64;
  
  HAL_FDCAN_MspInit(hfdcan);
  HAL_FDCAN_Init(hfdcan);
 
  EDT_CANx_Config(hfdcan);
}
void EDT_CANx_Config(FDCAN_HandleTypeDef *hfdcan)
{
  FDCAN_FilterTypeDef sFilterConfig;
  /* Configure Rx filter */
  sFilterConfig.IdType =  FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = 0x03 & (uint32_t) CANBUS_RX_ID_MODE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = FDCANx_ID1;
  sFilterConfig.FilterID2 = FDCANx_ID2; /* For acceptance, MessageID and FilterID1 must match exactly */
  
  HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig);

  /* Configure global filter to reject all non-matching frames */
  HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* Configure Rx FIFO 0 watermark to 2 */
//  HAL_FDCAN_ConfigFifoWatermark(hfdcan, FDCAN_CFG_RX_FIFO0, 2);
  HAL_FDCAN_ConfigExtendedIdMask(hfdcan, 2); // channing

  /* Activate Rx FIFO 0 watermark notification */
//  HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_WATERMARK, 0);
  HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);


  TxHeader.Identifier              = 0;                   //32bit ID
  TxHeader.IdType                  = FDCAN_STANDARD_ID;    
  TxHeader.TxFrameType             = FDCAN_DATA_FRAME;
#if defined(USEFDCAN)
  TxHeader.DataLength              = FDCAN_DLC_BYTES_64;
#else
  TxHeader.DataLength              = FDCAN_DLC_BYTES_8;
#endif
  TxHeader.ErrorStateIndicator     = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch           = FDCAN_BRS_ON;
#if defined(USEFDCAN)
  TxHeader.FDFormat                = FDCAN_FD_CAN;
#else
  TxHeader.FDFormat                = FDCAN_CLASSIC_CAN;
#endif

  TxHeader.TxEventFifoControl      = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker           = 0;     

  fdcanSt.CAN_HandleDataEn=false;
  fdcanSt.CAN_HandleDataSize=0;
  memset(fdcanSt.msg,0xff,sizeof(fdcanSt.msg));
     
  HAL_FDCAN_Start(hfdcan);

}
/*****************************************************************************
  * @brief  Can bus 2 DeInitial
  * @param  
  * @retval None
******************************************************************************/
void EDT_CANx_DeInit( FDCAN_HandleTypeDef * hfdcan )
{
   if ( HAL_FDCAN_Init(hfdcan) != HAL_OK ) { }
}
/*****************************************************************************
  * @brief  Can bus setting BaudRate
  * @param  baud: FDCan_BaudRate1M , FDCan_BaudRate500K ,FDCan_BaudRate125K
  * @retval None
******************************************************************************/
void  EDT_FDCANx_SettingBaudRate( FDCAN_HandleTypeDef * hfdcan, FDCanBaud baud )
{  
  switch(baud) {
    case FDCan_BaudRate1M:
       hfdcan->Init.NominalPrescaler     = 1;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 50 */
       hfdcan->Init.NominalSyncJumpWidth = 4; 
       hfdcan->Init.NominalTimeSeg1      = 15;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
       hfdcan->Init.NominalTimeSeg2      = 4;  
     break;
      
    case FDCan_BaudRate500K:
       hfdcan->Init.NominalPrescaler     = 1;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 50*/
       hfdcan->Init.NominalSyncJumpWidth = 8; 
       hfdcan->Init.NominalTimeSeg1      = 31;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
       hfdcan->Init.NominalTimeSeg2      = 8;  
      break;
      
    case FDCan_BaudRate250K:
       hfdcan->Init.NominalPrescaler     = 2;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
       hfdcan->Init.NominalSyncJumpWidth = 8; 
       hfdcan->Init.NominalTimeSeg1      = 31 ;  /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
       hfdcan->Init.NominalTimeSeg2      = 8;  
      break;
      
    case FDCan_BaudRate125K:
       hfdcan->Init.NominalPrescaler     = 4;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
       hfdcan->Init.NominalSyncJumpWidth = 8; 
       hfdcan->Init.NominalTimeSeg1      = 31 ;  /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
       hfdcan->Init.NominalTimeSeg2      = 8;  
      break;
      
    default:
       hfdcan->Init.NominalPrescaler     = 4;    /* tq = NominalPrescaler x (1/fdcan_ker_ck) = 200 */
       hfdcan->Init.NominalSyncJumpWidth = 8; 
       hfdcan->Init.NominalTimeSeg1      = 31;   /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
       hfdcan->Init.NominalTimeSeg2      = 8;  
      break;
  }
}
/*****************************************************************************
  * @brief  Can bus EDT_FDCANx_SettingIDFilterMode
  * @param  filterMode: FDCan_Fileter_Range , FDCan_Fileter_Dual ,FDCan_Filter_Mask,FDCan_Filter_Range_No_Eidm
  * @param  ID1: filter id1
  * @param  ID2: filter id2
  * @retval None
******************************************************************************/
void  EDT_FDCANx_SettingIDFilterMode(FDCAN_HandleTypeDef *hfdcan, \
    uint8_t filterMode, uint16_t ID1, uint16_t ID2 )
{ 
  FDCAN_FilterTypeDef sFilterConfig;
  sFilterConfig.IdType =  FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  
  switch(filterMode)
  {
    case FDCan_Fileter_Range:
     sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
    break;
    
    case FDCan_Fileter_Dual:
      sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
    break;
    
    case FDCan_Filter_Mask:
      sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    break;
    
    case FDCan_Filter_Range_No_Eidm:
      sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
    break;
    
  default:
      sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
    break;
  }
  
  sFilterConfig.FilterID1 = ID1;
  sFilterConfig.FilterID2 = ID2; /* For acceptance, MessageID and FilterID1 must match exactly */
  
  HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig);
}
/*****************************************************************************
  * @brief  EDT_FDCANx_Send_Msg
  * @param  hfdcan 
            ID       to external fdcan node
            msg      data message buf
            length   data length
  * @retval None
******************************************************************************/
uint8_t  EDT_FDCANx_Send_Msg( FDCAN_HandleTypeDef *hfdcan, uint32_t ID, \
          uint8_t* msg, uint32_t length )
{ 
  uint8_t txBuf[64],i;  

#if defined(USEFDCAN)
  if ( length > 16 ) 
#else
  if ( length > 8 ) 
#endif
  { return 1; }
  
  memset(txBuf , 0 , sizeof(txBuf));

  for(i=0;i<length;i++)
  txBuf[i]=msg[i];  

  TxHeader.DataLength   = GetCanBufDLC(length);

  TxHeader.Identifier              = ID;                   //32bit ID
  TxHeader.IdType                  = FDCAN_STANDARD_ID;    
  TxHeader.TxFrameType             = FDCAN_DATA_FRAME;
  TxHeader.ErrorStateIndicator     = FDCAN_ESI_ACTIVE;  
  TxHeader.BitRateSwitch           = FDCAN_BRS_OFF; 

#if defined(USEFDCAN)
  TxHeader.FDFormat                = FDCAN_FD_CAN; 
#else
  TxHeader.FDFormat                = FDCAN_CLASSIC_CAN;
#endif
  TxHeader.TxEventFifoControl      = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker           = 0;

  HAL_FDCAN_Start(hfdcan);

  HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, &txBuf[0]);  
     
   return 0;    
}
/*****************************************************************************
  * @brief  Can bus 2 Initial
  * @param  
  * @retval None
******************************************************************************/
uint8_t EDT_FDCANx_Receive_Msg(FDCAN_HandleTypeDef *hfdcan , uint8_t *msg, \
        uint8_t length)
{ 
  if((fdcanSt.CAN_HandleDataEn)&&(length<=64))   
  { 
    fdcanSt.CAN_HandleDataSize  = GetRxCanBufDLC(RxHeader.DataLength);  
    memcpy(msg , fdcanSt.msg , fdcanSt.CAN_HandleDataSize);
    fdcanSt.CAN_HandleDataEn=false;
    //fdcanSt.CAN_HandleDataSize=0;
    memset(fdcanSt.msg,0xff,sizeof(fdcanSt.msg));
    return fdcanSt.CAN_HandleDataSize;
  }
  else
  return 0;
}
 /*****************************************************************************
  * @brief  Can bus 2 Initial
  * @param  
  * @retval None
******************************************************************************/
int EDT_CAN_Receive(FDCAN_HandleTypeDef *hfdcan ,  uint8_t *buf , uint8_t length )
{
  uint8_t i;
    if(length>16)  return -1;
  else
  {
    for(i=0;i<length;i++)
    buf[i]=RxData[i];
    return 0;
  }
} 
/*************************************************************************
  * @brief  FDCAN Receive Fifo Callback
  * @param  None
  * @retval 
  ************************************************************************/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{  
//  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != RESET)
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
  /* Retreive Rx messages from RX FIFO0 */

    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData);

    fdcanSt.IDentify = RxHeader.Identifier;
    fdcanSt.CAN_HandleDataEn    = true;
    fdcanSt.CAN_HandleDataSize  = GetRxCanBufDLC(RxHeader.DataLength);
    memset(fdcanSt.msg,0xff,sizeof(fdcanSt.msg));
    memcpy(fdcanSt.msg,RxData,fdcanSt.CAN_HandleDataSize);

//    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_WATERMARK, 0);
    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  }
}
/*************************************************************************
  * @brief  FFDCAN_CAL_IRQHandler
  * @param  None
  * @retval 
  ************************************************************************/
void FDCAN_CAL_IRQHandler(void)
{
  
}

static uint32_t GetCanBufDLC(uint8_t length)
{
	uint32_t DLC;
  switch(length)
  {
    case 0:
      DLC = FDCAN_DLC_BYTES_0; //0
    break;

    case 1:
      DLC = FDCAN_DLC_BYTES_1; //1
    break;

    case 2:
      DLC = FDCAN_DLC_BYTES_2; //2
    break;

    case 3:
      DLC = FDCAN_DLC_BYTES_3; //3
    break;

    case 4:
      DLC = FDCAN_DLC_BYTES_4; //4
    break;

    case 5:
      DLC = FDCAN_DLC_BYTES_5; //5
    break;

    case 6:
      DLC = FDCAN_DLC_BYTES_6; //6
    break;

    case 7:
      DLC = FDCAN_DLC_BYTES_7; //7
    break;
        
    case 8:
      DLC = FDCAN_DLC_BYTES_8; //8
    break;

    case 12:
      DLC = FDCAN_DLC_BYTES_12; //12
    break;

    case 16:
      DLC = FDCAN_DLC_BYTES_16; //16
    break;

    case 20:
      DLC = FDCAN_DLC_BYTES_20; //2
    break;

    case 32:
      DLC = FDCAN_DLC_BYTES_32; //2
    break;

    case 48:
      DLC = FDCAN_DLC_BYTES_48; //2
    break;

    case 64:
      DLC = FDCAN_DLC_BYTES_64; //2
    break;       

    default:
      if((length>8)&&(length<12))
        DLC = FDCAN_DLC_BYTES_12; //32
      else if((length>12)&&(length<16))
        DLC = FDCAN_DLC_BYTES_16; //32
      else if((length>16)&&(length<20))
        DLC = FDCAN_DLC_BYTES_20; //32
      else if((length>20)&&(length<32))
        DLC = FDCAN_DLC_BYTES_32; //32
      else if((length>32)&&(length<48))
        DLC = FDCAN_DLC_BYTES_48; //48
      else if((length>48)&&(length<64))
        DLC = FDCAN_DLC_BYTES_64; //64
      else
        DLC = FDCAN_DLC_BYTES_0; //0
    break;
  }
	return DLC ;
}
static uint32_t GetRxCanBufDLC(uint32_t lengthidx)
{
	uint32_t DLC;
  switch(lengthidx)
  {
    case FDCAN_DLC_BYTES_0:
      DLC = 0; //0
    break;

    case FDCAN_DLC_BYTES_1:
      DLC = 1; //1
    break;

    case FDCAN_DLC_BYTES_2:
      DLC = 2; //2
    break;

    case FDCAN_DLC_BYTES_3:
      DLC = 3; //3
    break;

    case FDCAN_DLC_BYTES_4:
      DLC = 4; //4
    break;

    case FDCAN_DLC_BYTES_5:
      DLC = 5; //5
    break;

    case FDCAN_DLC_BYTES_6:
      DLC = 6; //6
    break;

    case FDCAN_DLC_BYTES_7:
      DLC = 7; //7
    break;

    case FDCAN_DLC_BYTES_8:
      DLC = 8; //8
    break;

    case FDCAN_DLC_BYTES_12:
      DLC = 12; //12
    break;

    case FDCAN_DLC_BYTES_16:
      DLC = 16; //16
    break;

    case FDCAN_DLC_BYTES_20:
      DLC = 20; //2
    break;

    case FDCAN_DLC_BYTES_32:
      DLC = 32; //2
    break;

    case FDCAN_DLC_BYTES_48:
      DLC = 48; //2
    break;

    case FDCAN_DLC_BYTES_64:
      DLC = 64; //2
    break;       

    default:
      DLC = 0;
    break;
  }
     return DLC;  
}

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/

