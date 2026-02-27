/*
 * Com_o3.h
 *
 *  Created on: Jan 28, 2026
 *      Author: Roberto.Sanchez
 */

#ifndef COM_O3_H_
#define COM_O3_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define COM_O3_LOOP_BUFFER_LEN	128

typedef struct com_o3_loopBuffer_t
{
	uint8_t head;
	uint8_t tail;
	uint8_t data[COM_O3_LOOP_BUFFER_LEN];
}COM_O3_LOOP_BUFFER_T;

void COM_O3_Init(void);
void COM_O3_Clear(void);
void COM_O3_SendConst(const char *);
void COM_O3_Send(uint8_t *);
void COM_O3_PutString(uint8_t *Data);
void COM_O3_PollRx(void);
uint8_t COM_O3_GetChar(uint8_t *);
uint8_t COM_O3_DataAvailable(void);

#ifdef __cplusplus
}
#endif


#endif /*COM_O3_H_ */
