/**
  ******************************************************************************
  * @file    ILI2511v6.h
  * @author  EDT Embedded Application Team
  * @version V1.0.0
  * @date    02-AUG-2022
  * @brief   This file contains all the functions prototypes for the
  *          ILI2511v6.c Touch screen driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 EDT</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI2511V6_H
#define __ILI2511V6_H

#ifdef __cplusplus
extern "C" {
#endif

/* Set Multi-touch as supported */
#if !defined(TS_MONO_TOUCH_SUPPORTED)
#define TS_MULTI_TOUCH_SUPPORTED        1
#endif /* TS_MONO_TOUCH_SUPPORTED */

/* Includes ------------------------------------------------------------------*/

#include "../Common/ts.h"

/* Macros --------------------------------------------------------------------*/

#if defined(ILI2511_ENABLE_ASSERT)
/* Assert activated */
#define ILI2511_ASSERT(__condition__)      do { if(__condition__) \
                                               {  \
                                                 while(1);  \
                                               } \
                                          }while(0)
#else
/* Assert not activated : macro has no effect */
#define ILI2511_ASSERT(__condition__)    do { if(__condition__) \
                                             {  \
                                                ;  \
                                             } \
                                            }while(0)
#endif /* ILI2511_ENABLE_ASSERT == 1 */

/** @typedef ili2511_handle_TypeDef
 *  ili2511 Handle definition.
 */

typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} ili2511_handle_TypeDef; 

typedef struct
{
  uint8_t t0;
  uint8_t xh : 7;
  uint8_t td : 1;
  uint8_t xl;
  uint8_t yh;
  uint8_t yl;
  uint8_t dummy;
} ili2511_V3_Pos_t;

typedef struct
{
  uint8_t td;
  uint8_t dummy;
  uint8_t xl;
  uint8_t xh;
  uint8_t yl;
  uint8_t yh;
} ili2511_V6_Pos_t;

//typedef union
//{
//  uint8_t pos[6];      
//  ili2511_V3_Pos_t xy;
//}ili2511_XY_V3_t;
//
//typedef union
//{
//  uint8_t pos[6];      
//  ili2511_V6_Pos_t xy;
//}ili2511_XY_V6_t;
typedef union
{
  uint8_t buf[64];
  ili2511_V3_Pos_t v3;
  ili2511_V6_Pos_t v6;
}ili2511_XY_t;
typedef struct _ili2511_pos_t
{
  ili2511_XY_t xy;
  uint8_t version;
  uint8_t buf_len;
}ili2511_pos_t;
//#if ( UI2_EVK070023 == 1U)
#if 0
#define RX_BUF_SIZE			((uint8_t)64)
#define P1_XL_REG			((uint8_t)0x02)
#define P1_XH_REG			((uint8_t)0x03)
#define P1_YL_REG			((uint8_t)0x04)
#define P1_YH_REG			((uint8_t)0x05)
#define NB_TOUCH_REG			((uint8_t)0x0)
#define NB_TOUCH_MASK			((uint8_t)0xFF)
#define NB_TOUCH_DETECTED		((uint8_t)0x48)
#else
#define RX_BUF_SIZE			((uint8_t)6)
#define P1_XH_REG			((uint8_t)0x01)
#define P1_XL_REG			((uint8_t)0x02)
#define P1_YH_REG			((uint8_t)0x03)
#define P1_YL_REG			((uint8_t)0x04)
#define NB_TOUCH_REG			((uint8_t)0x01)
#define NB_TOUCH_MASK			((uint8_t)0x80)
#define NB_TOUCH_DETECTED		((uint8_t)0x80)
#endif

  /* I2C Slave address of touchscreen FocalTech ILI2511 */
#define ILI2511_I2C_SLAVE_ADDRESS            ((uint8_t)0x82)

  /* Maximum border values of the touchscreen pad */
#define ILI2511_MAX_RESOLUTION_X             ((uint16_t)16384)    
#define ILI2511_MAX_RESOLUTION_Y             ((uint16_t)9600)     
#define ILI2511_XY_RATIO                 ((uint16_t)16)  
  /* Possible values of driver functions return status */
#define ILI2511_STATUS_OK                    ((uint8_t)0x00)
#define ILI2511_STATUS_NOT_OK                ((uint8_t)0x01)

  /* Possible values of global variable 'TS_I2C_Initialized' */
#define ILI2511_I2C_NOT_INITIALIZED          ((uint8_t)0x00)
#define ILI2511_I2C_INITIALIZED              ((uint8_t)0x01)

  /* Max detectable simultaneous touches */
#define ILI2511_MAX_DETECTABLE_TOUCH         ((uint8_t)0x05)

  /**
   * @brief : Definitions for ILI2511 I2C register addresses on 8 bit
   **/

  /* Current mode register of the ILI2511 (R/W) */
#define ILI2511_DEV_MODE_REG                 ((uint8_t)0x00)

  /* Possible values of ILI2511_DEV_MODE_REG */
#define ILI2511_DEV_MODE_WORKING             ((uint8_t)0x00)
#define ILI2511_DEV_MODE_FACTORY             ((uint8_t)0x04)

#define ILI2511_DEV_MODE_MASK                ((uint8_t)0x07)
#define ILI2511_DEV_MODE_SHIFT               ((uint8_t)0x04)

  /* Touch Data Status register : gives number of active touch points (0..5) */
#define ILI2511_TD_STAT_REG                  ((uint8_t)0x10)  // Modified for Ilitek

#define ILI2511_TD_STAT_MASK                 ((uint8_t)0x80)  // Key ON Flag in Ilitek
#define ILI2511_TD_STAT_SHIFT                ((uint8_t)0x07)  // Shift 7 >>

  /* Values Pn_XH and Pn_YH related */
#define ILI2511_TOUCH_EVT_FLAG_PRESS_DOWN    ((uint8_t)0x00)
#define ILI2511_TOUCH_EVT_FLAG_LIFT_UP       ((uint8_t)0x01)
#define ILI2511_TOUCH_EVT_FLAG_CONTACT       ((uint8_t)0x02)
#define ILI2511_TOUCH_EVT_FLAG_NO_EVENT      ((uint8_t)0x03)

#define ILI2511_TOUCH_EVT_FLAG_SHIFT         ((uint8_t)0x06)
#define ILI2511_TOUCH_EVT_FLAG_MASK          ((uint8_t)(3 << ILI2511_TOUCH_EVT_FLAG_SHIFT)) 

#define ILI2511_TOUCH_POS_MSB_MASK           ((uint8_t)0x0F)
#define ILI2511_TOUCH_POS_MSB_SHIFT          ((uint8_t)0x00)

  /* Values Pn_XL and Pn_YL related */
#define ILI2511_TOUCH_POS_LSB_MASK           ((uint8_t)0xFF)
#define ILI2511_TOUCH_POS_LSB_SHIFT          ((uint8_t)0x00)

#define ILI2511_P1_XH_REG                    ((uint8_t)0x03)
#define ILI2511_P1_XL_REG                    ((uint8_t)0x04)
#define ILI2511_P1_YH_REG                    ((uint8_t)0x05)
#define ILI2511_P1_YL_REG                    ((uint8_t)0x06)

/* Touch Pressure register value (R) */
#define ILI2511_P1_WEIGHT_REG                ((uint8_t)0x07)

/* Values Pn_WEIGHT related  */
#define ILI2511_TOUCH_WEIGHT_MASK            ((uint8_t)0xFF)
#define ILI2511_TOUCH_WEIGHT_SHIFT           ((uint8_t)0x00)

/* Touch area register */
#define ILI2511_P1_MISC_REG                  ((uint8_t)0x08)

/* Values related to ILI2511_Pn_MISC_REG */
#define ILI2511_TOUCH_AREA_MASK              ((uint8_t)(0x04 << 4))
#define ILI2511_TOUCH_AREA_SHIFT             ((uint8_t)0x04)

#define ILI2511_P2_XH_REG                    ((uint8_t)0x09)
#define ILI2511_P2_XL_REG                    ((uint8_t)0x0A)
#define ILI2511_P2_YH_REG                    ((uint8_t)0x0B)
#define ILI2511_P2_YL_REG                    ((uint8_t)0x0C)
#define ILI2511_P2_WEIGHT_REG                ((uint8_t)0x0D)
#define ILI2511_P2_MISC_REG                  ((uint8_t)0x0E)

#define ILI2511_P3_XH_REG                    ((uint8_t)0x0F)
#define ILI2511_P3_XL_REG                    ((uint8_t)0x10)
#define ILI2511_P3_YH_REG                    ((uint8_t)0x11)
#define ILI2511_P3_YL_REG                    ((uint8_t)0x12)
#define ILI2511_P3_WEIGHT_REG                ((uint8_t)0x13)
#define ILI2511_P3_MISC_REG                  ((uint8_t)0x14)

#define ILI2511_P4_XH_REG                    ((uint8_t)0x15)
#define ILI2511_P4_XL_REG                    ((uint8_t)0x16)
#define ILI2511_P4_YH_REG                    ((uint8_t)0x17)
#define ILI2511_P4_YL_REG                    ((uint8_t)0x18)
#define ILI2511_P4_WEIGHT_REG                ((uint8_t)0x19)
#define ILI2511_P4_MISC_REG                  ((uint8_t)0x1A)

#define ILI2511_P5_XH_REG                    ((uint8_t)0x1B)
#define ILI25116_P5_XL_REG                    ((uint8_t)0x1C)
#define ILI2511_P5_YH_REG                    ((uint8_t)0x1D)
#define ILI2511_P5_YL_REG                    ((uint8_t)0x1E)
#define ILI2511_P5_WEIGHT_REG                ((uint8_t)0x1F)
#define ILI2511_P5_MISC_REG                  ((uint8_t)0x20)

#define ILI2511_P6_XH_REG                    ((uint8_t)0x21)
#define ILI2511_P6_XL_REG                    ((uint8_t)0x22)
#define ILI2511_P6_YH_REG                    ((uint8_t)0x23)
#define ILI2511_P6_YL_REG                    ((uint8_t)0x24)
#define ILI2511_P6_WEIGHT_REG                ((uint8_t)0x25)
#define ILI2511_P6_MISC_REG                  ((uint8_t)0x26)

#define ILI2511_P7_XH_REG                    ((uint8_t)0x27)
#define ILI2511_P7_XL_REG                    ((uint8_t)0x28)
#define ILI2511_P7_YH_REG                    ((uint8_t)0x29)
#define ILI2511_P7_YL_REG                    ((uint8_t)0x2A)
#define ILI2511_P7_WEIGHT_REG                ((uint8_t)0x2B)
#define ILI2511_P7_MISC_REG                  ((uint8_t)0x2C)

#define ILI2511_P8_XH_REG                    ((uint8_t)0x2D)
#define ILI2511_P8_XL_REG                    ((uint8_t)0x2E)
#define ILI2511_P8_YH_REG                    ((uint8_t)0x2F)
#define ILI2511_P8_YL_REG                    ((uint8_t)0x30)
#define ILI2511_P8_WEIGHT_REG                ((uint8_t)0x31)
#define ILI2511_P8_MISC_REG                  ((uint8_t)0x32)

#define ILI2511_P9_XH_REG                    ((uint8_t)0x33)
#define ILI2511_P9_XL_REG                    ((uint8_t)0x34)
#define ILI2511_P9_YH_REG                    ((uint8_t)0x35)
#define ILI2511_P9_YL_REG                    ((uint8_t)0x36)
#define ILI2511_P9_WEIGHT_REG                ((uint8_t)0x37)
#define ILI2511_P9_MISC_REG                  ((uint8_t)0x38)

#define ILI2511_P10_XH_REG                   ((uint8_t)0x39)
#define ILI2511_P10_XL_REG                   ((uint8_t)0x3A)
#define ILI2511_P10_YH_REG                   ((uint8_t)0x3B)
#define ILI2511_P10_YL_REG                   ((uint8_t)0x3C)
#define ILI2511_P10_WEIGHT_REG               ((uint8_t)0x3D)
#define ILI2511_P10_MISC_REG                 ((uint8_t)0x3E)

  /* Threshold for touch detection */
#define ILI2511_TH_GROUP_REG                 ((uint8_t)0x80) // CHANNING

  /* Values ILI2511_TH_GROUP_REG : threshold related  */
#define ILI2511_THRESHOLD_MASK               ((uint8_t)0xFF)
#define ILI2511_THRESHOLD_SHIFT              ((uint8_t)0x00)

  /* Filter function coefficients */
#define ILI2511_TH_DIFF_REG                  ((uint8_t)0x85)

  /* Control register */
#define ILI2511_CTRL_REG                     ((uint8_t)0x86)

  /* Values related to ILI2511_CTRL_REG */

  /* Will keep the Active mode when there is no touching */
#define ILI2511_CTRL_KEEP_ACTIVE_MODE        ((uint8_t)0x00)

  /* Switching from Active mode to Monitor mode automatically when there is no touching */
#define ILI2511_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE  ((uint8_t)0x01

  /* The time period of switching from Active mode to Monitor mode when there is no touching */
#define ILI2511_TIMEENTERMONITOR_REG         ((uint8_t)0x87)

  /* Report rate in Active mode */
#define ILI2511_PERIODACTIVE_REG             ((uint8_t)0x88)

  /* Report rate in Monitor mode */
#define ILI2511_PERIODMONITOR_REG            ((uint8_t)0x89)

  /* The value of the minimum allowed angle while Rotating gesture mode */
#define ILI2511_RADIAN_VALUE_REG             ((uint8_t)0x91)

  /* Maximum offset while Moving Left and Moving Right gesture */
#define ILI2511_OFFSET_LEFT_RIGHT_REG        ((uint8_t)0x92)

  /* Maximum offset while Moving Up and Moving Down gesture */
#define ILI2511_OFFSET_UP_DOWN_REG           ((uint8_t)0x93)

  /* Minimum distance while Moving Left and Moving Right gesture */
#define ILI2511_DISTANCE_LEFT_RIGHT_REG      ((uint8_t)0x94)

  /* Minimum distance while Moving Up and Moving Down gesture */
#define ILI2511_DISTANCE_UP_DOWN_REG         ((uint8_t)0x95)

  /* Maximum distance while Zoom In and Zoom Out gesture */
#define ILI2511_DISTANCE_ZOOM_REG            ((uint8_t)0x96)

  /* High 8-bit of LIB Version info */
#define ILI2511_LIB_VER_H_REG                ((uint8_t)0xA1)

  /* Low 8-bit of LIB Version info */
#define ILI2511_LIB_VER_L_REG                ((uint8_t)0xA2)

  /* Chip Selecting */
#define ILI2511_CIPHER_REG                   ((uint8_t)0xA3)

  /* Interrupt mode register (used when in interrupt mode) */
#define ILI2511_GMODE_REG                    ((uint8_t)0xA4)

#define ILI2511_G_MODE_INTERRUPT_MASK        ((uint8_t)0x03)
#define ILI2511_G_MODE_INTERRUPT_SHIFT       ((uint8_t)0x00)

  /* Possible values of ILI2511_GMODE_REG */
#define ILI2511_G_MODE_INTERRUPT_POLLING     ((uint8_t)0x00)
#define ILI2511_G_MODE_INTERRUPT_TRIGGER     ((uint8_t)0x01)

  /* Current power mode the ILI2511 system is in (R) */
#define ILI2511_PWR_MODE_REG                 ((uint8_t)0xA5)

  /* ILI2511 firmware version */
#define ILI2511_FIRMID_REG                   ((uint8_t)0xA6)

  /* ILI2511 Chip identification register */
#define ILI2511_CHIP_ID_REG                  ((uint8_t)0xA8)

  /*  Possible values of ILI2511_CHIP_ID_REG */
#define ILI2511_ID_VALUE                     ((uint8_t)0x51)

  /* Release code version */
#define ILI2511_RELEASE_CODE_ID_REG          ((uint8_t)0xAF)

  /* Current operating mode the ILI2511 system is in (R) */
#define ILI2511_STATE_REG                    ((uint8_t)0xBC)

#define PROTOCOL_V3			((uint8_t)0x03)
#define PROTOCOL_V6			((uint8_t)0x06)
  /* Exported macro ------------------------------------------------------------*/

  /* Exported functions --------------------------------------------------------*/


/**
 * @brief  Initialize the ILI2511 communication bus
 *         from MCU to ILI2511 : ie I2C channel initialization (if required).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
 * @retval None
 */
void ili2511_Init(uint16_t DeviceAddr,uint16_t MaxX,uint16_t MaxY);

/**
 * @brief  Software Reset the ILI2511.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
 * @retval None
 */
void ili2511_Reset(uint16_t DeviceAddr);

/**
 * @brief  Read the ILI2511 device ID, pre initialize I2C in case of need to be
 *         able to read the ILI2511 device ID, and verify this is a ILI2511.
 * @param  DeviceAddr: I2C ILI2511 Slave address.
 * @retval The Device ID (two bytes).
 */
uint16_t ili2511_ReadID(uint16_t DeviceAddr);

/**
 * @brief  Configures the touch Screen IC device to start detecting touches
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
 * @retval None.
 */
void ili2511_TS_Start(uint16_t DeviceAddr);

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */
uint8_t ili2511_TS_DetectTouch(uint16_t DeviceAddr);

/**
 * @brief  Get the touch screen X and Y positions values
 *         Manage multi touch thanks to touch Index global
 *         variable 'ILI2511_handle.currActiveTouchIdx'.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  X: Pointer to X position value
 * @param  Y: Pointer to Y position value
 * @retval None.
 */
void ili2511_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);

/**
 * @brief  Configure the ILI2511 device to generate IT on given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of ILI2511).
 * @retval None
 */
void ili2511_TS_EnableIT(uint16_t DeviceAddr);

/**
 * @brief  Configure the ILI2511 device to stop generating IT on the given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of ILI2511).
 * @retval None
 */
void ili2511_TS_DisableIT(uint16_t DeviceAddr);

/**
 * @brief  Get IT status from ILI2511 interrupt status registers
 *         Should be called Following an EXTI coming to the MCU to know the detailed
 *         reason of the interrupt.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
 * @retval TS interrupts status
 */
uint8_t ili2511_TS_ITStatus (uint16_t DeviceAddr);

/**
 * @brief  Clear IT status in ILI2511 interrupt status clear registers
 *         Should be called Following an EXTI coming to the MCU.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
 * @retval TS interrupts status
 */
void ili2511_TS_ClearIT (uint16_t DeviceAddr);

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/



 //cancel //
//#if (TS_MULTI_TOUCH_SUPPORTED == 1)

///**
// * @brief  Get the last touch gesture identification (zoom, move up/down...).
// * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
// * @param  pGestureId : Pointer to get last touch gesture Identification.
// * @retval None.
// */

//void Device_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);
//extern void Device_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);

///**
// * @brief  Get the touch detailed informations on touch number 'touchIdx' (0..1)
// *         This touch detailed information contains :
// *         - weight that was applied to this touch
// *         - sub-area of the touch in the touch panel
// *         - event of linked to the touch (press down, lift up, ...)
// * @param  DeviceAddr: Device address on communication Bus (I2C slave address of ILI2511).
// * @param  touchIdx : Passed index of the touch (0..1) on which we want to get the
// *                    detailed information.
// * @param  pWeight : Pointer to to get the weight information of 'touchIdx'.
// * @param  pArea   : Pointer to to get the sub-area information of 'touchIdx'.
// * @param  pEvent  : Pointer to to get the event information of 'touchIdx'.
//
// * @retval None.
// */
//
//
//void Device_TS_GetTouchInfo(uint16_t   DeviceAddr,
//                            uint32_t   touchIdx,
//                            uint32_t * pWeight,
//                            uint32_t * pArea,
//                            uint32_t * pEvent);
//
//extern void Device_TS_GetTouchInfo(uint16_t   DeviceAddr,
//                            uint32_t   touchIdx,
//                            uint32_t * pWeight,
//                            uint32_t * pArea,
//                            uint32_t * pEvent);
//
//#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */



/* Imported TS IO functions --------------------------------------------------------*/

/** @defgroup ILI2511_Imported_Functions
 * @{
 */

/* TouchScreen (TS) external IO functions */


  /* Imported global variables --------------------------------------------------------*/

extern void    TS_IO_Init(void);
extern void    TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg);
extern void    TS_IO_Read_Multi(uint8_t Addr, uint8_t Reg, uint8_t* buffer);
//extern void    TS_IO_Delay(uint32_t Delay);

  /** @defgroup ILI2511_Imported_Globals
   * @{
   */


/* Touch screen driver structure */
extern TS_DrvTypeDef ili2511_ts_drv;

#ifdef __cplusplus
}
#endif
#endif /* __ILI2511V6_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
