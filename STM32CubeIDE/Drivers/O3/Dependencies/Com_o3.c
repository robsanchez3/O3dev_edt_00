/*
 * Com_o3.c
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */


#include <stdio.h>
#include <string.h>
//#include <../vendor/Board/SE21070/Driver/edt_f7xx_usart.h>
#include <../Vendor/Driver/edt_bsp_uart.h>
#include <Dependencies/Com_o3.h>
//////////////////#include <gui/widget/edt_f7xxh7xx_TestAPI.h>   TODO ¿quitar widget?

COM_O3_LOOP_BUFFER_T comBuff;

// Variable estática para recepción por interrupción
static uint8_t com_o3_rx_byte;

void COM_O3_Init(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
	memset(comBuff.data, 0, COM_O3_LOOP_BUFFER_LEN);

	// TODO New reception system due to new hw (REVIEW)
	// Iniciar recepción por interrupción
	HAL_UART_Receive_IT(&hRs232, &com_o3_rx_byte, 1);
}

void COM_O3_Clear(void)
{
	comBuff.head = 0;
	comBuff.tail = 0;
}

void COM_O3_SendConst(const char *Data)
{
	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen (Data));
//	EDT_UART_Transmit_IT(&hRs485, (uint8_t *)Data, (uint16_t) strlen (Data));
}

void COM_O3_Send(uint8_t *Data)
{
	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
//	EDT_UART_Transmit_IT(&hRs485, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
}

void COM_O3_PutString(uint8_t *Data)
{
//	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
//	EDT_UART_Transmit_IT(&hRs485, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data));
	EDT_UART_Transmit(&hRs232, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data), HAL_MAX_DELAY);
//	EDT_UART_Transmit(&hRs485, (uint8_t *)Data, (uint16_t) strlen ((const char *)Data), HAL_MAX_DELAY);
}

void COM_O3_PollRx(void)
{
	uint8_t i;
#if 0
	// legacy obsolete TODO remove this code when config by means of therapy templates be well tested and validated 
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

// TODO New reception system due to new hw (REVIEW)
// Implementación de la función de callback de recepción por interrupción
void EDT_RS232_RxCpltCallback(void)
{
	// Almacenar el byte recibido en el buffer circular
	comBuff.data[comBuff.head++] = com_o3_rx_byte;
	if (comBuff.head == COM_O3_LOOP_BUFFER_LEN)
	{
		comBuff.head = 0;
	}
	// Reiniciar la recepción por interrupción
	HAL_UART_Receive_IT(&hRs232, &com_o3_rx_byte, 1);
}
