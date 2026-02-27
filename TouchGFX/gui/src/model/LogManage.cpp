/*
 * LogManage.cpp
 *
 *  Created on: Dec 17, 2025
 *      Author: Roberto.Sanchez
 */
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <gui/model/LogManage.hpp>

static FIL gLogFile;	         // FatFs file object
static uint8_t gLogFileOpen = 0; // file open flag
const char *gWriteLineFormat;	 // write line format
const char* gReadLineFormat;     // read line format

FRESULT ensureDirExists(const char *path)
{
    DIR dir;
    FRESULT fr;

    fr = f_opendir(&dir, path);
    if (fr == FR_OK) {
        f_closedir(&dir);
        return FR_OK;
    }

    if (fr == FR_NO_PATH) {
        /* try to create directory */
        fr = f_mkdir(path);
        if (fr == FR_OK) return FR_OK;
    }

    return fr;
}

int8_t openLogFileWrite(const char *filePath)
{
    FRESULT res;

    /* extract directory part from filePath */
    char dirPath[128];
    strncpy(dirPath, filePath, sizeof(dirPath));
    dirPath[sizeof(dirPath)-1] = '\0';
    char *lastSlash = strrchr(dirPath, '/');
    if (lastSlash) *lastSlash = '\0'; // remove filename, keep directory

    res = ensureDirExists(dirPath);
    if (res != FR_OK) {
        printf("ERROR: cannot create directory '%s' (FatFS=%d)\n", dirPath, res);
        return -1;
    }

    if (gLogFileOpen)
        f_close(&gLogFile);

    res = f_open(&gLogFile, filePath, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        printf("ERROR: cannot open log file '%s' (FatFS=%d)\n", filePath, res);
        gLogFileOpen = 0;
        return -1;
    }

    gLogFileOpen = 1;
    return 0;
}

int8_t openLogFileRead(const char *filePath)
{
    FRESULT res;

    if (gLogFileOpen)
        f_close(&gLogFile);

    res = f_open(&gLogFile, filePath, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: cannot open log file for read '%s' (FatFS=%d)\n",
               filePath, res);
        gLogFileOpen = 0;
        return -1;
    }

    gLogFileOpen = 1;
    return 0;
}

void closeLogFile(void)
{
    if (gLogFileOpen)
    {
        f_sync(&gLogFile);
        f_close(&gLogFile);
        gLogFileOpen = 0;
        printf("Log file closed\n");
    }
}

int8_t writeLogLine(uint16_t storageId, int32_t value)
{
    if (!gLogFileOpen)
    {
        printf("ERROR: log file not open\n");
        return -1;
    }

    char line[64];
    UINT bw;

    /* Format: PARAM_000 = 123 */
    snprintf(line, sizeof(line), gWriteLineFormat, storageId, (long)value);

    FRESULT res = f_write(&gLogFile, line, strlen(line), &bw);
    if (res != FR_OK || bw != strlen(line))
    {
        return -1;
    }

    return 0;
}

/**
 * Reads a line from the given file into a buffer.
 * Handles '\n', '\r\n', and the last line without newline.
 *
 * @param fp      Pointer to an open FIL object (FatFs file)
 * @param buf     Buffer to store the line
 * @param maxLen  Maximum buffer size (including null terminator)
 * @return        1 if a line was read, 0 if EOF reached without reading data
 */
int get_line(FIL *fp, int8_t *buf, int16_t maxLen)
{
    int16_t i = 0;
    int8_t c;
    UINT br;

    if (!fp || !buf || maxLen <= 1)
        return 0;

    while (i < maxLen - 1)
    {
        /* Read a single character */
        if (f_read(fp, &c, 1, &br) != FR_OK || br == 0)
            break;  // EOF or read error

        if (c == '\r')
            continue; // Ignore carriage return

        if (c == '\n')
            break;    // End of line

        buf[i++] = c;
    }

    buf[i] = '\0'; // Null-terminate the string

    /* Return 1 if at least one character was read, otherwise 0 */
    return (i > 0 || br > 0) ? 1 : 0;
}

/**
 * Remove leading and trailing whitespace from an int8_t string.
 *
 * @param str The string to trim. Modified in-place.
 */
void trim(int8_t* str)
{
    if (!str) return;

    // Remove leading whitespace
    int16_t i = 0;
    while (isspace((unsigned char)str[i])) i++;
    if (i > 0) memmove(str, str + i, strlen((char*)(str + i)) + 1);

    // Remove trailing whitespace
    int16_t len = strlen((char*)str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

int8_t readLogLine(uint16_t storageId, int32_t *value)
{
    int8_t line[64];
    char key[16];
    char fileKey[16];
    long val;

    if (!gLogFileOpen)
    {
        printf("ERROR: log file not open\n");
        return -1;
    }

    if (!value)
    {
    	printf("ERROR: readLogLine, null pointer for value\n");
        return -1;
    }

    /* Build key */
    snprintf(key, sizeof(key), gReadLineFormat, storageId);

    /* Rewind file to beginning */
    f_lseek(&gLogFile, 0);

    while (get_line(&gLogFile, line, sizeof(line)))
    {
        trim(line);

        if (line[0] == '\0' || line[0] == '#')
            continue;

        if (sscanf((const char *)line, " %15[^=] = %ld", fileKey, &val) != 2)
            continue;

        trim((int8_t*)fileKey);

        if (!strcmp(fileKey, key))
        {
            *value = (int32_t)val;
            return 0;
        }
    }

    printf("ERROR: %s not found\n", key);
    return -1;
}

int8_t openParamFileWrite(void)
{
	gReadLineFormat  = "PARAM_%03u";
	gWriteLineFormat = "PARAM_%03u = %ld\r\n";

	printf("openParamFileWrite called\n");
	return openLogFileWrite("1:/Config/Params/params.prm");
}

int8_t openParamFileRead(void)
{
	gReadLineFormat  = "PARAM_%03u";
	gWriteLineFormat = "PARAM_%03u = %ld\r\n";

	printf("openParamFileRead called\n");
	return openLogFileRead("1:/Config/Params/params.prm");
}

void closeParamFile(void)
{
	closeLogFile();
}
