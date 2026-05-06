/*
 * dep_o3_stm32U599.c
 *
 * O3 library platform dependencies implementation for STM32U599.
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <../Vendor/Driver/edt_bsp_uart.h>
#include "usart.h"
#include "dep_o3.h"
#include <gui/widget/edt_f7xxh7xx_TestAPI.h>
#include "cmsis_os2.h"

/*
    Usart 2 - main board CN2
	Usart 6 - fan out expansion board Tx(pin 21) Rx(pin 23)
	Usart 1 - fan out expansion board Tx(pin 06) Rx(pin 08) - 27 Series Connector Board CN5 Tx(pin 05) Rx(pin 04) control (pin 01) - Low power UART (LPUART)
	Usart 3 - fan out expansion board Tx(pin 22) Rx(pin 24) - 27 Series Connector Board CN5 Tx(pin 03) Rx(pin 02)
*/

//#define COM_O3_TX_UART_HANDLE (&huart2)
//#define COM_O3_TX_UART_HANDLE (&huart6)
//#define COM_O3_TX_UART_HANDLE (&huart1)
#define COM_O3_TX_UART_HANDLE (&huart3)


/* ---- COM ---- */

#define COM_O3_LOOP_BUFFER_LEN	128

typedef struct com_o3_loopBuffer_t
{
	uint8_t head;
	uint8_t tail;
	uint8_t data[COM_O3_LOOP_BUFFER_LEN];
}COM_O3_LOOP_BUFFER_T;

uint8_t COM_O3_Ready = 0;
COM_O3_LOOP_BUFFER_T comBuff;
uint8_t tmpBuff[COM_O3_LOOP_BUFFER_LEN/2];

static HAL_StatusTypeDef COM_O3_Arm_Rx(void)
{
	__HAL_UART_CLEAR_FLAG(COM_O3_TX_UART_HANDLE, UART_CLEAR_IDLEF);
	__HAL_UART_CLEAR_OREFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_NEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_FEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_CLEAR_PEFLAG(COM_O3_TX_UART_HANDLE);
	__HAL_UART_SEND_REQ(COM_O3_TX_UART_HANDLE, UART_RXDATA_FLUSH_REQUEST);

	return HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, tmpBuff, sizeof(tmpBuff));
}

void dep_o3_com_init(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
	memset(tmpBuff, 0, sizeof(tmpBuff));
	memset(comBuff.data, 0, COM_O3_LOOP_BUFFER_LEN);

	COM_O3_Arm_Rx();
	COM_O3_Ready = 1;
}

void dep_o3_com_clear(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
}

void dep_o3_com_sendConst(const char *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen(Data));
}

void dep_o3_com_send(uint8_t *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
}

void dep_o3_com_putString(uint8_t *Data)
{
	HAL_UART_Transmit_IT(COM_O3_TX_UART_HANDLE, (uint8_t *)Data, (uint16_t)strlen((const char *)Data));
}

uint8_t dep_o3_com_getChar(uint8_t *data)
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

uint8_t dep_o3_com_dataAvailable(void)
{
	if(comBuff.head != comBuff.tail)
	{
        return 1;
	}
	return 0;
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

	memset(tmpBuff, 0, sizeof(tmpBuff));
	HAL_UARTEx_ReceiveToIdle_IT(COM_O3_TX_UART_HANDLE, tmpBuff, sizeof(tmpBuff));
}

void dep_o3_com_pollRx(void)
{
}

/* Transfers USART3 ownership to the IAP bootloader protocol.
 * Sets COM_O3_Ready=0 so HAL_UARTEx_RxEventCallback does not re-arm IT
 * reception, then aborts any active IT transfer leaving the UART in
 * HAL_UART_STATE_READY for blocking IAP use.
 * Called once before dev_upd_gen_fw_task_fn() starts the IAP cycle.
 * No restore needed: system resets via HAL_NVIC_SystemReset() after IAP. */
void dep_o3_com_iap_takeover(void)
{
    COM_O3_Ready = 0;
    HAL_UART_Abort(COM_O3_TX_UART_HANDLE);
}

/* ---- Delay ---- */

void dep_o3_delay_ms(uint32_t ms)
{
	osDelay(ms);
}

/* ---- Debug output ---- */

void dep_o3_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void dep_o3_vprintf(const char *fmt, va_list args)
{
	vprintf(fmt, args);
}

/* ---- Platform ---- */

void dep_o3_softwareReset(void)
{
}

void dep_o3_beep(void)
{
}
