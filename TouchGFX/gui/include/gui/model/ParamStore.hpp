/*
 * ParamStore.hpp
 *
 *  Created on: Dec 18, 2025
 *      Author: Roberto.Sanchez
 */

#ifndef APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARAMSTORE_HPP_
#define APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARAMSTORE_HPP_

typedef enum
{
    P_STORE_OK = 0,             /* Operation successful */
    P_STORE_ERR_OPEN = -1,      /* File open failed */
    P_STORE_ERR_NOT_FOUND = -2, /* PARAM_xxx not found */
    P_STORE_ERR_FORMAT = -3     /* Invalid line format or null pointer */
} P_STORE_STATUS_E;

int8_t p_store_openFileWrite(const char *filePath);
void   p_store_closeFile(void);
int8_t p_store_writeLine(uint16_t storageId, int32_t value);
int8_t p_store_readLine(uint16_t storageId, int32_t *value);
int8_t p_store_openWrite(void);
int8_t p_store_openRead(void);
void   p_store_close(void);

#endif /* APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARAMSTORE_HPP_ */
