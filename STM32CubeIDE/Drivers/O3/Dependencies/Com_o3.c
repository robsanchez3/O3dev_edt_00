/*
 * Com_o3.c
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */
#include <stdio.h>
#include <string.h>
#include <../Vendor/Driver/edt_bsp_uart.h>
#include "usart.h"
#include <Dependencies/Com_o3.h>
#include <gui/widget/edt_f7xxh7xx_TestAPI.h>




#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
//#include "stm32f4xx_hal.h"
#define UART_RX_DMA_BUFFER_SIZE   128
#define UART_STREAM_BUFFER_SIZE   256
static uint8_t uart_rx_dma_buffer[UART_RX_DMA_BUFFER_SIZE]; // buffer where HAL writes
static StreamBufferHandle_t uartRxStream; // circular buffer managed by FreeRTOS






uint8_t COM_O3_Ready = 0;
uint8_t tmpBuff[COM_O3_LOOP_BUFFER_LEN/2];
COM_O3_LOOP_BUFFER_T comBuff;


/*
    Usart 2 - main board CN2
	Usart 6 - fan out expansion board Tx(pin 21) Rx(pin 23)
	Usart 1 - fan out expansion board Tx(pin 06) Rx(pin 08) - 27 Series Connector Board CN5 Tx(pin 05) Rx(pin 04) control (pin 01) - Low power UART (LPUART)
	Usart 3 - fan out expansion board Tx(pin 22) Rx(pin 24) - 27 Series Connector Board CN5 Tx(pin 03) Rx(pin 02)
*/


//#define COM_O3_TX_UART_HANDLE (&huart2)
  #define COM_O3_TX_UART_HANDLE (&huart6)
//#define COM_O3_TX_UART_HANDLE (&huart1)
//#define COM_O3_TX_UART_HANDLE (&huart3)

static HAL_StatusTypeDef COM_O3_Arm_Rx(void);

void COM_O3_Init(void)
{
    uartRxStream = xStreamBufferCreate(UART_STREAM_BUFFER_SIZE, 1);   // trigger level means that the task is unlocked when it receives at least 1 byte.

	comBuff.head = 0;
	comBuff.tail = 0;
	memset(comBuff.data, 0, COM_O3_LOOP_BUFFER_LEN);
	memset(tmpBuff, 0, sizeof(tmpBuff));

	HAL_StatusTypeDef rxStart;
	rxStart = COM_O3_Arm_Rx();
	printf("COM_O3_Arm_RX status=%d\n", (int)rxStart);

	COM_O3_Ready = 1;
}

void COM_O3_Clear(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
}

void COM_O3_SendConst(const char *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen(Data));
}

void COM_O3_Send(uint8_t *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
}

void COM_O3_PutString(uint8_t *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
}

void COM_O3_PollRx(void)
{
	if(COM_O3_Ready)
	{
		if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_RXFNE))
		{
			comBuff.data[comBuff.head++] = (uint8_t)(huart6.Instance->RDR & 0xFFU);

			if(comBuff.head == COM_O3_LOOP_BUFFER_LEN)
			{
				comBuff.head = 0;
			}
		}

		if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_ORE)) __HAL_UART_CLEAR_OREFLAG(&huart6);
		if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_FE))  __HAL_UART_CLEAR_FEFLAG(&huart6);
		if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_NE))  __HAL_UART_CLEAR_NEFLAG(&huart6);
		if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_PE))  __HAL_UART_CLEAR_PEFLAG(&huart6);
	}
}

uint8_t COM_O3_GetChar(uint8_t *data)
{
	if(comBuff.head != comBuff.tail)
	{
		*data = comBuff.data[comBuff.tail++];

        if(comBuff.tail == COM_O3_LOOP_BUFFER_LEN)
		{
        	comBuff.tail = 0;
		}
        return 1;
	}
	return 0;
}

uint8_t COM_O3_DataAvailable(void)
{
	if(comBuff.head != comBuff.tail)
	{
        return 1;
	}
	return 0;
}

static HAL_StatusTypeDef COM_O3_Arm_Rx(void)
{
	__HAL_UART_CLEAR_FLAG(COM_O3_TX_UART_HANDLE, UART_CLEAR_IDLEF);
	__HAL_UART_CLEAR_OREFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_NEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_FEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_PEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_SEND_REQ(COM_O3_TX_UART_HANDLE, UART_RXDATA_FLUSH_REQUEST);

	return HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, tmpBuff, sizeof(tmpBuff));
//	return HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, uart_rx_dma_buffer, UART_RX_DMA_BUFFER_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

	if ( (huart == NULL) ||  ((huart->Instance != USART6) && (huart->Instance != USART1) && (huart->Instance != USART3)) )
	{
		return;
	}
	for(uint16_t i = 0; i < Size; i++)
	{
		comBuff.data[comBuff.head++] = tmpBuff[i];

		if (comBuff.head >= COM_O3_LOOP_BUFFER_LEN)
		{
			comBuff.head = 0;
		}
	}

//	printf("RX received = %u bytes -> %s\n", (unsigned int)Size, tmpBuff);
	memset(tmpBuff, 0, sizeof(tmpBuff));
	HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, tmpBuff, sizeof(tmpBuff));

#if 0

	printf("RX received = %u bytes\n", (unsigned int)Size);

	  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if ( (huart == NULL) ||  ((huart->Instance != USART6) && (huart->Instance != USART1) && (huart->Instance != USART3)) )
	{
		return;
	}
    size_t sent;

     sent = xStreamBufferSendFromISR(uartRxStream, uart_rx_dma_buffer, Size, &xHigherPriorityTaskWoken);

     /* control opcional de overflow */
     if(sent != Size)
     {
         /* datos perdidos */
     }

     /* rearmar recepción */
     HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, uart_rx_dma_buffer, UART_RX_DMA_BUFFER_SIZE);

     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
}

//int UART_ReadByte(uint8_t *byte, uint32_t timeout_ms)
int COM_O3_GetChar_Bis(uint8_t *byte, uint32_t timeout_ms)
{
    size_t n;

    n = xStreamBufferReceive(
            uartRxStream,
            byte,
            1,                         // just one byte
            pdMS_TO_TICKS(timeout_ms));

    if(n == 1)
        return 1;   // byte received

    return 0;       // timeout
}


