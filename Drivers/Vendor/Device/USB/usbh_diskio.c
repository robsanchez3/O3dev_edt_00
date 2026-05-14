/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FATFS/Target/usbh_diskio.c
  * @brief   USB Host Disk I/O driver for FatFS - STM32U5 adaptation
  *
  * Adapted from OztDUI_UsbFlash reference project.
  * Key change: hUSB_Host -> hUsbHostHS (USB_OTG_HS handle in usb_host.c)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"
#include "usbh_diskio.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* Private defines -----------------------------------------------------------*/
#define USB_DEFAULT_BLOCK_SIZE  512U

/* Private variables ---------------------------------------------------------*/
/* hUsbHostHS is declared in USB_Host/App/usb_host.c */
extern USBH_HandleTypeDef  hUsbHostHS;

/* Private function prototypes -----------------------------------------------*/
DSTATUS USBH_initialize(BYTE lun);
DSTATUS USBH_status(BYTE lun);
DRESULT USBH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
DRESULT USBH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
DRESULT USBH_ioctl(BYTE lun, BYTE cmd, void *buff);

/* FatFS driver interface registration */
const Diskio_drvTypeDef USBH_Driver =
{
  USBH_initialize,
  USBH_status,
  USBH_read,
  USBH_write,
  USBH_ioctl,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize USB drive
  */
DSTATUS USBH_initialize(BYTE lun)
{
  /* USB Host library must be initialized in the application before FatFS mount */
  return RES_OK;
}

/**
  * @brief  Get USB drive status
  */
DSTATUS USBH_status(BYTE lun)
{
  if (USBH_MSC_UnitIsReady(&hUsbHostHS, lun))
    return RES_OK;
  else
    return RES_ERROR;
}

/**
  * @brief  Read sector(s)
  */
DRESULT USBH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;

  if (USBH_MSC_Read(&hUsbHostHS, lun, sector, buff, count) == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info);
    switch (info.sense.asc)
    {
      case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
      case SCSI_ASC_MEDIUM_NOT_PRESENT:
      case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
        USBH_ErrLog("USB Disk is not ready!");
        res = RES_NOTRDY;
        break;
      default:
        res = RES_ERROR;
        break;
    }
  }
  return res;
}

/**
  * @brief  Write sector(s)
  */
DRESULT USBH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;

  if (USBH_MSC_Write(&hUsbHostHS, lun, sector, (BYTE *)buff, count) == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info);
    switch (info.sense.asc)
    {
      case SCSI_ASC_WRITE_PROTECTED:
        USBH_ErrLog("USB Disk is Write protected!");
        res = RES_WRPRT;
        break;
      case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
      case SCSI_ASC_MEDIUM_NOT_PRESENT:
      case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
        USBH_ErrLog("USB Disk is not ready!");
        res = RES_NOTRDY;
        break;
      default:
        res = RES_ERROR;
        break;
    }
  }
  return res;
}

/**
  * @brief  IOCTL - get drive info
  */
DRESULT USBH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;

  switch (cmd)
  {
    case CTRL_SYNC:
      /* SCSI SYNCHRONIZE CACHE (10): flush drive's write cache to NAND.
         Drives that don't support it return ILLEGAL REQUEST — acceptable,
         we return RES_OK regardless so FatFS continues normally.          */
      if ((hUsbHostHS.gState == HOST_CLASS) &&
          (hUsbHostHS.device.PortEnabled != 0U))
      {
        MSC_HandleTypeDef *msc = (MSC_HandleTypeDef *) hUsbHostHS.pActiveClass->pData;
        if (msc->unit[lun].state == MSC_IDLE)
        {
          (void)USBH_memset(msc->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
          msc->hbot.cbw.field.DataTransferLength = 0;
          msc->hbot.cbw.field.Flags              = USB_EP_DIR_OUT;
          msc->hbot.cbw.field.CBLength           = CBW_LENGTH;
          msc->hbot.cbw.field.CB[0]              = 0x35U; /* SYNCHRONIZE CACHE (10) */
          msc->hbot.state                        = BOT_SEND_CBW;

          uint32_t t0 = hUsbHostHS.Timer;
          while (USBH_MSC_BOT_Process(&hUsbHostHS, lun) == USBH_BUSY)
          {
            if ((hUsbHostHS.Timer - t0) > 3000U) break;
          }
        }
      }
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT:
      if (USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info) == USBH_OK)
      {
        *(DWORD *)buff = info.capacity.block_nbr;
        res = RES_OK;
      }
      break;

    case GET_SECTOR_SIZE:
      if (USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info) == USBH_OK)
      {
        *(DWORD *)buff = info.capacity.block_size;
        res = RES_OK;
      }
      break;

    case GET_BLOCK_SIZE:
      if (USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info) == USBH_OK)
      {
        *(DWORD *)buff = info.capacity.block_size / USB_DEFAULT_BLOCK_SIZE;
        res = RES_OK;
      }
      break;

    default:
      res = RES_PARERR;
      break;
  }
  return res;
}
