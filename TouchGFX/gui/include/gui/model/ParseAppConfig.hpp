/*
 * ParseAppConfig.hpp
 *
 *  Created on: Dec 16, 2025
 *      Author: Roberto.Sanchez
 */

#ifndef APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEAPPCONFIG_HPP_
#define APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEAPPCONFIG_HPP_

#define EMPTY_VALUE   -99   // impossible value on this context
#define LINE_BUF_SIZE 256

// Context structure for .mod file parsing (.ini structure file)
typedef struct {
    OPERATION_MODE_E selectedMode;
    OPERATION_MODE_E currentMode;
    char inModeSection;
    char inStepSection;
    int currentStep;
    THERAPY_CTX *ctx;
} ModCtx;

typedef struct {
    const char* name;
    int value;
} ENUM_MAP;

void print_guiTherapyCtx(const THERAPY_CTX *ctx);
uint8_t loadTherapyFromFile(OPERATION_MODE_E mode, THERAPY_CTX *guiTherapyCtx);
int16 loadMainMenu(signed char * decode_out, int16 maxDevTherapies);
int16 loadHardwareConfig(HW_CONFIG_T *cfg);
int16 loadSyringeConfig(uint16 *cfg);
int16 loadUserConfig(USR_CONFIG_T *cfg);
int16 saveUserConfig(const USR_CONFIG_T *cfg);



#endif /* APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEAPPCONFIG_HPP_ */
