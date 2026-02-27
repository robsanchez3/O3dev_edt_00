/*
 * LogManage.hpp
 *
 *  Created on: Dec 18, 2025
 *      Author: Roberto.Sanchez
 */

#ifndef APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_LOGMANAGE_HPP_
#define APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_LOGMANAGE_HPP_

typedef enum
{
    LOG_OK = 0,           	/* Operation successful */
    LOG_ERR_OPEN = -1,    	/* File open failed */
    LOG_ERR_NOT_FOUND = -2, /* PARAM_xxx not found */
    LOG_ERR_FORMAT = -3   	/* Invalid line format or null pointer */
} LOG_STATUS_E;

int8_t openLogFileWrite(const char *filePath);
void   closeLogFile(void);
int8_t writeLogLine(uint16_t storageId, int32_t value);
int8_t readLogLine(uint16_t storageId, int32_t *value);
int8_t openParamFileWrite(void);
int8_t openParamFileRead(void);
void   closeParamFile(void);

#endif /* APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_LOGMANAGE_HPP_ */
