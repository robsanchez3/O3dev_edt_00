/**
  ******************************************************************************
  * @file    MXT640U.c
  * @author  EDT Embedded Application Team
  * @version V1.0.0
  * @date    22-May-2020
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
#include "main.h"
#include "mxt640u.h"
#include <math.h>

#if defined(MXT640U)

static uint8_t REPORT_MODE  ;

static  uint16_t MXT640U_MAX_WIDTH =0;
static  uint16_t MXT640U_MAX_HEIGHT =0;

TS_Muti_DrvTypeDef mxt640u_ts_drv =
{
  mxt640u_Init,
  mxt640u_ReadID,
  mxt640u_Reset,

  mxt640u_TS_Start,
  mxt640u_TS_DetectTouch,
  mxt640u_TS_GetXY,

  mxt640u_TS_EnableIT,
  mxt640u_TS_ClearIT,
  mxt640u_TS_ITStatus,
  mxt640u_TS_DisableIT,

  mxt640u_TS_Set2D3DMode,
  mxt640u_TS_Get2D3DMode,
};

/*  MXT640U General Function     */
static uint8_t mxt640u_Get_I2C_InitializedStatus(void);
static void mxt640u_I2C_InitializeIfRequired(void);
static int mxt_read_Point2D(struct mxt_data *data , struct mxt_touchReport *report);
static int mxt_read_Point3D(struct mxt_data *data , struct mxt_touchReport *report);
/*  MXT640U Driver Function     */

static int __mxt_read_reg(uint16_t DeviceAddr,uint16_t reg, uint16_t len, void *val);
static int mxt_read_reg(uint16_t DeviceAddr  , uint16_t reg, uint8_t *val);
static int mxt_write_reg(uint16_t DeviceAddr , uint16_t reg, uint8_t val);
//static int mxt_read_object_table(uint16_t DeviceAddr , uint16_t reg, uint8_t *object_buf);
//static struct mxt_object *mxt_get_object(struct mxt_data *data, uint8_t type);
//static int mxt_read_object(struct mxt_data *data,uint8_t type, uint8_t offset, uint8_t *val);
//static int mxt_write_object(struct mxt_data *data, uint8_t type, uint8_t offset, uint8_t val);
static int mxt_get_info(struct mxt_data *data);
static int mxt_get_object_table(struct mxt_data *data);
static int mxt_initialize(struct mxt_data *data);
static int mxt_calc_selfRawdata(struct mxt_touchReport *report,struct mxt_touchSelfRawData *rawdata ,struct mxt_touchMutualRawData *mrawdata);
//static int mxt_calc_selfRawdata(struct mxt_touchReport *report,struct mxt_touchSelfRawData *rawdata );
static void mxt_self_start(struct mxt_data *data);
static void mxt_self_stop(struct mxt_data *data);

//static void mxt_mutual_start(struct mxt_data *data);
//static void mxt_mutual_stop(struct mxt_data *data);

uint32_t EMPTY_ReportEvent =0;

/**************************************************************************/

static mxt640u_handle_TypeDef mxt640u_handle = { MXT640U_I2C_NOT_INITIALIZED, 0, 0};
static struct mxt_touchReport mtouch;
struct mxt_data mxt640u_data;
struct mxt_platform_data mxt640u_platform_data;

struct mxt_BackuptouchRawData mxt640u_backrawdata;
bool rawdataBackEn = false;
/******************************************************************************
  * @brief  Initialize the mxt640u communication bus
  *         from MCU to mxt640u : ie I2C channel initialization (if required).
  * @param  DeviceAddr: Device address on communication Bus 
                       (I2C slave address of mxt640u).
  * @retval None
  *****************************************************************************/
void mxt640u_Init(uint16_t DeviceAddr,uint16_t MaxX,uint16_t MaxY)
{
  /* Wait at least 200ms after power up before accessing registers
   * Trsi timing (Time of starting to report point after resetting) from ILI2511GQQ datasheet */
//  TS_IO_Delay(200);
  /* Initialize I2C link if needed */
  
  MXT640U_MAX_WIDTH   = MaxX;
  MXT640U_MAX_HEIGHT  = MaxY;
 REPORT_MODE = MXT640U_REPORT_2D_MODE;
 // REPORT_MODE = MXT640U_DEFAULT_2D3D_MODE;
  
  mxt640u_I2C_InitializeIfRequired();    
}
/*******************************************************************************
  * @brief  Software Reset the MXT640U.
  *         @note : Not applicable to MXT640U.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of MXT640U).
  * @retval None
  ******************************************************************************/
void mxt640u_TS_Set2D3DMode(uint8_t mode)
{
  switch(mode)
  {
  case MXT640U_REPORT_2D_MODE:
     REPORT_MODE = MXT640U_REPORT_2D_MODE;
    break;
  case MXT640U_REPORT_3D_MODE:
     REPORT_MODE = MXT640U_REPORT_3D_MODE;
    break;
  default:
    REPORT_MODE = MXT640U_REPORT_2D_MODE;
      break;
  }
}
/*******************************************************************************
  * @brief  Software Reset the MXT640U.
  *         @note : Not applicable to MXT640U.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of MXT640U).
  * @retval None
  ******************************************************************************/
   
uint8_t mxt640u_TS_Get2D3DMode(void)
{   
  return REPORT_MODE;
}
/*******************************************************************************
  * @brief  Software Reset the MXT640U.
  *         @note : Not applicable to MXT640U.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of MXT640U).
  * @retval None
  ******************************************************************************/
void mxt640u_Reset(uint16_t DeviceAddr)
{
  /* Do nothing */
  /* No software reset sequence available in MXT640U IC */
}
/***********************************************************************************
  * @brief  Read the MXT640U device ID, pre initialize I2C in case of need to be
  *         able to read the MXT640U device ID, and verify this is a MXT640U.
  * @param  DeviceAddr: I2C MXT640U Slave address.
  * @retval The Device ID (two bytes).
  **********************************************************************************/
uint16_t mxt640u_ReadID(uint16_t DeviceAddr)
{
  volatile uint8_t ucReadId = 0;

  return (ucReadId);
}
/*******************************************************************************
  * @brief  Configures the touch Screen IC device to start detecting touches
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
  * @retval None.
  ****************************************************************************/
void mxt640u_TS_Start(uint16_t DeviceAddr)
{
 
}

/*******************************************************************************
  * @brief  Configures the touch Screen IC device to start detecting touches
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
  * @retval None.
  ****************************************************************************/
uint8_t mxt640u_TS_DetectTouch(uint16_t DeviceAddr)
{
  uint8_t nbTouch = 0;
  uint16_t xtemp=0;
  uint16_t ytemp=0;
  
 if(mxt640u_handle.i2cInitialized ==  MXT640U_I2C_INITIALIZED)
 {
   if(REPORT_MODE == MXT640U_REPORT_2D_MODE)  //2D
   {
      mxt_read_Point2D(&mxt640u_data,&mtouch);
      if (mtouch.event!=0x30)
      {
        return(false);
      }

//      if((mtouch.flag==MXT_TOUCH_EVENT_CONTECT)||(mtouch.flag==MXT_TOUCH_EVENT_SILDER))
      if((mtouch.flag==0x90 )||(mtouch.flag==MXT_TOUCH_EVENT_CONTECT)||(mtouch.flag==MXT_TOUCH_EVENT_SILDER))
//if (mtouch.flag!=0x20)
      {      
        nbTouch= true;
      }
      else
        nbTouch=false;
   }
   else   //3D
   {
    mxt_read_Point3D(&mxt640u_data,&mtouch);
   if(mtouch.event==MXT_TOUCH_EVENT_FINGER1)
    {        
          xtemp = mtouch.xpoint;
          ytemp = mtouch.ypoint;
      
     if((mxt640u_data.cfg1&0x80)==0x80)
     {
        mtouch.xpoint = mxt640u_data.max_x - xtemp ;
     }
     if((mxt640u_data.cfg1&0x40)==0x40)
     {
        mtouch.ypoint = mxt640u_data.max_y - ytemp ;
     }
     if((mxt640u_data.cfg1&0x20)==0x20)
     {
       mtouch.xpoint = ytemp;
       mtouch.ypoint = xtemp;
     }
      nbTouch= true;    
    }
   else 
   {        
       nbTouch= false;    
   }  
   }
   
    if((MXT640U_MAX_WIDTH ==0)&&(MXT640U_MAX_HEIGHT ==0))
       nbTouch=false;
 }
  return(nbTouch);
}

/******************************************************************
  * @brief  Get the touch screen X and Y positions values
  *         Manage multi touch thanks to touch Index global
  *         variable 'ILI2511_handle.currActiveTouchIdx'.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  X: Pointer to X position value
  * @param  Y: Pointer to Y position value
  * @retval None.
 ******************************************************************/
void mxt640u_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  static uint16_t coordX, coordY;

  uint16_t lcdw,lcdh;
  float Xfloat = 0 , Yfloat = 0;  
  

      coordX = (uint16_t)  mtouch.xpoint;
      coordY = (uint16_t)  mtouch.ypoint;
     
      lcdw = EDT_LCD_GetXSize();//800
      lcdh = EDT_LCD_GetYSize();//480
      
      if(REPORT_MODE == MXT640U_REPORT_2D_MODE)
      {
      Xfloat = ((float)(lcdw-10)   / (float)(mxt640u_data.max_x )) ;  // 800/4096
      Yfloat = ((float)(lcdh-10) / (float)(mxt640u_data.max_y )) ;
      
      }
      else
      {
        Xfloat = (float)(lcdw) / (float)(mxt640u_data.max_x ) ; 
        Yfloat = (float)(lcdh) / (float)(mxt640u_data.max_y ) ;
      }
            
    /* Send back ready X position to caller */
    *X = (uint16_t) (coordX* Xfloat);
    /* Send back ready Y position to caller */
    *Y = (uint16_t) (coordY* Yfloat);
    
   // #if (CTP_DEBUG_RAW_LEVEL > 0)
   //   dev_err(NULL,"x:%04d , y:%04d \r\n ",&X,&Y);
   // #endif
    
    
}

/******************************************************************************
  * @brief  Configure the mxt640u device to generate IT on given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus 
                       (Slave I2C address of mxt640u).
  * @retval None
  ******************************************************************************/
void mxt640u_TS_EnableIT(uint16_t DeviceAddr)
{
  /* Nothing to be done here for mxt640u */
}
/*****************************************************************************************
  * @brief  Clear IT status in mxt640u interrupt status clear registers
  *         Should be called Following an EXTI coming to the MCU.
  *         @note : This feature is not applicable to mxt640u.
  * @param  DeviceAddr: Device address on communication Bus 
                        (I2C slave address of mxt640u).
  * @retval None
  ****************************************************************************************/
void mxt640u_TS_ClearIT(uint16_t DeviceAddr)
{
  /* Nothing to be done here for mxt640u */
}
/****************************************************************************************
  * @brief  Get IT status from mxt640u interrupt status registers
  *         Should be called Following an EXTI coming to the MCU to know the detailed
  *         reason of the interrupt.
  *         @note : This feature is not applicable to mxt640u.
  * @param  DeviceAddr: Device address on communication Bus
                       (I2C slave address of mxt640u).
  * @retval TS interrupts status : always return 0 here
  ***************************************************************************************/
uint8_t mxt640u_TS_ITStatus(uint16_t DeviceAddr)
{
  /* Nothing to be done here for mxt640u */
  return 0;
}
/**************************************************************************************
  * @brief  Configure the mxt640u device to stop generating IT on the given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of mxt640u).
  * @retval None
  *************************************************************************************/
void mxt640u_TS_DisableIT(uint16_t DeviceAddr)
{  
  /* Nothing to be done here for mxt640u */
}
/******************************************************************************
  * @brief  Return the status of I2C was initialized or not.
  * @param  None.
  * @retval : I2C initialization status.
  *****************************************************************************/
static uint8_t mxt640u_Get_I2C_InitializedStatus(void)
{
  return(mxt640u_handle.i2cInitialized);
}

/*******************************************************************************
  * @brief  I2C initialize if needed.
  * @param  None.
  * @retval : None.
  *****************************************************************************/
static void mxt640u_I2C_InitializeIfRequired(void)
{   
  
  if(mxt640u_Get_I2C_InitializedStatus() == MXT640U_I2C_NOT_INITIALIZED)
  {
    /* Initialize TS IO BUS layer (I2C) */
    TS_IO_Init();
    /* Set state to initialized */
    
    mxt640u_data.DeviceAddr       =  MXT640U_I2C_SLAVE_ADDRESS;
    
//    TS_IO_Delay(100);
    
    mxt_initialize(&mxt640u_data) ;
		
    if((mxt640u_data.info.family_id==MXT_FAMILYID)&&(mxt640u_data.info.variant_id==MXT_VARIANTID))
    {
     mxt640u_handle.i2cInitialized =  MXT640U_I2C_INITIALIZED;
    }
  }
}

/**************************************************************************************
  * @brief  __mxt_read_reg.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int __mxt_read_reg(uint16_t DeviceAddr,uint16_t reg, uint16_t len, void *val)
{	
        TS_IO_MXT640U_Read_Multi_Buf(DeviceAddr, reg , val , len);
	return 0;
}
/**************************************************************************************
  * @brief  mxt_read_reg.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_reg(uint16_t DeviceAddr  , uint16_t reg, uint8_t *val)
{
	return __mxt_read_reg(DeviceAddr  , reg , 1 , val);
}
/**************************************************************************************
  * @brief  mxt_read_reg.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_buf(uint16_t DeviceAddr  , uint16_t reg , uint8_t *val , uint16_t len)
{
	return __mxt_read_reg(DeviceAddr  , reg , len , val);
}
/**************************************************************************************
  * @brief  mxt_write_reg.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_write_reg(uint16_t DeviceAddr , uint16_t reg, uint8_t val)
{
	TS_IO_MXT640U_Write_Value(DeviceAddr, reg, val);
        return 0;
}
/**************************************************************************************
  * @brief  mxt_write_reg.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
//static void mxt_read_only(uint16_t DeviceAddr , uint8_t *val , uint16_t Size)
//{
//        TS_IO_MXT640U_Read_Only(DeviceAddr,   val , Size);
//}
/**************************************************************************************
  * @brief  mxt_read_object_table.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_object_table(uint16_t DeviceAddr , uint16_t reg, uint8_t *object_buf)
{
	return __mxt_read_reg(DeviceAddr, reg, MXT_OBJECT_SIZE , object_buf);
}
/**************************************************************************************
  * @brief  mxt_get_object.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static struct mxt_object *mxt_get_object(struct mxt_data *data, uint8_t type)
{
	struct mxt_object *object;
	int i;

	for (i = 0; i < data->info.object_num; i++) {
		object = data->object_table + i;
		if (object->type == type)
			return object;
	}
	return NULL;
}

/**************************************************************************************
  * @brief  mxt_read_Point2D.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_Point2D(struct mxt_data *data, struct mxt_touchReport *report)
{
      //uint8_t valut28h=0;     
#if defined MXT_GEN_MESSAGE_MODE
        struct mxt_object *object;
	uint16_t reg;           
        
  
            
	object = mxt_get_object(data, MXT_GEN_MESSAGE);
	if (!object) return -1;
	reg = object->start_address;  
        mxt_read_buf(data->DeviceAddr  , reg , (uint8_t*)report ,sizeof(struct mxt_touchReport));        
#else
//        mxt_read_object(data,MXT_TOUCH_MULTI, 28, &valut28h);//    
//        mxt_write_object(data,MXT_TOUCH_MULTI,28, 0x0b);
        
        mxt_write_reg(data->DeviceAddr , 0x8183, 0xa8);
//        mxt_write_reg(data->DeviceAddr , 0x0106, 0x);
        mxt_read_only(data->DeviceAddr,(uint8_t*)report ,sizeof(struct mxt_touchReport));
#endif 
        
//#if (CTP_DEBUG_RAW_LEVEL > 0)
//           dev_err(NULL,"%02x %02x %02x %02x %02x %02x %02x %02x %02x",report->event,
//                   report->flag,(uint8_t)(report->xpoint>>8),(uint8_t)(report->xpoint&0xff),
//                     (uint8_t)(report->ypoint>>8),(uint8_t)(report->ypoint&0xff),
//(uint8_t)(report->xpoint1>>8),(uint8_t)(report->xpoint1&0xff),(uint8_t)(report->ypoint1>>8),
//(uint8_t)(report->ypoint1&0xff));           
//           dev_err(NULL,"\r\n");
//#endif
        
	return -1;
}
/**************************************************************************************
  * @brief  mxt_read_message.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_Point3D(struct mxt_data *data, struct mxt_touchReport *report)
{
   struct mxt_object *object;
   uint16_t reg;  
   uint16_t i;
/*   
   uint8_t value=0;
   uint16_t j; 
   uint32_t rawMoreIdx=0;
  
   uint8_t  buffer[20];
   uint8_t  raw2dData[130];
*/ 
   struct mxt_touchSelfRawData   selfrawdata;    
   struct mxt_touchMutualRawData mutualrawdata;    
   
//   
//   mxt_write_object(data, MXT_GEN_COMMAND,MXT_COMMAND_RESET, 0x01) ;//RESET
//   HAL_Delay(2);
//   
//   mxt_write_object(data, MXT_GEN_COMMAND,MXT_COMMAND_REPORTALL, 0x01) ;//RESET     
//    HAL_Delay(2);
//    
//   object = mxt_get_object(data, MXT_GEN_COMMAND);  //Read T37    
//     if (!object) 
//     {    
//      return -1;
//     }  
//   
//  // mxt_write_object(data,MXT_TOUCH_MULTI,28, 0x00);
//    
//    
//   object = mxt_get_object(data, MXT_TOUCH_MULTI);       //T100
//     if (!object) 
//    return -1;           
//    
//   mxt_mutual_start(data);  //T6->MXT_COMMAND_DIAGNOSTIC ->MXT_MCDM
//      
//    
//   object = mxt_get_object(data, MXT_DEBUG_DIAGNOSTIC);  //Read T37    
//     if (!object) 
//     {
//       mxt_mutual_stop(data);
//       return -1;
//     }  
//     reg = object->start_address;         //0x0100    
//     
//    for(int i=0;i<((data->info.matrix_xsize*data->info.matrix_ysize *2)/128+1);i++)
//    {         
//      memset(&raw2dData,0,sizeof(raw2dData));
//      
//      mxt_read_buf(data->DeviceAddr  , reg      , &raw2dData[0] , 62 );      
//      mxt_read_buf(data->DeviceAddr  , reg + 0x3e , &raw2dData[62] , 62 );
//      mxt_read_buf(data->DeviceAddr  , reg+0x7c , &raw2dData[124] , 6 );    
//            
//      mutualrawdata.flag=(raw2dData[0]<<8)|raw2dData[1];
//      
//      for( j=0;j<64;j++)
//      {
//        mutualrawdata.raw[i*64+j]= (raw2dData[3+j*2]<<8)+raw2dData[2+j*2];
//      }
//      mxt_write_object(data, MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, MXT_PAGEUP);//Self Capacitance Delta Mode
//      mxt_read_object(data, MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, &value);//Self Capacitance Delta Mode      
//    }   
//      mxt_mutual_start(data);  //T6->MXT_COMMAND_DIAGNOSTIC ->MXT_MCDM
//     
//    for(int i=0;i<(data->info.matrix_xsize*data->info.matrix_ysize);i++)
//      {
//        if(mutualrawdata.raw[i]>32767)
//        {
//         mutualrawdata.raw[i] = abs(65535 - mutualrawdata.raw[i]);    
//        }
//      }     
//    
//    mutualrawdata.rawMax=0;
//    mutualrawdata.XrawMax=0xff;
//    mutualrawdata.YrawMax=0xff;
//    
//    for(int i=0;i<data->info.matrix_xsize;i++)
//    {  
//      for(int j=0;j<data->info.matrix_ysize;j++)
//      {   
//       //   printf("%05d ",mutualrawdata.raw[i*data->info.matrix_ysize+j]); 
//      
//          if(mutualrawdata.raw[i*data->info.matrix_ysize+j]>600)
//          {
//              mutualrawdata.rawMax = 0;
//              mutualrawdata.XrawMax =0;
//              mutualrawdata.YrawMax =0;
//              return -1;
//          }
//          else if((mutualrawdata.raw[i*data->info.matrix_ysize+j]>20)&&(mutualrawdata.raw[i*data->info.matrix_ysize+j]<=200)&&(j<data->matrix_y)&&(i<data->matrix_x))
//          {
//            if(mutualrawdata.raw[i*data->info.matrix_ysize+j]>mutualrawdata.rawMax)
//            {
//              mutualrawdata.rawMax = mutualrawdata.raw[i*data->info.matrix_ysize+j];
//              mutualrawdata.XrawMax =(i*data->info.matrix_ysize+j)/20+1;
//              mutualrawdata.YrawMax =((i*data->info.matrix_ysize+j)%20)+1;
//            }        
//          }
//         
//        }    
//      
//      //  printf("\r\n");
//    }    
//    
//    if(mutualrawdata.rawMax !=0 )
//    {
//      printf("X: %d Y: %d    RAW: %d ",mutualrawdata.XrawMax , mutualrawdata.YrawMax ,mutualrawdata.rawMax);    
//            printf("\r\n");
//    }
//  //   printf("\r\n");
//    
   
  
    object = mxt_get_object(data, MXT_GEN_POWER);  ///T7   
    if (!object) {mxt_self_stop(data);
    return -1;}
    
    object = mxt_get_object(data, MXT_SPT_SELFTEST);//T25
     if (!object) {mxt_self_stop(data);
    return -1;}
     
    mxt_self_start(data);                           //T6 ->F7
//    HAL_Delay(10);
   
    object = mxt_get_object(data, MXT_GEN_COMMAND);//T6
    if (!object) {mxt_self_stop(data);
    return -1;}
    
   object = mxt_get_object(data, MXT_DEBUG_DIAGNOSTIC);  //T37
    if (!object) {mxt_self_stop(data);
    return -1;}
        
    reg = object->start_address;         
    mxt_read_buf(data->DeviceAddr  , reg  , (uint8_t*)&selfrawdata ,sizeof(struct mxt_touchSelfRawData));
    
    for(i=0;i<51;i++)
    {
      if(selfrawdata.raw[i]>32767)
      {
        selfrawdata.raw[i] =abs(65535 - selfrawdata.raw[i]);
      }
    }
//     HAL_Delay(5);
    
     mxt_self_stop(data);

//     HAL_Delay(5);


    
#if (CTP_DEBUG_RAW_LEVEL > 0)
     dev_err(NULL,"==========================Max17  YChannel=========================\r\n");
    for(i=0;i<2;i++)
    {
      for(j=0;j<10;j++)
        dev_err(NULL,"Y%02d:%05d  ", i*10+j, selfrawdata.raw[i*10+j]);
        dev_err(NULL,"\r\n");
    }    
    dev_err(NULL,"===========================Max26 XChannel=========================\r\n");
     for(i=0;i<2;i++)
    {
      for(j=0;j<16;j++)
       dev_err(NULL,"X%02d:%05d  ", i*16+j, selfrawdata.raw[MXT640U_MAX_Y_Channel+ i*16+j]);
        dev_err(NULL,"\r\n");
    }    
    dev_err(NULL,"========================================================\r\n");
#endif


    if(mxt_calc_selfRawdata(report , &selfrawdata,&mutualrawdata)==0)       
    {
      report->event=MXT_TOUCH_EVENT_FINGER1;    
            
      selfrawdata.flag=0;
      for(i=0;i<51;i++)
       {
         selfrawdata.raw[i]=0;
       }
      return 0;
    }
    else      
    {
      mtouch.event=0xff;   
      selfrawdata.flag=0;      
      for(i=0;i<51;i++)
       {
         selfrawdata.raw[i]=0;
       }
      return -1;
    }    
}
/**************************************************************************************
  * @brief  mxt_read_message.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_calc_selfRawdata(struct mxt_touchReport *report,struct mxt_touchSelfRawData *rawdata ,struct mxt_touchMutualRawData *mrawdata)
{
  int idx;
  int error= -1;
  
  //int xMaxDataNum = mxt640u_data.info.matrix_xsize; 
  int yMaxDataNum = mxt640u_data.info.matrix_ysize;
  
  int xMatrixNum = mxt640u_data.matrix_x;
  int yMatrixNum = mxt640u_data.matrix_y;
   
  float xPMax=0,yPMax=0;
  int   xPMaxIndex=0,yPMaxIndex=0;
  
  float xPoint=0;
  float xPointSum=0;
  float yPoint=0;
  float yPointSum=0;

  for(idx = yMaxDataNum;  idx < yMaxDataNum+ xMatrixNum; idx++) //X max matrix
  {
    if(xPMax <= rawdata->raw[idx]) //shift Y Max matrix
    {
       xPMax =(float)(rawdata->raw[idx]);   //scan X Max value
       xPMaxIndex = idx-yMaxDataNum;
    }
  }

  for(idx=0;  idx < yMaxDataNum; idx++) //Y
  {
    if(yPMax <= rawdata->raw[idx]) 
    {
       yPMax =(float)(rawdata->raw[idx]);   
       yPMaxIndex = idx;
    }
  }      
 if(!((xPMax>MXT640U_XCONTECT_SELFRAWVALUE)&&(yPMax>MXT640U_YCONTECT_SELFRAWVALUE)))
    return error;
  
 if(xPMax>MXT640U_XCONTECT_SELFRAWVALUE)
  {
    
   if((xPMaxIndex + yMaxDataNum) ==   yMaxDataNum) //x0
   {
     xPoint=0;
     xPointSum=0;
     for(idx=0;idx<2;idx++)
     {          
       xPoint += (float)((float)(rawdata->raw[(xPMaxIndex + yMaxDataNum +idx)])*(float)((float)xPMaxIndex+(float)idx));
     }
     for(idx=0;idx<2;idx++)
     {      
       xPointSum += (float)((float)(rawdata->raw[(xPMaxIndex + yMaxDataNum + idx)]));
     }
     xPoint /=xPointSum;
     xPoint *=(float)(((float)mxt640u_data.max_x)/((float)xMatrixNum-1));
   }
   
  else if((xPMaxIndex + yMaxDataNum) ==   yMaxDataNum + 1 )//x1
   {
      xPoint=0;
     xPointSum=0;
     for(idx=0;idx<3;idx++)
     {
       xPoint += (float)((float)(rawdata->raw[(xPMaxIndex + yMaxDataNum + idx)])*(float)((float)xPMaxIndex+(float)idx));
     }
       xPoint += (float)((float)(rawdata->raw[((xPMaxIndex-1)+yMaxDataNum)])*(float)((float)(xPMaxIndex-1)));
                          
     for(idx=0;idx<3;idx++)
     {
       xPointSum += (float)((float)(rawdata->raw[(xPMaxIndex+yMaxDataNum+idx)]));
     }
       xPointSum += (float)((float)(rawdata->raw[((xPMaxIndex-1)+yMaxDataNum)]));
     
     xPoint /=xPointSum;
     xPoint *=(float)((float)mxt640u_data.max_x/((float)xMatrixNum-1));
   }
      
   
   else if(((xPMaxIndex+yMaxDataNum) >yMaxDataNum)&&      //Xmax
           ((xPMaxIndex+yMaxDataNum)==(yMaxDataNum+(xMatrixNum-1))))
   {
      xPoint=0;
     xPointSum=0;
     for(idx=0;idx<2;idx++)
     {       
       xPoint += (float)((float)(rawdata->raw[(xPMaxIndex+yMaxDataNum-idx)])*(float)((float)xPMaxIndex-(float)idx));
     }
     for(idx=0;idx<2;idx++)
     {      
       xPointSum += (float)((float)(rawdata->raw[(xPMaxIndex+yMaxDataNum-idx)]));
     }
     xPoint /=xPointSum;
     xPoint *=(float)((float)mxt640u_data.max_x/((float)xMatrixNum-1));
   }
   
   else if(((xPMaxIndex+yMaxDataNum) >yMaxDataNum)&&      //Xmax-1
           ((xPMaxIndex+yMaxDataNum)==(yMaxDataNum+(xMatrixNum-2))))
   {
      xPoint=0;
     xPointSum=0;
     for(idx=0;idx<3;idx++)
     {
       xPoint += (float)((float)(rawdata->raw[(xPMaxIndex+yMaxDataNum-idx)])*(float)((float)xPMaxIndex-(float)idx));
     }
      xPoint += (float)((float)(rawdata->raw[((xPMaxIndex+1)+yMaxDataNum)])*(float)((float)(xPMaxIndex+1)));
     
     for(idx=0;idx<3;idx++)
     {
       xPointSum += (float)((float)(rawdata->raw[(xPMaxIndex+yMaxDataNum-idx)]));
     }
       xPointSum += (float)((float)(rawdata->raw[((xPMaxIndex+1)+yMaxDataNum)]));
     xPoint /=xPointSum;
     xPoint *=(float)((float)mxt640u_data.max_x/((float)xMatrixNum-1));
   }
   
   else
   {
     xPoint=0;
     xPointSum=0;
     for(idx=0;idx<5;idx++)
     {
       xPoint += (float)((float)(rawdata->raw[(xPMaxIndex-2 + yMaxDataNum+idx)])*(float)((float)xPMaxIndex-2+idx));
     }
     for(idx=0;idx<5;idx++)
     {
       xPointSum += (float)((float)(rawdata->raw[(xPMaxIndex-2 + yMaxDataNum + idx)]));
     }     
     xPoint /=xPointSum;
     xPoint *=(float)((float)mxt640u_data.max_x/((float)xMatrixNum-1));
   }   
   report->xpoint=(int)xPoint;
   error = 0;    
  }
  else 
    error = -1;    
  
   if(yPMax>MXT640U_YCONTECT_SELFRAWVALUE)
  {
    yPoint    = 0;
    yPointSum = 0;
   if((yPMaxIndex) == 0)//x0
   {
     for(idx=0;idx<2;idx++)
     {     
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex+idx)])*(float)((float)yPMaxIndex+(float)idx));
     }
     for(idx=0;idx<2;idx++)
     {      
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex+idx)]));
     }
     yPoint /=yPointSum;
     yPoint *=(float)((float)mxt640u_data.max_y/((float)yMatrixNum-1));
   }
   
  else if((yPMaxIndex) ==   1 )//x1
   {
    yPoint    = 0;
    yPointSum = 0;
     for(idx=0;idx<3;idx++)
     {
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex+idx)])*(float)((float)yPMaxIndex+(float)idx));
     }
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex-1)])*(float)((float)(yPMaxIndex-1)));
                          
     for(idx=0;idx<3;idx++)
     {
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex+idx)]));
     }
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex-1)]));
     
     yPoint /=yPointSum;
     yPoint *=(float)((float)mxt640u_data.max_y/((float)yMatrixNum-1));
   }
      
   
   else if(((yPMaxIndex) < yMaxDataNum)&&      //Ymax
           ((yPMaxIndex)==((yMatrixNum-1))))
   {
     yPoint    = 0;
     yPointSum = 0;
     for(idx=0;idx<3;idx++)
     { 
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex-idx)])*(float)((float)yPMaxIndex-(float)idx));
     }
     for(idx=0;idx<3;idx++)
     {           
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex-idx)]));
     }
     yPoint /=yPointSum;
     yPoint *=(float)((float)mxt640u_data.max_y/((float)yMatrixNum-1));
   }
   
   else if(((yPMaxIndex) <yMaxDataNum)&&      //Ymax-1
           ((yPMaxIndex)==((yMatrixNum-2))))
   {
     yPoint    = 0;
    yPointSum = 0;
     for(idx=0;idx<2;idx++)
     {      
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex-idx)])*(float)((float)yPMaxIndex-(float)idx));
     }
      yPoint += (float)((float)(rawdata->raw[(yPMaxIndex+1)])*(float)((float)(yPMaxIndex+1)));
     
     for(idx=0;idx<2;idx++)
     {     
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex-idx)]));
     }
      yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex+1)]));
      yPoint /=yPointSum;
      yPoint *=(float)((float)mxt640u_data.max_y/((float)yMatrixNum-1));
   }   
   else
   {
    yPoint    = 0;
    yPointSum = 0;
     for(idx=0;idx<5;idx++)
     {
       yPoint += (float)((float)(rawdata->raw[(yPMaxIndex-2 +idx)])*(float)((float)yPMaxIndex-2+idx));
     }
     for(idx=0;idx<5;idx++)
     {
       yPointSum += (float)((float)(rawdata->raw[(yPMaxIndex-2  + idx)]));
     }     
     yPoint /=yPointSum;
     yPoint *=(float)((float)mxt640u_data.max_y/((float)yMatrixNum-1));
   }
   
   report->ypoint=(int)yPoint;        
   error = 0;    
  }
  else 
   error = -1; 
  
  return error;
 }
/**************************************************************************************
  * @brief  mxt_read_object.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_read_object(struct mxt_data *data,
				uint8_t type, uint8_t offset, uint8_t *val)
{
	struct mxt_object *object;
	uint16_t reg;

	object = mxt_get_object(data, type);
	if (!object)
		return -1;

	reg = object->start_address;
	return __mxt_read_reg(data->DeviceAddr, reg + offset, 1, val);
}
/**************************************************************************************
  * @brief  mxt_write_object.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_write_object(struct mxt_data *data,
				 uint8_t type, uint8_t offset, uint8_t val)
{
	struct mxt_object *object;
	uint16_t reg;

	object = mxt_get_object(data, type);
	if (!object)
		return -1;

	reg = object->start_address;
	return mxt_write_reg(data->DeviceAddr, reg + offset, val);
}

/**************************************************************************************
  * @brief  mxt_get_info.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_get_info(struct mxt_data *data)
{
	struct mxt_info *info = &data->info;
	int error;
	uint8_t val;

	error = mxt_read_reg(data->DeviceAddr, MXT_FAMILY_ID, &val);
	if (error)
		return error;
	info->family_id = val;
        
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "Family ID 0x%2x", info->family_id );
#endif

	error = mxt_read_reg(data->DeviceAddr, MXT_VARIANT_ID, &val);
	if (error)
		return error;
	info->variant_id = val;
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "Variant ID : 0x%2x", info->variant_id );
#endif
	error = mxt_read_reg(data->DeviceAddr, MXT_VERSION, &val);
	if (error)
		return error;
	info->version = val;
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "Version ID : 0x%2x", info->version );
#endif
	error = mxt_read_reg(data->DeviceAddr, MXT_BUILD, &val);
	if (error)
		return error;
	info->build = val;
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "Build : 0x%2x", info->build );
#endif
        error = mxt_read_reg(data->DeviceAddr, MXT_MATRIX_X_SIZE, &val);
	if (error)
		return error;
	info->matrix_xsize = val;
#if (CTP_DEBUG_LEVEL > 0)
         dev_err(NULL, "MATRIX_X_SIZE : 0x%2x", info->matrix_xsize );        
#endif   
        error = mxt_read_reg(data->DeviceAddr, MXT_MATRIX_Y_SIZE, &val);
	if (error)
		return error;
	info->matrix_ysize = val;
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "MATRIX_Y_SIZE : 0x%2x", info->matrix_ysize );        
#endif
	error = mxt_read_reg(data->DeviceAddr, MXT_OBJECT_NUM, &val);
	if (error)
		return error;
	info->object_num = val;
#if (CTP_DEBUG_LEVEL > 0)
        dev_err(NULL, "Number of Elements in the Object Table : 0x%2x", info->object_num );        
#endif
	return 0;
}
/**************************************************************************************
  * @brief  mxt_get_info.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_get_object_table(struct mxt_data *data)
{
	int error;
	int i;
	uint16_t reg;	
	uint8_t buf[MXT_OBJECT_SIZE];

	for (i = 0; i < data->info.object_num; i++) {
		struct mxt_object *object = data->object_table + i;

		reg = MXT_OBJECT_START + MXT_OBJECT_SIZE * i;
		error = mxt_read_object_table(data->DeviceAddr, reg, buf);
		if (error)
			return error;

		object->type = buf[0];
		object->start_address = (buf[2] << 8) | buf[1];
		object->size = buf[3];
		object->instances = buf[4];
		object->num_report_ids = buf[5];
#if (CTP_DEBUG_LEVEL > 0)
           
	       dev_err(dev, "type: T%03d --  start_address: %04x  --  size: %03d,-- instances : %02d , num_report_ids : %02d",
			object->type,object->start_address, object->size ,object->instances , object->num_report_ids);
#endif
		}	
	return 0;
}
/**************************************************************************************
  * @brief  mxt_initialize.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static int mxt_initialize(struct mxt_data *data)
{	
	struct mxt_info *info = &data->info;
	int error;
	uint8_t val;
        
        
        data->pdata = &mxt640u_platform_data;
                    
	error = mxt_get_info(data);
	if (error)
		return error;

         data->object_table = calloc(info->object_num,sizeof(struct mxt_object));
	if (!data->object_table) {
#if (CTP_DEBUG_LEVEL > 0)
		dev_err(&client->dev, "Failed to allocate memory\n");
#endif
		return -1;
	}        
      
	/* Get object table information */
	error = mxt_get_object_table(data);
	if (error)
		return error;

	/* Update matrix size at info struct */
	error = mxt_read_reg(data->DeviceAddr, MXT_MATRIX_X_SIZE, &val);
        
	if (error)
		return error;
        
	info->matrix_xsize = val;

	error = mxt_read_reg(data->DeviceAddr, MXT_MATRIX_Y_SIZE, &val);
	if (error)
		return error;
	info->matrix_ysize = val;
        
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_XRANGE_L , &val);
        data->max_x = (val&0XFF);
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_XRANGE_H , &val);
        data->max_x |= ((val<<8)&0XFF00);
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_YRANGE_L , &val);
        data->max_y = (val&0XFF);
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_YRANGE_H , &val);
        data->max_y |= ((val<<8)&0XFF00);
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_CFG1 , &val);
        data->cfg1 |= (val&0XFF);        
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_XSIZE , &val);
        data->matrix_x |= (val&0XFF);
        
        mxt_read_object(data , MXT_TOUCH_MULTI, MXT_TOUCH_YSIZE , &val);
        data->matrix_y |= (val&0XFF);
        
        mxt_write_object(data, MXT_GEN_COMMAND,MXT_COMMAND_RESET, 0x01) ;//RESET     
//        HAL_Delay(30);
        

#define MTX_LENS_BEND   65

// Disable Lens bending T65 ID15,16,17 ?
        mxt_write_object(data, MTX_LENS_BEND,0  , 0x00) ;//1st.
        mxt_write_object(data, MTX_LENS_BEND,23  , 0x00) ;//1st.
        mxt_write_object(data, MTX_LENS_BEND,46  , 0x00) ;//1st.

        mxt_write_object(data, MXT_TOUCH_MULTI,MXT_TOUCH_NUMTCH  , 0x01); // Reduce to 1 finger...
;;;;;;;;;;;;;


#if (CTP_DEBUG_LEVEL > 0)
	dev_err(dev, "Family ID: %d Variant ID: %d Version: %d Build: %d\n",
			info->family_id, info->variant_id, info->version,
			info->build);

	dev_err(dev, "Matrix X Size: %d Matrix Y Size: %d Object Num: %d  CFG1: %02x  Matrix X:%d  Matrix y:%d \n",
			info->matrix_xsize, info->matrix_ysize,
			info->object_num ,data->cfg1,data->matrix_x,data->matrix_y);
        
        dev_err(dev, "X Resolution: %d   Y Resolution: : %d ",data->max_x,data->max_y);			
#endif
	return 0;
}
/**************************************************************************************
  * @brief  mxt_get_info.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static void mxt_self_start(struct mxt_data *data)
{
 
  uint8_t buffer[10];
  uint16_t reg;
  struct mxt_object *object;
 
  
	mxt_write_object(data,MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, MXT_SCDM);//Self Capacitance Delta Mode        
//        HAL_Delay(2);
        object = mxt_get_object(data, MXT_GEN_COMMAND);  ///T6   
        reg = object->start_address;          
        mxt_read_buf(data->DeviceAddr  , reg , (uint8_t*)buffer ,7);          
       // mxt_read_object(data,MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC,&value);
}
/**************************************************************************************
  * @brief  mxt_get_info.
  * @param  DeviceAddr: Device address on communication Bus 
            (Slave I2C address of MXT640U).
  * @retval None
  *************************************************************************************/
static void mxt_self_stop(struct mxt_data *data)
{
	 uint8_t buffer[10];
	  uint16_t reg;
	  struct mxt_object *object;

	mxt_write_object(data,MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, MXT_PAGEUP);//Self Capacitance Delta Mode

    object = mxt_get_object(data, MXT_GEN_COMMAND);  ///T6
    reg = object->start_address;
    mxt_read_buf(data->DeviceAddr  , reg , (uint8_t*)buffer ,7);
}
//             
///**************************************************************************************
//  * @brief  mxt_get_info.
//  * @param  DeviceAddr: Device address on communication Bus 
//            (Slave I2C address of MXT640U).
//  * @retval None
//  *************************************************************************************/
//static void mxt_mutual_start(struct mxt_data *data)
//{
//   uint8_t buffer[10];
//  uint16_t reg;
//  struct mxt_object *object;
//	mxt_write_object(data,MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, MXT_MCDM);//Self Capacitance Delta Mode
//        
//         object = mxt_get_object(data, MXT_GEN_COMMAND);  ///T6   
//        reg = object->start_address;          
//        mxt_read_buf(data->DeviceAddr  , reg , (uint8_t*)buffer ,7);                
//}
///**************************************************************************************
//  * @brief  mxt_get_info.
//  * @param  DeviceAddr: Device address on communication Bus 
//            (Slave I2C address of MXT640U).
//  * @retval None
//  *************************************************************************************/
//static void mxt_mutual_stop(struct mxt_data *data)
//{
//	mxt_write_object(data,MXT_GEN_COMMAND, MXT_COMMAND_DIAGNOSTIC, MXT_PAGEUP);//Self Capacitance Delta Mode
//}
     
#endif  

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT EDT *****END OF FILE****/
