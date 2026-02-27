/*
 * Delay_o3.c
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */

#include "Delay_o3.h" //  osDelay
#include "cmsis_os2.h" //  osDelay

void Delay_o3_ms(uint32 ms)
{
	osDelay(ms);
}
