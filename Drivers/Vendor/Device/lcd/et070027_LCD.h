/**
  ******************************************************************************
  * @file    et070027.h
  * @author  EDT Embedded Application Team
  * @version V1.0.0
  * @date    19-Sep-2025
  * @brief   This file contains all the constants parameters for the et057027
  *          LCD component.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2025 EDT</center></h2>
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
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/  
  /** 
  * @brief  et070027 Size
  */
    #define  TFT_WIDTH            ((uint16_t)800)           /* LCD PIXEL WIDTH            */
    #define  TFT_HEIGHT           ((uint16_t)480)           /* LCD PIXEL HEIGHT           */
  /*   Time  Typ.
    DCLK                             2.5    Mhz
    Horizontal display area(thd)     800    DCLK
    HSYNC period time(th)            860    DCLK
    HSYNC blanking (thb+thfp)        57     DCLK

    Vertical display area(tvd)       480     DCLK
    VSYNC period time(tv)            530     DCLK
    VSYNC blanking (tvb+tvfp)        47      DCLK
  */
/** 
  * @brief  et070027 Timing
  */    
/** et070027 HSYNC Timing **/
    #define  TFT_HSYNC            ((uint16_t)3)  /* Horizontal synchronization */
    #define  TFT_HBP              ((uint16_t)13)   /* Horizontal back porch      */
    #define  TFT_HFP              ((uint16_t)44)   /* Horizontal front porch     */
/** et070027 VSYNC Timing **/
    #define  TFT_VSYNC            ((uint16_t)2)    /* Vertical synchronization   */
    #define  TFT_VBP              ((uint16_t)3)   /* Vertical back porch        */
    #define  TFT_VFP              ((uint16_t)44)   /* Vertical front porch       */
  

/**
*    STM32  LCD clock configuration
*    DCLK       = 27.22MHZ / TFT_PLL3M * TFT_PLL3N / TFT_PLL3R
*ex: DCLK       = 27.22MHz  / 5 * 49 / 9 =27.22Mhz
*    FLM        = DCLK / ((TFT_WIDTH+TFT_HSYNC+TFT_HBP+TFT_HFP) * ( TFT_HEIGHT+ TFT_VSYNC+ TFT_VBP+TFT_VFP))
*ex: FLM        = 27.22Mz / (860) * (530))  = 59.72Hz(16.74ms)
**/ 
    #define  TFT_PLL3M           5
    #define  TFT_PLL3N           49
    #define  TFT_PLL3P           2
    #define  TFT_PLL3Q           5   
    #define  TFT_PLL3R           9

   
/**LCD Polarity**/
    #define  HS_POLARITY    LTDC_HSPOLARITY_AL      /* horizontal synchronization polarity */
    #define  VS_POLARITY    LTDC_VSPOLARITY_AL      /* vertical synchronization polarity */
    #define  DE_POLARITY    LTDC_DEPOLARITY_AL      /* not data enable polarity */
    #define  PC_POLARITY    LTDC_PCPOLARITY_IIPC     /* pixel clock polarity */

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */

/************************ (C) COPYRIGHT EDT *****END OF FILE****/
