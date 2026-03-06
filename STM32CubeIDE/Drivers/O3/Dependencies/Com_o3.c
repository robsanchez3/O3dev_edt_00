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

COM_O3_LOOP_BUFFER_T comBuff;

//#define USE_EXPANSION_TTL_USART6
/*
#if defined(USE_EXPANSION_TTL_USART6)
#define COM_O3_TX_UART_HANDLE (&huart6)
#else
#define COM_O3_TX_UART_HANDLE (&hRs232)
#endif
*/

//#define COM_O3_TX_UART_HANDLE (&huart2)
#define COM_O3_TX_UART_HANDLE (&huart6)
//#define COM_O3_TX_UART_HANDLE (&huart1)
//#define COM_O3_TX_UART_HANDLE (&huart3)

#if defined(USE_EXPANSION_TTL_USART6)
static HAL_StatusTypeDef COM_O3_ArmRx6(void);
#endif

static HAL_StatusTypeDef COM_O3_ArmRx6(void);

void COM_O3_Init(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
	memset(comBuff.data, 0, COM_O3_LOOP_BUFFER_LEN);

#if defined(USE_EXPANSION_TTL_USART6)
	HAL_StatusTypeDef rxStart;
	rxStart = COM_O3_ArmRx6();
	printf("COM_O3_Init RX6 arm status=%d\n", (int)rxStart);
#endif

	HAL_StatusTypeDef rxStart;
	rxStart = COM_O3_ArmRx6();
	printf("COM_O3_Init RX arm status=%d\n", (int)rxStart);

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
#if 0
	uint8_t i;

	if(_RS232RevSt.RevF==true)
	{
		for(i=0; i<_RS232RevSt.size; i++)
		{
			comBuff.data[comBuff.head++] = _RS232RevSt.pdata[i];

			if(comBuff.head == COM_O3_LOOP_BUFFER_LEN)
			{
				comBuff.head = 0;
			}
		}

		_RS232RevSt.RevF=false;
		memset(_RS232RevSt.pdata,0,sizeof(_RS232RevSt.pdata));
		_RS232RevSt.size=0;
	}
#endif
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

//#if defined(USE_EXPANSION_TTL_USART6)
#if 1
static HAL_StatusTypeDef COM_O3_ArmRx6(void)
{
	__HAL_UART_CLEAR_FLAG(COM_O3_TX_UART_HANDLE, UART_CLEAR_IDLEF);
	__HAL_UART_CLEAR_OREFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_NEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_FEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_PEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_SEND_REQ(COM_O3_TX_UART_HANDLE, UART_RXDATA_FLUSH_REQUEST);

	return HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, &comBuff.data[comBuff.head], COM_O3_LOOP_BUFFER_LEN);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

	if ( (huart == NULL) ||  ((huart->Instance != USART6) && (huart->Instance != USART1) && (huart->Instance != USART3)) )
	{
		return;
	}

	comBuff.head = (uint8_t)(comBuff.head + Size);
	if (comBuff.head >= COM_O3_LOOP_BUFFER_LEN)
	{
		comBuff.head = 0;
	}
	printf("RX received = %u bytes\n", (unsigned int)Size);

	HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, &comBuff.data[comBuff.head], COM_O3_LOOP_BUFFER_LEN);
}
#endif
