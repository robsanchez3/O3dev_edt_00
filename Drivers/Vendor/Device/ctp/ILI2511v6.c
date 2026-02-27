/**
  ******************************************************************************
  * @file    ILI2511v6.c
  * @author  EDT Embedded Application Team
  * @version V1.0.0
  * @date    02-AUG-2022
  * @brief   This file provides a set of functions needed to manage the Touch 
  *          Screen on EDT smart embedded board.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ili2511v6.h"
#include "edt_bsp_ctp.h"
//#include "ili2511.h"
//static uint8_t ctp_rx_buffer[64];
static uint16_t ILI2511_MAX_WIDTH = 0;
static uint16_t ILI2511_MAX_HEIGHT = 0;
/* Private define ------------------------------------------------------------*/

/* Touch screen driver structure initialization */
TS_DrvTypeDef ili2511_ts_drv =
{
  ili2511_Init,
  ili2511_ReadID,
  ili2511_Reset,

  ili2511_TS_Start,
  ili2511_TS_DetectTouch,
  ili2511_TS_GetXY,

  ili2511_TS_EnableIT,
  ili2511_TS_ClearIT,
  ili2511_TS_ITStatus,
  ili2511_TS_DisableIT
};

/* Global ili2511 handle */
static ili2511_handle_TypeDef ili2511_handle = { ILI2511_I2C_NOT_INITIALIZED, 0, 0};


/* Private functions prototypes-----------------------------------------------*/

/**
  * @brief  Return the status of I2C was initialized or not.
  * @param  None.
  * @retval : I2C initialization status.
  */
static uint8_t ili2511_Get_I2C_InitializedStatus(void);

/**
  * @brief  I2C initialize if needed.
  * @param  None.
  * @retval : None.
  */
static void ili2511_I2C_InitializeIfRequired(void);

/**
  * @brief  Basic static configuration of TouchScreen
  * @param  DeviceAddr: ILI2511 Device address for communication on I2C Bus.
  * @retval Status ILI2511_STATUS_OK or ILI2511_STATUS_NOT_OK.
  */
static uint32_t ili2511_TS_Configure(uint16_t DeviceAddr);
static uint8_t ili2511_firmware_version(uint16_t DeviceAddr);
//typedef struct ili2511_pos_t pt;
ili2511_pos_t pt;
float Xratio, Yratio;
/* Public functions bodies-----------------------------------------------*/


/******************************************************************************
  * @brief  Initialize the ILI2511 communication bus
  *         from MCU to ILI2511 : ie I2C channel initialization (if required).
  * @param  DeviceAddr: Device address on communication Bus 
                       (I2C slave address of ILI2511).
  * @retval None
  *****************************************************************************/
void ili2511_Init(uint16_t DeviceAddr,uint16_t MaxX,uint16_t MaxY)
{
  /* Wait at least 200ms after power up before accessing registers
   * Trsi timing (Time of starting to report point after resetting) from ILI2511GQQ datasheet */
 // TS_IO_Delay(200);
  
   ILI2511_MAX_WIDTH  = MaxX;
   ILI2511_MAX_HEIGHT = MaxY;
   Xratio = 1.0 * ILI2511_MAX_WIDTH /ILI2511_MAX_RESOLUTION_X;
   Yratio = 1.0 * ILI2511_MAX_HEIGHT /ILI2511_MAX_RESOLUTION_Y;
  /* Initialize I2C link if needed */
  ili2511_I2C_InitializeIfRequired();
}

/*******************************************************************************
  * @brief  Software Reset the ILI2511.
  *         @note : Not applicable to ILI2511.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of ILI2511).
  * @retval None
  ******************************************************************************/
void ili2511_Reset(uint16_t DeviceAddr)
{
  /* Do nothing */
  /* No software reset sequence available in ILI2511 IC */
}
/***********************************************************************************
  * @brief  Read the ILI2511 device ID, pre initialize I2C in case of need to be
  *         able to read the ILI2511 device ID, and verify this is a ILI2511.
  * @param  DeviceAddr: I2C ILI2511 Slave address.
  * @retval The Device ID (two bytes).
  **********************************************************************************/
uint16_t ili2511_ReadID(uint16_t DeviceAddr)
{
  volatile uint8_t ucReadId = 0;
  uint8_t nbReadAttempts = 0;
  uint8_t bFoundDevice = 0; /* Device not found by default */

  /* Initialize I2C link if needed */
  ili2511_I2C_InitializeIfRequired();

  /* At maximum 4 attempts to read ID : exit at first finding of the searched device ID */
  for(nbReadAttempts = 0; ((nbReadAttempts < 3) && !(bFoundDevice)); nbReadAttempts++)
  {
    /* Read register ILI2511_CHIP_ID_REG as DeviceID detection */
    ucReadId = TS_IO_Read(DeviceAddr, (uint8_t)ILI2511_CHIP_ID_REG);

    /* Found the searched device ID ? */
    //if(ucReadId == ILI2511_ID_VALUE)
    {
      /* Set device as found */
      bFoundDevice = 1;
    }
  }

  /* Return the device ID value */
  return (ucReadId);
}
/***********************************************************************************
  * @brief  Read the ILI2511 device firware version, pre initialize I2C in case of need to be
  *         able to read the ILI2511 device ID, and verify this is a ILI2511.
  * @param  DeviceAddr: I2C ILI2511 Slave address.
  * @retval The Device ID (two bytes).
  **********************************************************************************/
static uint8_t ili2511_firmware_version(uint16_t DeviceAddr)
{
	uint8_t fwbuf[8],protocol[3];
	TS_IO_Read_Buffer(DeviceAddr,0x40,fwbuf,8);		//read firmware version
	TS_IO_Read_Buffer(DeviceAddr,0x42,protocol,3);	//read protocol version
	return protocol[0];
}
/*******************************************************************************
  * @brief  Configures the touch Screen IC device to start detecting touches
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
  * @retval None.
  ****************************************************************************/
void ili2511_TS_Start(uint16_t DeviceAddr)
{
  /* Minimum static configuration of ILI2511 */
  ILI2511_ASSERT(ili2511_TS_Configure(DeviceAddr));

  /* By default set ILI2511 IC in Polling mode : no INT generation on ILI2511 for new touch available */
  /* Note TS_INT is active low                                                                      */
  ili2511_TS_DisableIT(DeviceAddr);
  pt.version = ili2511_firmware_version(DeviceAddr);
  if ( pt.version < 6 ) {
    pt.buf_len = 5;
    pt.version = 0;
  } 
  else {
    pt.buf_len = 64;
    pt.version = 1;
  }
}

//#pragma optimize = none

uint8_t ili2511_TS_DetectTouch(uint16_t DeviceAddr)
{
  
//  volatile uint8_t nbTouch = 0;

  static uint8_t nbTouch = 0;
  // First check if /INT line is LOW 
#if 1
//    TS_IO_Read_Buffer(DeviceAddr,0x10,ctp_rx_buffer,pt.buf_len); 
//  memcpy(&pt.xy.buf, &ctp_rx_buffer, 6);
  TS_IO_Read_Buffer(DeviceAddr,0x10,pt.xy.buf,pt.buf_len);
//  memcpy(&ctp_rx_buffer, &pt.xy.buf, 6);

  if ( pt.version ==0 ) {
//    if ( pt.xy.v3.td == 1 ) {
    if ( pt.xy.v3.t0 == 0x48 ) {
      nbTouch = 1;
    } else {
      nbTouch = 0;
    }
  } else {
    if ( pt.xy.v6.td == 0x48) {
      nbTouch = 1;
    } else {
      nbTouch = 0;
    }
  }
  return nbTouch;
#else
  TS_IO_Read_Buffer(DeviceAddr,0x10,ctp_rx_buffer,buf_size);
  if ((ctp_rx_buffer[NB_TOUCH_REG] & NB_TOUCH_MASK) == NB_TOUCH_DETECTED)
  //if (ctp_rx_buffer[0]==0x48)
  {
     nbTouch = 1;
  }
  else
  {
      nbTouch = 0;
  }

  if(nbTouch > ILI2511_MAX_DETECTABLE_TOUCH)
  {
    // If invalid number of touch detected, set it to zero
    nbTouch = 0;
  }

 // if((ILI2511_MAX_WIDTH==0)&&(ILI2511_MAX_HEIGHT==0))
 //   nbTouch = 0 ;
  
  // Update ILI2511 driver internal global : current number of active touches
  ili2511_handle.currActiveTouchNb = nbTouch;

  // Reset current active touch index on which to work on
  ili2511_handle.currActiveTouchIdx = 0;

  return(nbTouch);
#endif
}

/******************************************************************
  * @brief  Get the touch screen X and Y positions values
  *         Manage multi touch thanks to touch Index global
  *         variable 'ILI2511_handle.currActiveTouchIdx'.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  X: Pointer to X position value
  * @param  Y: Pointer to Y position value
  * @retval None.
  */
//#pragma optimize = none

void ili2511_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  //volatile uint8_t ucReadData = 0;
  static uint16_t coordX, coordY;
  static uint32_t val_low;
  static uint32_t val_high;
#if 1
  val_low = pt.xy.v3.xl;
  val_high = pt.xy.v3.xh;
  val_high  = (val_high << 8) + val_low;
//  val_high = (val_high * ILI2511_MAX_WIDTH)/ILI2511_MAX_RESOLUTION_X;
  val_high = (uint32_t)(val_high * Xratio);
// val_high = val_high >> 4;        //make sure the ratio of 
                        //ILI2511_MAX_RESOLUTION_X/ILI2511_MAX_WIDTH is 16(2^4)
  coordX = (uint16_t) val_high;
  *X = coordX;
  
  val_low = pt.xy.v3.yl;
  val_high = pt.xy.v3.yh;
  val_high  = (val_high << 8) + val_low;
//  val_high = (val_high * ILI2511_MAX_HEIGHT)/ILI2511_MAX_RESOLUTION_Y;
  val_high = (uint32_t)(val_high * Yratio);
//  val_high = val_high >> 4;        //make sure the ratio of 
                        //ILI2511_MAX_RESOLUTION_Y/ILI2511_MAX_HEIGHT is 16(2^4)
  coordY = (uint16_t) val_high;
  *Y = coordY;
#else
    /* Read low part of x position */
    val_low = ctp_rx_buffer[P1_XL_REG];
//  val_low = ctp_rx_buffer[2];
    /* Read high part of X position */
    val_high = ctp_rx_buffer[P1_XH_REG] & 0x03f;
//  val_high = ctp_rx_buffer[3] & 0x03f;
    val_high  = (val_high << 8) + val_low;
    val_high = (val_high * ILI2511_MAX_WIDTH)/ILI2511_MAX_RESOLUTION_X;
    coordX = (uint16_t) val_high;
      
    /* Send back ready X position to caller */
    *X = coordX;
    val_low = ctp_rx_buffer[P1_YL_REG];
    val_high = ctp_rx_buffer[P1_YH_REG] & 0x03f;
//    val_low= ctp_rx_buffer[4];
//    val_high = ctp_rx_buffer[5]&0x3f ;
    val_high = (val_high << 8 ) + val_low;

    val_high = (val_high * ILI2511_MAX_HEIGHT)/ILI2511_MAX_RESOLUTION_Y;

    // 1040=165.579/121.54*768
    coordY = val_high;
    
    /* Send back ready Y position to caller */
    *Y = coordY;

    ili2511_handle.currActiveTouchIdx++; 
#endif
   /* next call will work on next touch */

   /* of if(ILI2511_handle.currActiveTouchIdx < ILI2511_handle.currActiveTouchNb) */
}

/******************************************************************************
  * @brief  Configure the ILI2511 device to generate IT on given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus 
                       (Slave I2C address of ILI2511).
  * @retval None
  ******************************************************************************/
void ili2511_TS_EnableIT(uint16_t DeviceAddr)
{
   //uint8_t regValue = 0;
//   regValue = (ILI2511_G_MODE_INTERRUPT_TRIGGER & (ILI2511_G_MODE_INTERRUPT_MASK >> ILI2511_G_MODE_INTERRUPT_SHIFT)) << ILI2511_G_MODE_INTERRUPT_SHIFT;

   /* Set interrupt trigger mode in ILI2511_GMODE_REG */
 // Ilitek no need  TS_IO_Write(DeviceAddr, ILI2511_GMODE_REG, regValue);
}
/**************************************************************************************
  * @brief  Configure the ILI2511 device to stop generating IT on the given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of ILI2511).
  * @retval None
  *************************************************************************************/
void ili2511_TS_DisableIT(uint16_t DeviceAddr)
{
  //uint8_t regValue = 0;
  //regValue = (ILI2511_G_MODE_INTERRUPT_POLLING & (ILI2511_G_MODE_INTERRUPT_MASK >> ILI2511_G_MODE_INTERRUPT_SHIFT)) << ILI2511_G_MODE_INTERRUPT_SHIFT;

  /* Set interrupt polling mode in ILI2511_GMODE_REG */
  // IlitekTS_IO_Write(DeviceAddr, ILI2511_GMODE_REG, regValue);    
}
/****************************************************************************************
  * @brief  Get IT status from ILI2511 interrupt status registers
  *         Should be called Following an EXTI coming to the MCU to know the detailed
  *         reason of the interrupt.
  *         @note : This feature is not applicable to ILI2511.
  * @param  DeviceAddr: Device address on communication Bus
                       (I2C slave address of ILI2511).
  * @retval TS interrupts status : always return 0 here
  ***************************************************************************************/
uint8_t ili2511_TS_ITStatus(uint16_t DeviceAddr)
{
  /* Always return 0 as feature not applicable to ILI2511 */
  return 0;
}
/*****************************************************************************************
  * @brief  Clear IT status in ILI2511 interrupt status clear registers
  *         Should be called Following an EXTI coming to the MCU.
  *         @note : This feature is not applicable to ILI2511.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of ILI2511).
  * @retval None
  ****************************************************************************************/
void ili2511_TS_ClearIT(uint16_t DeviceAddr)
{
  /* Nothing to be done here for ILI2511 */
}

/* Static functions bodies-----------------------------------------------*/
/******************************************************************************
  * @brief  Return the status of I2C was initialized or not.
  * @param  None.
  * @retval : I2C initialization status.
  *****************************************************************************/
static uint8_t ili2511_Get_I2C_InitializedStatus(void)
{
  return(ili2511_handle.i2cInitialized);
}

/*******************************************************************************
  * @brief  I2C initialize if needed.
  * @param  None.
  * @retval : None.
  *****************************************************************************/
static void ili2511_I2C_InitializeIfRequired(void)
{
  if(ili2511_Get_I2C_InitializedStatus() == ILI2511_I2C_NOT_INITIALIZED)
  {
    /* Initialize TS IO BUS layer (I2C) */
    TS_IO_Init();

    /* Set state to initialized */
    ili2511_handle.i2cInitialized = ILI2511_I2C_INITIALIZED;
  }
}
/*******************************************************************************
  * @brief  Basic static configuration of TouchScreen
  * @param  DeviceAddr: ILI2511 Device address for communication on I2C Bus.
  * @retval Status ILI2511_STATUS_OK or ILI2511_STATUS_NOT_OK.
  ******************************************************************************/
static uint32_t ili2511_TS_Configure(uint16_t DeviceAddr)
{
  uint32_t status = ILI2511_STATUS_OK;

  /* Nothing special to be done for ILI2511 */

  return(status);
}

/************************ (C) COPYRIGHT EDT *****END OF FILE****/
