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
    static FIL     srcFile, dstFile;
    static uint8_t buf[COPY_BUF_SIZE];
    FRESULT fr;
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
 * ClearDirRecursive
 *
 * Recursively deletes all files and subdirectories inside dirPath,
 * then removes dirPath itself.
 * --------------------------------------------------------------------------*/
int16_t ClearDirRecursive(const char* dirPath)
{
    DIR          dir;
    static FILINFO fno;
    FRESULT      fr;
    static char  childPath[256];
    char         selfPath[64];  /* local copy — childPath may alias dirPath in recursive calls */

    /* Save dirPath NOW, before the loop overwrites childPath */
    strncpy(selfPath, dirPath, sizeof(selfPath) - 1);
    selfPath[sizeof(selfPath) - 1] = '\0';

    fr = f_opendir(&dir, dirPath);
    if (fr != FR_OK)
        return (fr == FR_NO_PATH) ? 0 : -1;

    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == '\0')
            break;

        snprintf(childPath, sizeof(childPath), "%s/%s", selfPath, fno.fname);

        if (fno.fattrib & AM_DIR)
            ClearDirRecursive(childPath);
        else
            f_unlink(childPath);
    }

    f_closedir(&dir);
    f_unlink(selfPath);
    return 0;
}

/* ---------------------------------------------------------------------------
 * CopyDirRecursive  (internal)
 *
 * Recursively copies all files and subdirectories from srcDir to dstDir.
 * dstDir must already exist.
 * --------------------------------------------------------------------------*/
static int16_t CopyDirRecursive(const char* srcDir, const char* dstDir)
{
    DIR            dir;
    static FILINFO fno;
    FRESULT        fr;
    static char    srcPath[256];
    static char    dstPath[256];
    char           srcBase[48], dstBase[48];  /* local copies — srcPath/dstPath may alias srcDir/dstDir */
    int16_t        copied = 0;

    strncpy(srcBase, srcDir, sizeof(srcBase) - 1);  srcBase[sizeof(srcBase)-1] = '\0';
    strncpy(dstBase, dstDir, sizeof(dstBase) - 1);  dstBase[sizeof(dstBase)-1] = '\0';

    fr = f_opendir(&dir, srcBase);
    if (fr != FR_OK)
        return -1;

    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == '\0')
            break;

        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcBase, fno.fname);
        snprintf(dstPath, sizeof(dstPath), "%s/%s", dstBase, fno.fname);

        if (fno.fattrib & AM_DIR)
        {
            f_mkdir(dstPath);
            int16_t sub = CopyDirRecursive(srcPath, dstPath);
            if (sub >= 0) copied += sub;
        }
        else
        {
            if (CopyFile(srcPath, dstPath) == 0)
            {
                printf("ExportDirToUSB: copied '%s'\n", fno.fname);
                copied++;
            }
            else
            {
                printf("ExportDirToUSB: ERROR copying '%s'\n", fno.fname);
            }
        }
    }

    f_closedir(&dir);
    return copied;
}

/* ---------------------------------------------------------------------------
 * ExportDirFlatToUSB  (internal helper + public entry)
 *
 * Copies every file found in srcDir (recursively) directly into dstDir,
 * without recreating the source subdirectory structure.
 *   0:/Config/Hw/a.hwr  →  1:/Log/a.hwr
 * --------------------------------------------------------------------------*/
static int16_t flat_copy_dir(const char* srcDir, const char* dstDir)
{
    DIR     dir;
    FILINFO fno;
    char    srcPath[256], dstPath[256];
    int16_t copied = 0;

    if (f_opendir(&dir, srcDir) != FR_OK) return 0;

    while (1)
    {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == '\0') break;

        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, fno.fname);

        if (fno.fattrib & AM_DIR)
        {
            copied += flat_copy_dir(srcPath, dstDir);
        }
        else
        {
            snprintf(dstPath, sizeof(dstPath), "%s/%s", dstDir, fno.fname);
            if (CopyFile(srcPath, dstPath) == 0)
                { printf("ExportFlat: '%s'\n", fno.fname); copied++; }
            else
                printf("ExportFlat: ERROR '%s'\n", fno.fname);
        }
    }
    f_closedir(&dir);
    return copied;
}

int16_t ExportDirFlatToUSB(const char* srcDir, const char* dstDir)
{
    f_mkdir(dstDir);
    printf("ExportDirFlatToUSB: '%s' -> '%s'\n", srcDir, dstDir);
    return flat_copy_dir(srcDir, dstDir);
}

/* ---------------------------------------------------------------------------
 * ExportDirToUSB
 *
 * Recursively copies srcDir (SD) into dstDir (USB).
 * Creates dstDir if it does not exist.
 * --------------------------------------------------------------------------*/
int16_t ExportDirToUSB(const char* srcDir, const char* dstDir)
{
    f_mkdir(dstDir);  /* no-op if already exists */
    printf("ExportDirToUSB: '%s' -> '%s'\n", srcDir, dstDir);
    return CopyDirRecursive(srcDir, dstDir);
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
