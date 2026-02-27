/**
  ******************************************************************************  
  * File Name          : edt_f7xxh7xx_USARTAPI.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the edt_f7xxh7xx_USARTAPI instances.  
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

#ifndef SIMULATOR

/* Includes ------------------------------------------------------------------*/


#include <gui/widget/edt_f7xxh7xx_USARTAPI.h>    
#include <gui/widget/edt_f7xxh7xx_TESTAPI.h>
    
  #include "main.h"
  #include "cmsis_os.h"
  #include "FreeRTOS.h"


#include <stdio.h>
#include <string.h>

#include "task.h"
#include "queue.h"

UART_HandleTypeDef *pRs485;
UART_HandleTypeDef *pRs232;

#define rs485Rx_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 5 )
#define rs485Rx_TASK_STK_SIZE                 ( 512 )   
static USART_HandleData RS485_RxQueue;
void rs485_Process_task(void *pvParameters);	//Process block Rx Data
void rs485Rx_task(void *pvParameters);	        //Receive block Rx Data

QueueHandle_t rs485TxQueue = NULL;
QueueHandle_t rs485RxQueue = NULL;
TaskHandle_t rs485_Rx_handle;

#define rs232Rx_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 6 )
#define rs232Rx_TASK_STK_SIZE                 ( 512 )   
static USART_HandleData RS232_RxQueue;
void rs232_Process_task(void *pvParameters);	//Process block Rx Data
void rs232Rx_task(void *pvParameters);	        //Receive block Rx Data

QueueHandle_t rs232TxQueue = NULL;
QueueHandle_t rs232RxQueue = NULL;
TaskHandle_t rs232_Rx_handle;

TesterFun TesterSt;

/*****************************************************************************
 * @brief  USARTAPI_Init for RS485 /RS232
 * @param  usart :uart interface handler
 * @retval None
 ******************************************************************************/
void USARTAPI_Init(UART_HandleTypeDef *usart)
{ 
  if( usart->Instance == USART_RS485_INSTANCE)
  {
    pRs485 = usart;
  }
  else if( usart->Instance == USART_RS232_INSTANCE)
  {
    pRs232 = usart;
  }    
}
/*****************************************************************************
 * @brief  CreateUsart_Task_Init (Create queue Task for rs232/rs485)
 * @param
 * @retval None
 ******************************************************************************/
void CreateUsart_Task_Init(void)
{
    uint32_t i;

    rs485TxQueue = xQueueCreate(64, sizeof(uint8_t));
    rs485RxQueue = xQueueCreate(64, sizeof(uint8_t));
    
    rs232TxQueue = xQueueCreate(64, sizeof(uint8_t));
    rs232RxQueue = xQueueCreate(64, sizeof(uint8_t));

    RS485_RxQueue.RxEn = 0;
    RS485_RxQueue.DataSize = 0;
    for (i = 0; i < sizeof(RS485_RxQueue.Data); i++)
      RS485_RxQueue.Data[i] = 0;
    
    RS232_RxQueue.RxEn = 0;
    RS232_RxQueue.DataSize = 0;
    for (i = 0; i < sizeof(RS232_RxQueue.Data); i++)
      RS232_RxQueue.Data[i] = 0;

    xTaskCreate(rs485Rx_task, "rs485_rxtask", rs485Rx_TASK_STK_SIZE, NULL, rs485Rx_TASK_PRIORITY, &rs485_Rx_handle);
    xTaskCreate(rs232Rx_task, "rs232_rxtask", rs232Rx_TASK_STK_SIZE, NULL, rs232Rx_TASK_PRIORITY, &rs232_Rx_handle);
}
/******************************************************************************
 * @brief  RS232 Receive Task Process to Queue to Data Struct
 * @param  None
 * @retval None
 ******************************************************************************/
void rs232Rx_task(void *pvParameters)
{    
    while (1)
    {
      if (rs232RxQueue != NULL)
      {
        if (xQueueReceive(rs232RxQueue,&RS232_RxQueue.Data[RS232_RxQueue.DataSize],10/portTICK_RATE_MS) == pdPASS)  //block queue
        {
          RS232_RxQueue.DataSize++;
          RS232_RxQueue.RxEn = 1;
        }
        else  //exit block queue
        {
          if (RS232_RxQueue.DataSize > 0)   //process Queue usart Recevie to Sturct RS232_RxData
          {
            _RS232RevSt.RevF=true;
            _RS232RevSt.size=RS232_RxQueue.DataSize;
            memcpy(_RS232RevSt.pdata,RS232_RxQueue.Data,_RS232RevSt.size);     
            
            RS232_RxQueue.DataSize=0;
            RS232_RxQueue.RxEn=0;
            memset(RS232_RxQueue.Data,0,sizeof(RS232_RxQueue.Data));
          }    
         }
        }       
//       vTaskDelay(20);
      }
     
}
/******************************************************************************
 * @brief  RS232 Byte Recieve Interrupt Handler
 * @param  None
 * @retval None
 ******************************************************************************/
void EDT_RS232_IRQHandler(void)
{
  long xHigherPriorityTaskWoken = pdFALSE;
    uint8_t rbyte, tbyte;

     if(pRs232 != NULL)
  {
      if (pRs232->Instance == USART_RS232_INSTANCE)
      {
        if ((__HAL_UART_GET_IT(&hRs232, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&hRs232, UART_IT_RXNE) != RESET))
        {
          __HAL_UART_SEND_REQ(&hRs232, UART_RXDATA_FLUSH_REQUEST);
          rbyte = (uint8_t)(hRs232.Instance->RDR);

          if (rs232RxQueue != NULL)
          {
            xQueueSendFromISR(rs232RxQueue, &rbyte, &xHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
            rbyte = 0;
          }
        }
       if ((__HAL_UART_GET_IT(&hRs232, UART_IT_TXE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&hRs232, UART_IT_TXE) != RESET))
        {
          __HAL_UART_SEND_REQ(&hRs232, UART_TXDATA_FLUSH_REQUEST);

          if (xQueueReceiveFromISR(rs232TxQueue, &tbyte, &xHigherPriorityTaskWoken))
          {
            (hRs232.Instance->TDR) = (uint8_t) tbyte;
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
          }
          else
          {
            __HAL_UART_DISABLE_IT(&hRs232, UART_IT_TXE);
          }
        }
          if((__HAL_UART_GET_IT(&hRs232, UART_IT_TC) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&hRs232, UART_IT_TC) != RESET))
    {
        __HAL_UART_SEND_REQ(&hRs232, UART_TXDATA_FLUSH_REQUEST);       
        __HAL_UART_DISABLE_IT(&hRs232, UART_IT_TC);
    }
      HAL_UART_IRQHandler(&hRs232);
    
      }
  } 
}

/******************************************************************************
 * @brief  RS485 Receive Task Process to Queue to Data Struct
 * @param
 * @retval None
 ******************************************************************************/
void rs485Rx_task(void *pvParameters)
  {   
    uint32_t i;
    while (1)
    {
      if(pRs485!=NULL)
      {
      if (rs485RxQueue != NULL)
      {
        if (xQueueReceive(rs485RxQueue,&RS485_RxQueue.Data[ RS485_RxQueue.DataSize],10/portTICK_RATE_MS) == pdPASS)  //block queue
        {
          RS485_RxQueue.DataSize++;
          RS485_RxQueue.RxEn = 1;

        }
        else  //exit block queue
        {
          if (RS485_RxQueue.DataSize > 0)   //process Queue usart Recevie to Sturct RS232_RxData
          {
            USART_ReceiveData(&hRs485, RS485_RxQueue.Data, RS485_RxQueue.DataSize);
            RS485_RxQueue.DataSize = 0;
            RS485_RxQueue.RxEn = 0;
            for (i = 0; i < RS485_RxQueue.DataSize; i++)
              RS485_RxQueue.Data[i] = 0;
          }
        }
      }
      }
     //  vTaskDelay(20);
    }
  }
/******************************************************************************
 * @brief  RS485 Byte Recieve Interrupt Handler
 * @param  None
 * @retval None
 ******************************************************************************/
void EDT_RS485_IRQHandler(void)
{  
  long xHigherPriorityTaskWoken = pdFALSE;
    uint8_t rbyte, tbyte;

      if(pRs485 != NULL)
       {
      if (pRs485->Instance == USART_RS485_INSTANCE)
      {
        if ((__HAL_UART_GET_IT(&hRs485, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&hRs485, UART_IT_RXNE) != RESET))
        {
          __HAL_UART_SEND_REQ(pRs485, UART_RXDATA_FLUSH_REQUEST);
          rbyte = (uint8_t)(pRs485->Instance->RDR);

          if (rs485RxQueue != NULL)
          {
            xQueueSendFromISR(rs485RxQueue, &rbyte, &xHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
            rbyte = 0;
          }
        }
        
   //usart Tx Process
       if ((__HAL_UART_GET_IT(&hRs485, UART_IT_TXE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&hRs485, UART_IT_TXE) != RESET))
        {
          __HAL_UART_SEND_REQ(&hRs485, UART_TXDATA_FLUSH_REQUEST);

          if (xQueueReceiveFromISR(rs485TxQueue, &tbyte, &xHigherPriorityTaskWoken))
          {
            (hRs485.Instance->TDR) = (uint8_t) tbyte;
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
          }
          else
          {
            __HAL_UART_DISABLE_IT(&hRs485, UART_IT_TXE);
          }
        }   
           if((__HAL_UART_GET_IT(&hRs485, UART_IT_TC) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&hRs485, UART_IT_TC) != RESET))
    {
        __HAL_UART_SEND_REQ(&hRs485, UART_TXDATA_FLUSH_REQUEST);       
        __HAL_UART_DISABLE_IT(&hRs485, UART_IT_TC);
    }
      HAL_UART_IRQHandler(&hRs485);
      }
   }
}
/******************************************************************************
 * @brief  EDT_UART_Transmit_IT (__weak Function from EDT BSP UART Driver)
 * @param  huart : uart Handler rs232/rs485
 * @param  pdata : data buffer 
 * @param  Size  : data size
 * @retval None  : HAL_StatusTypeDef  None,ERROR
 ******************************************************************************/
HAL_StatusTypeDef EDT_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pdata, uint16_t Size)  
  {
    uint16_t ul;
    if (huart->Instance == USART_RS485_INSTANCE)
    {
      if (rs485TxQueue != NULL)
      {
        for (ul = 0; ul < Size; ul++)
        {
          xQueueSend(rs485TxQueue, &(pdata[ul]), 0);
        }
        __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
      }
    }
    else if (huart->Instance == USART_RS232_INSTANCE)
    {
      if (rs232TxQueue != NULL)
      {
        for (ul = 0; ul < Size; ul++)
        {
          xQueueSend(rs232TxQueue, &(pdata[ul]), 0);
        }
        __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
      }
    }
    return HAL_OK;
  }
/******************************************************************************
 * @brief  EDT_UART_Receive_IT(__weak Function from EDT BSP UART Driver)
 * @param  huart : uart Handler rs232/rs485
 * @param  pdata : data buffer 
 * @param  Size  : data size
 * @retval None  : HAL_StatusTypeDef  None,ERROR
 ******************************************************************************/
HAL_StatusTypeDef EDT_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
 {
    return HAL_UART_Receive_IT(huart, pData, Size);
 }
/******************************************************************************
 * @brief  UsartXOR    (Special Xor for TestAPI examle)
 * @param  USART_FORMAT: Uart Communication Struct
 * @retval Xor         : Xor
 ******************************************************************************/
uint8_t UsartXOR(USART_FORMAT data)
{
  uint8_t *pdata = (unsigned char *) &data;	
  uint8_t  i,DataLen,XorLen;
  uint8_t  dxor=0xff;	
	uint8_t  tmp[64];
	
	DataLen = pdata[1]-4; 	XorLen  = pdata[1]-2;
	
	for(i = 0 ;i < 2    ;i ++)      tmp[i]=pdata[i+2]; 
	for(i = 0 ;i < DataLen ; i ++)	  tmp[i+2]=data.data[i];	
	for(i = 0 ;i< XorLen; i++) dxor = dxor^tmp[i];
	return dxor;
}
/******************************************************************************
 * @brief  UsartSUM    (Special SUMr for TestAPI examle)
 * @param  USART_FORMAT: Uart Communication Struct
 * @retval Sum         : Sum
 ******************************************************************************/
uint8_t UsartSUM(USART_FORMAT data)
{
        uint8_t  i,DataLen,SumLen;
        uint8_t  dsum = 0;
	uint8_t *pdata = (unsigned char *) &data;	
	uint8_t tmp[64];
  
	DataLen = pdata[1]-4; 	SumLen  = pdata[1]-1;
	
	for(i = 0 ;i < 2    ;i ++)      tmp[i]=pdata[i+2]; 
	for(i = 0 ;i < DataLen ; i ++)	  tmp[i+2]=data.data[i];
	
	tmp[pdata[1]-2] = data._xor;
	for(i = 0 ;i< SumLen; i++) dsum = dsum+tmp[i];
	dsum&=0xff;  
	return dsum;
}
/******************************************************************************
 * @brief  USART_ReceiveData(Special SUMr for TestAPI examle)
 * @param  huart : uart Handler rs232/rs485
 * @param  pdata : data buffer 
 * @param  len   : data size
 * @retval None
 ******************************************************************************/
void USART_ReceiveData(UART_HandleTypeDef *huart , uint8_t *pdata , uint32_t len)
{
         uint8_t rbuf[64];
	 uint16_t i;		 
	 USART_FORMAT UserUartRx,UserUartTx;	 
	 
	 for(i=0;i<len;i++)	 rbuf[i]=pdata[i];
	 
	 UserUartRx.head    =   rbuf[0];                     
	 UserUartRx.len     =   rbuf[1];  
	 UserUartRx.nodeid  =   rbuf[2];
	 UserUartRx.command =   rbuf[3];
	 *UserUartRx.data    =   rbuf[4];
	 UserUartRx._xor    =   rbuf[rbuf[1] ];   // rbuf[rbuf[1]+4];  /4 36
	 UserUartRx._sum    =   rbuf[rbuf[1]+1]; // rbuf[rbuf[1]+5];
         
         
         for(i=0;i<FUNC_CNT;i++)
		{
			if((UserUartRx.command == usart_cmd[i].cmd)&&(UserUartRx.len == usart_cmd[i].Rev_len))
                        {   
                          
                          UserUartTx = usart_cmd[i].BootFun(&rbuf[4]);
                        
                          if(UserUartTx.head!=0)       
                          {                           
                            SendUartData(huart, UserUartTx);
                          }
                          
                          break;
                        }
                }	 
}
/******************************************************************************
 * @brief  SendUartData(Special SUMr for TestAPI examle)
 * @param  USART_FORMAT: Uart Communication Struct
 * @retval None
 ******************************************************************************/
void SendUartData(UART_HandleTypeDef *huart, USART_FORMAT data)  
{		
        uint32_t i; 
        uint8_t *pdata = (unsigned char *) &data;	
        
        uint8_t txbuf[64];
        uint8_t len; // 6
        uint8_t datalen;
				
        for(i=0;i<sizeof(txbuf);i++)
        txbuf[i]=0;
        for(i=0;i<4;i++)
        txbuf[i]=pdata[i];        
       
        len = txbuf[1]; //wait for tran
        datalen= len - 4;
        if(datalen>0)
        {
         for(i=0;i<datalen;i++)
         {
           txbuf[4+i]=data.data[i];
         }
        }       
        txbuf[len]=data._xor;
        txbuf[len+1]=data._sum;	      
	 
         SCB_CleanInvalidateDCache();        
        
        HAL_UART_Transmit(huart,txbuf,len+2,20); 
}
/*****************************************************************************
 * @brief  Entry_Mode_Fn(Special Communication TestAPI examle)
 * @param  pdata       : data buffer
 * @retval USART_FORMAT: Uart Communication Struct
******************************************************************************/
 USART_FORMAT Entry_Mode_Fn(uint8_t *pdata)
 {
    USART_FORMAT resopne_usart; 
    
    TesterSt.Mode = pdata[0];
    TesterSt.Btn  = pdata[1];
    TesterSt.ModeRun =true;
    
    resopne_usart.head=0x5e;
    resopne_usart.len =4;
    resopne_usart.command=0xa0;
    resopne_usart._xor =UsartXOR(resopne_usart);
    resopne_usart._sum =UsartSUM(resopne_usart);
   
   memset(&resopne_usart,0,sizeof(resopne_usart));   
   return resopne_usart;
 }
/*****************************************************************************
 * @brief  Read_RACnt_Fn(Special Communication TestAPI examle)
 * @param  pdata       : data buffer
 * @retval USART_FORMAT: Uart Communication Struct
******************************************************************************/
 USART_FORMAT Read_RACnt_Fn(uint8_t *pdata)
 {
   uint16_t racnt =0 ;
   USART_FORMAT resopne_usart;   
   
   racnt =(pdata[0]>>8) |pdata[1];   
   
   TesterSt.RACnt =racnt;
 
   memset(&resopne_usart,0,sizeof(resopne_usart));
   return resopne_usart;
 }
/************************ (C) COPYRIGHT EDT *****END OF FILE****/

#endif