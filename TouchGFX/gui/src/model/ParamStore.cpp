/*
 * ParamStore.cpp
 *
 *  Created on: Dec 17, 2025
 *      Author: Roberto.Sanchez
 */
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <gui/model/ParamStore.hpp>

static FIL     gFile;
static uint8_t gFileOpen = 0;
const char    *gWriteLineFormat;
const char    *gReadLineFormat;

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
        fr = f_mkdir(path);
        if (fr == FR_OK) return FR_OK;
    }

    return fr;
}

int8_t p_store_openFileWrite(const char *filePath)
{
    FRESULT res;

    char dirPath[128];
    strncpy(dirPath, filePath, sizeof(dirPath));
    dirPath[sizeof(dirPath)-1] = '\0';
    char *lastSlash = strrchr(dirPath, '/');
    if (lastSlash) *lastSlash = '\0';

    res = ensureDirExists(dirPath);
    if (res != FR_OK) {
        printf("ERROR: cannot create directory '%s' (FatFS=%d)\n", dirPath, res);
        return -1;
    }

    if (gFileOpen)
        f_close(&gFile);

    res = f_open(&gFile, filePath, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        printf("ERROR: cannot open file '%s' (FatFS=%d)\n", filePath, res);
        gFileOpen = 0;
        return -1;
    }

    gFileOpen = 1;
    return 0;
}

static int8_t p_store_openFileRead(const char *filePath)
{
    FRESULT res;

    if (gFileOpen)
        f_close(&gFile);

    res = f_open(&gFile, filePath, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: cannot open file for read '%s' (FatFS=%d)\n", filePath, res);
        gFileOpen = 0;
        return -1;
    }

    gFileOpen = 1;
    return 0;
}

void p_store_closeFile(void)
{
    if (gFileOpen)
    {
        f_sync(&gFile);
        f_close(&gFile);
        gFileOpen = 0;
        printf("Param file closed\n");
    }
}

int8_t p_store_writeLine(uint16_t storageId, int32_t value)
{
    if (!gFileOpen)
    {
        printf("ERROR: param file not open\n");
        return -1;
    }

    char line[64];
    UINT bw;

    snprintf(line, sizeof(line), gWriteLineFormat, storageId, (long)value);

    FRESULT res = f_write(&gFile, line, strlen(line), &bw);
    if (res != FR_OK || bw != strlen(line))
        return -1;

    return 0;
}

/* ── Buffered line reader: reads 512 bytes per f_read call (~30x faster) ── */
#define LR_BUF_SIZE 512u

typedef struct {
    FIL     *fp;
    uint8_t  buf[LR_BUF_SIZE];
    UINT     len;
    UINT     pos;
} LineReader_t;

static void LR_init(LineReader_t *lr, FIL *fp)
{
    lr->fp  = fp;
    lr->len = 0;
    lr->pos = 0;
}

static int LR_getline(LineReader_t *lr, int8_t *out, int16_t maxlen)
{
    int16_t i = 0;
    if (!lr || !out || maxlen <= 1) return 0;
    while (i < maxlen - 1) {
        if (lr->pos >= lr->len) {
            f_read(lr->fp, lr->buf, LR_BUF_SIZE, &lr->len);
            lr->pos = 0;
            if (lr->len == 0) break;
        }
        uint8_t c = lr->buf[lr->pos++];
        if (c == '\r') continue;
        if (c == '\n') { out[i] = '\0'; return 1; }
        out[i++] = (int8_t)c;
    }
    out[i] = '\0';
    return (i > 0) ? 1 : 0;
}

void trim(int8_t* str)
{
    if (!str) return;

    int16_t i = 0;
    while (isspace((unsigned char)str[i])) i++;
    if (i > 0) memmove(str, str + i, strlen((char*)(str + i)) + 1);

    int16_t len = strlen((char*)str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
        str[--len] = '\0';
}

int8_t p_store_readLine(uint16_t storageId, int32_t *value)
{
    int8_t line[64];
    char key[16];
    char fileKey[16];
    long val;

    if (!gFileOpen)
    {
        printf("ERROR: param file not open\n");
        return -1;
    }

    if (!value)
    {
        printf("ERROR: p_store_readLine, null pointer for value\n");
        return -1;
    }

    snprintf(key, sizeof(key), gReadLineFormat, storageId);

    f_lseek(&gFile, 0);
    LineReader_t lr;
    LR_init(&lr, &gFile);
    while (LR_getline(&lr, line, sizeof(line)))
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

int8_t p_store_openWrite(void)
{
    gReadLineFormat  = "PARAM_%03u";
    gWriteLineFormat = "PARAM_%03u = %ld\r\n";

    printf("p_store_openWrite called\n");
    return p_store_openFileWrite("0:/Config/Params/params.prm");
}

int8_t p_store_openRead(void)
{
    gReadLineFormat  = "PARAM_%03u";
    gWriteLineFormat = "PARAM_%03u = %ld\r\n";

    printf("p_store_openRead called\n");
    return p_store_openFileRead("0:/Config/Params/params.prm");
}

void p_store_close(void)
{
    p_store_closeFile();
}
