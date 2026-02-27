/**
 ******************************************************************************
 * @file    storage_app.c
 * @author  MCD Application Team
 * @brief   This file provides the kernel storage functions
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "storage_app.h"

//#define DECODE_MEDIA_DISCONNECTED_MESSAGE 0xFF07

#ifdef __GNUC__
#define PRAGMA_STORAGE_LOCATION
#define ATTRIBUTE_STORAGE_LOCATION __attribute__ ((section (".storageram"))) __attribute__ ((aligned(4)))
#elif defined __ICCARM__
#define PRAGMA_STORAGE_LOCATION _Pragma("location=\"STORAGERAM\"")
#define ATTRIBUTE_STORAGE_LOCATION
#elif defined(__ARMCC_VERSION)
#define PRAGMA_STORAGE_LOCATION
#define ATTRIBUTE_STORAGE_LOCATION __attribute__ ((section ("STORAGERAM"))) __attribute__ ((aligned(4)))
#endif


PRAGMA_STORAGE_LOCATION FATFS MSDDISK_FatFs ATTRIBUTE_STORAGE_LOCATION;         /* File system object for SD card logical drive */
char MSDDISK_Drive[4];       /* USB Host logical drive number */
//FIL MyFile;     /* File object */

static void SD_Initialize(void);

#if (osCMSIS < 0x20000U)
static osMessageQId StorageEvent;
#else
static osMessageQueueId_t StorageEvent;

osThreadId_t StorageTaskHandle;
const osThreadAttr_t StorageTask_attributes = {
  .name = "StorageTask",
  .stack_size = 1024 * 2,
  .priority = (osPriority_t) osPriorityNormal,
};
#endif

static uint8_t isInitialized = 0;
//static __IO uint8_t statusChanged = 0;
//uint8_t workBuffer[2 * _MAX_SS];
//ALIGN_32BYTES(uint8_t rtext[64]);

static uint32_t StorageStatus[NUM_DISK_UNITS];
static void StorageThread(void const * argument);

/**
 * @brief  Storage drives initialization
 * @param  None
 * @retval None
 */
void STORAGE_Init(void)
{
	/* Link the SD Host disk I/O driver */
	if(FATFS_LinkDriver(&SD_Driver, MSDDISK_Drive)==0)
	{
		SD_Initialize();

		/* Create SD background task */
#if (osCMSIS < 0x20000U)
		osThreadDef(STORAGE_Thread, StorageThread, osPriorityNormal, 0, 2048);
		osThreadCreate (osThread(STORAGE_Thread), NULL);
#else
		StorageTaskHandle = osThreadNew(StorageThread, NULL, &StorageTask_attributes);
#endif

		/* Create Storage Message Queue */
#if (osCMSIS < 0x20000U)
		osMessageQDef(osqueue, 10, uint16_t);
		StorageEvent = osMessageCreate (osMessageQ(osqueue), NULL);
		vQueueAddToRegistry(StorageEvent, "Storage_Queue");
#else
		StorageEvent = osMessageQueueNew(10, sizeof(uint16_t), NULL);
#endif

		if(EDT_SD_IsDetected())
		{
#if (osCMSIS < 0x20000U)
			osMessagePut ( StorageEvent, MSDDISK_CONNECTION_EVENT, 1);
#else
		    uint16_t msg_box[1] = {MSDDISK_CONNECTION_EVENT};
		    osMessageQueuePut (StorageEvent, msg_box, osPriorityNormal, 1);
#endif
		}
		else
		{
#if (osCMSIS < 0x20000U)
			osMessagePut ( StorageEvent, MSDDISK_DISCONNECTION_EVENT, 1);
#else
		    uint16_t msg_box[1] = {MSDDISK_DISCONNECTION_EVENT};
		    osMessageQueuePut (StorageEvent, msg_box, osPriorityNormal, 1);
#endif
		}
	}
}

/**
 * @brief  Storage get status
 * @param  unit: logical storage unit index.
 * @retval int
 */
uint8_t STORAGE_GetStatus (uint8_t unit)
{  
	return StorageStatus[unit];
}

/**
 * @brief  Storage get capacity
 * @param  unit: logical storage unit index.
 * @retval int
 */
uint32_t STORAGE_GetCapacity (uint8_t unit)
{  
	uint32_t   tot_sect = 0;
	FATFS *fs;

	fs = &MSDDISK_FatFs;

	tot_sect = (fs->n_fatent - 2) * fs->csize;
	return (tot_sect);
}

/**
 * @brief  Storage get free space
 * @param  unit: logical storage unit index.
 * @retval int
 */
uint32_t STORAGE_GetFree (uint8_t unit)
{ 
	uint32_t   fre_clust = 0;
	FATFS *fs ;
	FRESULT res = FR_INT_ERR;

	fs = &MSDDISK_FatFs;
	res = f_getfree("1:", (DWORD *)&fre_clust, &fs);

	if(res == FR_OK)
	{
		return (fre_clust * fs->csize);
	}
	else
	{
		return 0;
	}
}

/**
 * @brief  Add entire folder to play list.
 * @param  Foldername: pointer to folder name.
 * @retval None
 */
void STORAGE_AddEntireFolder(char *Foldername, FILELIST_FileTypeDef *list, char *ext)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	char *fn;
	static char tmp[FILEMGR_FILE_NAME_SIZE];

	fno.fsize = _MAX_LFN;

	res = f_opendir(&dir, Foldername);

	if (res == FR_OK)
	{

		while (1)
		{
			res = f_readdir(&dir, &fno);

			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;
			}
			if (fno.fname[0] == '.')
			{
				continue;
			}

			fn = fno.fname;

			if (list->ptr < FILEMGR_LIST_DEPDTH)
			{
				if ((fno.fattrib & AM_DIR) == 0)
				{
					if(strstr(fn, ext))
					{
						strcpy(tmp, Foldername);
						strcat(tmp, "/");
						strcat(tmp, fn);
						strncpy((char *)list->file[list->ptr].name, (char *)tmp, FILEMGR_FILE_NAME_SIZE);
						list->ptr++;
					}
				}
			}
		}
	}
	f_closedir(&dir);
}

/**
 * @brief  Storage Thread
 * @param  argument: pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void StorageThread(void const * argument)
{
#if (osCMSIS < 0x20000U)
	osEvent event;
#else
    uint32_t msg_box[1] = {0};
    osStatus_t msg_status;
    FRESULT f_res;
#endif

#if (osCMSIS < 0x20000U)
    for( ;; )
    {
    	event = osMessageGet( StorageEvent, osWaitForever );

    	if( event.status == osEventMessage )
    	{
    		switch(event.value.v)
    		{
    		case MSDDISK_CONNECTION_EVENT:
    			if (!f_mount(&MSDDISK_FatFs, MSDDISK_Drive,  0))
    			{
    				StorageStatus[MSD_DISK_UNIT] = 1;
    				STORAGE_NotifyConnectionChange (MSD_DISK_UNIT, 1);
    			}
    			break;

    		case MSDDISK_DISCONNECTION_EVENT:
    			f_mount(0, MSDDISK_Drive, 0);
    			StorageStatus[MSD_DISK_UNIT] = 0;
    			STORAGE_NotifyConnectionChange (MSD_DISK_UNIT, 0);
    			break;
    		}
    	}
    }
#else
    for( ;; )
    {
    	msg_status = osMessageQueueGet (StorageEvent, msg_box, (uint8_t*)osPriorityNormal, osWaitForever);

    	if(msg_status == osOK)
    	{
    		switch(msg_box[0])
    		{

    		case MSDDISK_CONNECTION_EVENT:
    			f_mount( 0, "", 0);
    			f_res = f_mount(&MSDDISK_FatFs, (TCHAR const*)MSDDISK_Drive,  0);

    			if (!f_res)
    			{
    				StorageStatus[MSD_DISK_UNIT] = 1;
    				STORAGE_NotifyConnectionChange (MSD_DISK_UNIT, 1);
    			}
    			break;

    		case MSDDISK_DISCONNECTION_EVENT:
    			f_mount(0, (TCHAR const*)MSDDISK_Drive, 0);
    			StorageStatus[MSD_DISK_UNIT] = 0;
    			STORAGE_NotifyConnectionChange (MSD_DISK_UNIT, 0);
    			break;
    		}
    	}
    }
#endif
}

/**
 * @brief  Notify Storage Unit connection state.
 * @retval None
 */
__weak void  STORAGE_NotifyConnectionChange(uint8_t unit, uint8_t v)
{

}

/**
 * @brief  File system read/write function check.
 * @retval None
 */
uint8_t  STORAGE_FS_OPERATION()
{
	FIL MyFile;     /* File object */
	FRESULT res;
	uint32_t byteswritten, bytesread;

	uint8_t wtext[] = "This is EDT working with FatFs + uSD";
	uint8_t rtext[100];

    if(f_open(&MyFile, "EDT_FatFs.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    {
    	res = f_write(&MyFile, wtext, sizeof(wtext), (UINT*)&byteswritten);

        if((byteswritten > 0) && (res == FR_OK))
        {
        	f_close(&MyFile);

            if(f_open(&MyFile, "EDT_FatFs.TXT", FA_READ) == FR_OK)
            {
            	res = f_read(&MyFile, rtext, sizeof(rtext), (UINT *)&bytesread);

                if((bytesread > 0) && (res == FR_OK))
                {
                	f_close(&MyFile);

                    if((bytesread == byteswritten))
                    {
                    	return 0;
                    }
                }
            }
        }
    }

    return 1;
    
    /** cnacel file*/
    
//    f_unlink ("EDT_FatFs.TXT");	
    
}

static void SD_Initialize(void)
{
	if (isInitialized == 0)
	{
		EDT_SD_Init();
		EDT_SD_ITConfig();

		if(EDT_SD_IsDetected())
		{
			isInitialized = 1;
		}
	}
}
/**
 * @}
 */

 /**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
