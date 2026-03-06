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

#if defined(USE_EXPANSION_TTL_USART6)
static HAL_StatusTypeDef COM_O3_ArmRx6(void);
#endif

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
}

void COM_O3_Clear(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
}

void COM_O3_SendConst(const char *Data)
{
#if defined(USE_EXPANSION_TTL_USART6)
	HAL_UART_Transmit_IT(&huart6, (uint8_t *)Data, (uint16_t)strlen(Data));
#else
	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen (Data));
#endif
}

void COM_O3_Send(uint8_t *Data)
{
#if defined(USE_EXPANSION_TTL_USART6)
	HAL_UART_Transmit_IT(&huart6, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
#else
	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
#endif
}

void COM_O3_PutString(uint8_t *Data)
{
#if defined(USE_EXPANSION_TTL_USART6)
	HAL_UART_Transmit_IT(&huart6, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
#else
	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
#endif
}

void COM_O3_PollRx(void)
{
#if !defined(USE_EXPANSION_TTL_USART6)
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
#if defined(USE_EXPANSION_TTL_USART6)
		printf("RX6_CHAR=0x%02X ('%c')\n",
		       (unsigned int)(*data),
		       ((*data) >= 32U && (*data) <= 126U) ? (char)(*data) : '.');
#endif

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

#if defined(USE_EXPANSION_TTL_USART6)
static HAL_StatusTypeDef COM_O3_ArmRx6(void)
{
	uint16_t chunkLen;

	chunkLen = (uint16_t)(COM_O3_LOOP_BUFFER_LEN - comBuff.head);
	if (chunkLen == 0U)
	{
		comBuff.head = 0;
		chunkLen = COM_O3_LOOP_BUFFER_LEN;
	}

	__HAL_UART_CLEAR_FLAG(&huart6, UART_CLEAR_IDLEF);
	__HAL_UART_CLEAR_OREFLAG(&huart6);
	__HAL_UART_CLEAR_NEFLAG(&huart6);
	__HAL_UART_CLEAR_FEFLAG(&huart6);
	__HAL_UART_CLEAR_PEFLAG(&huart6);
	__HAL_UART_SEND_REQ(&huart6, UART_RXDATA_FLUSH_REQUEST);

	return HAL_UARTEx_ReceiveToIdle_IT(&huart6, &comBuff.data[comBuff.head], chunkLen);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	HAL_StatusTypeDef rxRestart;

	if ((huart == NULL) || (huart->Instance != USART6))
	{
		return;
	}

	comBuff.head = (uint8_t)(comBuff.head + Size);
	if (comBuff.head >= COM_O3_LOOP_BUFFER_LEN)
	{
		comBuff.head = 0;
	}
	printf("RX6 chunk=%u\n", (unsigned int)Size);

	rxRestart = COM_O3_ArmRx6();
	if (rxRestart != HAL_OK)
	{
		printf("RX6 rearm status=%d\n", (int)rxRestart);
	}
}
#endif
