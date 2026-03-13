/*
 * ParseUpdates.cpp
 *
 *  Created on: 2026
 *      Author: Roberto.Sanchez
 *
 * Imports configuration updates from a USB pen drive into the SD card.
 */

#include <string.h>
#include <stdio.h>

#include <gui/model/ParseUpdates.hpp>
#include "ff.h"

#define COPY_BUF_SIZE  512

/* ---------------------------------------------------------------------------
 * ClearDir
 *
 * Deletes all files in a directory (non-recursive).
 * Returns 0 on success, -1 on error.
 * --------------------------------------------------------------------------*/
static int16_t ClearDir(const char* dirPath)
{
    DIR     dir;
    FILINFO fno;
    FRESULT fr;
    char    filePath[256];

    fr = f_opendir(&dir, dirPath);
    if (fr != FR_OK)
        return -1;

    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == '\0')
            break;

        if (fno.fattrib & AM_DIR)
            continue;   /* skip subdirectories */

        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, fno.fname);
        f_unlink(filePath);
    }

    f_closedir(&dir);
    return 0;
}

/* ---------------------------------------------------------------------------
 * CopyFile
 *
 * Copies a single file from srcPath to dstPath.
 * Returns 0 on success, -1 on error.
 * --------------------------------------------------------------------------*/
static int16_t CopyFile(const char* srcPath, const char* dstPath)
{
    FIL     srcFile, dstFile;
    FRESULT fr;
    uint8_t buf[COPY_BUF_SIZE];
    UINT    bytesRead, bytesWritten;
    int16_t result = 0;

    fr = f_open(&srcFile, srcPath, FA_READ);
    if (fr != FR_OK)
    {
        printf("CopyFile: f_open src failed fr=%d '%s'\n", fr, srcPath);
        return -1;
    }

    fr = f_open(&dstFile, dstPath, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK)
    {
        printf("CopyFile: f_open dst failed fr=%d '%s'\n", fr, dstPath);
        f_close(&srcFile);
        return -1;
    }

    while (1)
    {
        fr = f_read(&srcFile, buf, sizeof(buf), &bytesRead);
        if (fr != FR_OK || bytesRead == 0)
            break;

        fr = f_write(&dstFile, buf, bytesRead, &bytesWritten);
        if (fr != FR_OK || bytesWritten != bytesRead)
        {
            printf("CopyFile: f_write failed fr=%d written=%u expected=%u\n", fr, bytesWritten, bytesRead);
            result = -1;
            break;
        }
    }

    f_close(&srcFile);
    f_close(&dstFile);
    return result;
}

/* ---------------------------------------------------------------------------
 * ImportDirFromUSB
 *
 * If srcDir exists on the USB pen drive, replaces the content of dstDir
 * on the SD card with the files found in srcDir.
 * --------------------------------------------------------------------------*/
int16_t ImportDirFromUSB(const char* srcDir, const char* dstDir)
{
    DIR     dir;
    FILINFO fno;
    FRESULT fr;
    char    srcPath[256];
    char    dstPath[256];
    int16_t copied = 0;
    int16_t errors = 0;

    /* Check if source directory exists on USB */
    fr = f_opendir(&dir, srcDir);
    if (fr != FR_OK)
    {
        /* srcDir not found - no update needed */
        return 0;
    }

    printf("ImportDirFromUSB: '%s' found, updating '%s'\n", srcDir, dstDir);

    /* Clear destination directory on SD */
    if (ClearDir(dstDir) != 0)
    {
        printf("ImportDirFromUSB: ERROR clearing '%s'\n", dstDir);
        f_closedir(&dir);
        return -2;
    }

    /* Copy each file from srcDir to dstDir */
    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == '\0')
            break;

        if (fno.fattrib & AM_DIR)
            continue;   /* skip subdirectories */

        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, fno.fname);
        snprintf(dstPath, sizeof(dstPath), "%s/%s", dstDir, fno.fname);

        if (CopyFile(srcPath, dstPath) == 0)
        {
            printf("ImportDirFromUSB: copied '%s'\n", fno.fname);
            copied++;
        }
        else
        {
            printf("ImportDirFromUSB: ERROR copying '%s'\n", fno.fname);
            errors++;
        }
    }

    f_closedir(&dir);

    if (errors > 0)
        return -3;

    return copied;
}
