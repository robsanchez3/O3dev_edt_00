/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : USB_Host/App/usb_host.c
  * @brief           : USB Host application layer - STM32U5 adaptation
  *
  * Adapted from OztDUI_UsbFlash (STM32F7) reference project.
  * Key changes vs reference:
  *   - HOST_FS  -> HOST_HS  (USB_OTG_HS peripheral)
  *   - hUsbHostFS -> hUsbHostHS
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "fatfs.h"
#include <stdio.h>

/* USER CODE BEGIN Includes */
#define NO_USB_HOST_DEBUG
#ifdef USB_HOST_DEBUG
#define USB_LOG(...)  printf(__VA_ARGS__)
#else
#define USB_LOG(...)  do {} while(0)
#endif
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
USBH_HandleTypeDef  hUsbHostHS;
ApplicationTypeDef  Appli_state = APPLICATION_IDLE;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Initialize USB Host library, register MSC class and start
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  USB_LOG("[USB] USBH_Init...\n");
  if (USBH_Init(&hUsbHostHS, USBH_UserProcess, HOST_HS) != USBH_OK)
  {
    printf("[USB] USBH_Init FAILED\n");
    Error_Handler();
  }
  USB_LOG("[USB] USBH_RegisterClass...\n");
  if (USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS) != USBH_OK)
  {
    printf("[USB] USBH_RegisterClass FAILED\n");
    Error_Handler();
  }
  USB_LOG("[USB] USBH_Start...\n");
  if (USBH_Start(&hUsbHostHS) != USBH_OK)
  {
    printf("[USB] USBH_Start FAILED\n");
    Error_Handler();
  }
  USB_LOG("[USB] Init complete\n");

  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/**
  * @brief  Returns 1 if a USB flash drive is connected and MSC class is active.
  * @retval 1 if ready, 0 otherwise
  */
uint8_t USBH_IsFlashReady(void)
{
  return (Appli_state == APPLICATION_READY) ? 1 : 0;
}

/**
  * @brief  USB Host background task - call periodically from FreeRTOS task
  * @retval None
  */
void MX_USB_HOST_Process(void)
{
  USBH_Process(&hUsbHostHS);
}

/**
  * @brief  User callback - notified on USB Host state changes
  * @param  phost: Host handle
  * @param  id:    Event ID (HOST_USER_*)
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch (id)
  {
    case HOST_USER_SELECT_CONFIGURATION:
      USB_LOG("[USB] HOST_USER_SELECT_CONFIGURATION\n");
      break;

    case HOST_USER_CONNECTION:
      USB_LOG("[USB] HOST_USER_CONNECTION\n");
      Appli_state = APPLICATION_START;
      break;

    case HOST_USER_CLASS_ACTIVE:
      USB_LOG("[USB] HOST_USER_CLASS_ACTIVE -> READY\n");
      f_mount(&USBHFatFS, (TCHAR const*)USBHPath, 0);
      Appli_state = APPLICATION_READY;
      break;

    case HOST_USER_DISCONNECTION:
      USB_LOG("[USB] HOST_USER_DISCONNECTION\n");
      f_mount(NULL, (TCHAR const*)USBHPath, 0);
      Appli_state = APPLICATION_DISCONNECT;
      break;

    default:
      break;
  }
  /* USER CODE END CALL_BACK_1 */
}
