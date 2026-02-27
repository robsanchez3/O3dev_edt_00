/*
 * ParseAppConfig.cpp
 *
 *  Created on: Dec 16, 2025
 *      Author: Roberto.Sanchez
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gui/model/Model.hpp>
#include <gui/model/parseappconfig.hpp>
#include <../../../../../Drivers/O3/Fsm_o3/fsm_o3_operation.h>/*TODO......  improve path*/
#include "ff.h"			/* Declarations of FatFs API */


/* **********************************************************************************************
 * Map of mode names to enumeration values
 * **********************************************************************************************/
ENUM_MAP modeMap[] = {
    {"SYRINGE_MODE",         SYRINGE_MODE},
    {"SYRINGE_AUTO_MODE",    SYRINGE_AUTO_MODE},
    {"SYRINGE_MANUAL_MODE",  SYRINGE_MANUAL_MODE},
    {"CONTINUOUS_MODE",      CONTINUOUS_MODE},
    {"INSUFFLATION_MODE",    INSUFFLATION_MODE},
    {"INSUFFLATION_R_MODE",  INSUFFLATION_R_MODE},
    {"INSUFFLATION_V_MODE",  INSUFFLATION_V_MODE},
    {"MANUAL_MODE",          MANUAL_MODE},
    {"DENTAL_MODE",          DENTAL_MODE},
    {"VACUUM_MODE",          VACUUM_MODE},
    {"VACUUM_TIME_MODE",     VACUUM_TIME_MODE},
    {"VACUUM_PRESSURE_MODE", VACUUM_PRESSURE_MODE},
    {"BAG_MODE",             BAG_MODE},
    {"CLOSED_BAG_MODE",      CLOSED_BAG_MODE},
    {"OPEN_BAG_MODE",        OPEN_BAG_MODE},
    {"DOSE_MODE",            DOSE_MODE},
    {"SALINE_MODE",          SALINE_MODE},
    {"MMF_END",              MMF_END},
    {"MMF_END_SHIFT_0_5",    MMF_END_SHIFT_0_5},
    {"MMF_END_SHIFT_1",      MMF_END_SHIFT_1},
    {"MMF_END_SHIFT_2",      MMF_END_SHIFT_2},
};

static const int16_t modeMapCount = sizeof(modeMap) / sizeof(modeMap[0]);

/* **********************************************************************************************
 *
 * HARDWARE CONFIG MAPS (.hwr)
 *
 * **********************************************************************************************/
ENUM_MAP hwMap[] = {
    {"A10068_XX_X",    A10068_XX_X},
    {"A10026_XX_X",    A10026_XX_X},
    {"A40170_XX_X",    A40170_XX_X},
    {"NO_O3_SENSOR",   NO_O3_SENSOR},
    {"AS03170",        AS03170},
    {"A9462_03",       A9462_03},
    {"A9462_04",       A9462_04},
    {"A9462_09",       A9462_09},
    {"A10021_02",      A10021_02},
    {"A10055_01",      A10055_01},
    {"NO_VACUUM_PUMP", NO_VACUUM_PUMP},
    {"52402054",       _52402054},
    {"CC02111",        CC02111},
    {"NO_LED_STRIP",   NO_LED_STRIP},
    {"A40236_XX_X",    A40236_XX_X},
    {"SERIAL",         SERIAL},
    {"I2C",            I2C},
};

static const int16_t hwMapCount = sizeof(hwMap) / sizeof(hwMap[0]);

int8 lastUserfileName[100] = {0};

// Searches a given directory and returns the first file whose extension
// matches the given one. The returned filename includes the full path,
// e.g. "config/settings.ini".
//
// Parameters:
//   dirPath  - directory to search (e.g. "config")
//   ext      - extension without the dot (e.g. "ini")
//   outName  - buffer where the full path + filename is written
//   outSize  - size of outName buffer
//
// Returns:
//   1  - file found
//   0  - no matching file found
//  -1  - error opening directory
//  -2  - error reading directory
//  -3  - output buffer too small
int16 FindFirstFileWithExt(const char* dirPath, const char* ext, char* outName, int16 outSize)

{
	DIR dir;
	FILINFO fno;
	FRESULT fr;

	fr = f_opendir(&dir, dirPath);
	if (fr != FR_OK) {
		printf("ERROR: cannot open directory '%s' (FatFS code: %d)\n", dirPath, fr);
		return -1;
	}

	// Read directory entries
	for (;;) {
		fr = f_readdir(&dir, &fno);
		if (fr != FR_OK) {
			printf("ERROR: failed to read directory '%s' (FatFS code: %d)\n", dirPath, fr);
			f_closedir(&dir);
			return -2;
		}

		// End of directory
		if (fno.fname[0] == '\0') {
			printf("INFO: no file with extension '%s' was found in '%s'.\n", ext, dirPath);
			break;
		}

		// Skip directories
		if (fno.fattrib & AM_DIR) {
			printf("INFO: skipping directory entry '%s'.\n", fno.fname);
			continue;
		}

		// Identify extension
		const char* dot = strrchr(fno.fname, '.');
		if (dot == NULL) {
			printf("INFO: file '%s' has no extension, skipping.\n", fno.fname);
			continue;
		}

		// Compare with requested extension
		if (strcasecmp(dot + 1, ext) == 0) {

			// Check output buffer size
			int needed = strlen(dirPath) + 1 + strlen(fno.fname) + 1;
			if (needed > outSize) {
				printf("ERROR: output buffer too small (needed %d bytes, available %d).\n",
						needed, outSize);
				f_closedir(&dir);
				return -3;
			}

			// Build "dirPath/filename.ext"
			snprintf(outName, outSize, "%s/%s", dirPath, fno.fname);

			printf("SUCCESS: found file '%s' with extension '%s'. Full path: '%s'\n",
					fno.fname, ext, outName);

			f_closedir(&dir);
			return 1;  // found
		}
		else {
			printf("INFO: file '%s' does not match extension '%s'.\n", fno.fname, ext);
		}
	}

	f_closedir(&dir);
	return 0; // no file found
}

uint8 get_line(FIL* fp, int8* buf, UINT maxlen)
{
	UINT br;
	int8 c;
	UINT i = 0;
	uint8 ret = 0; //failure

	while (i < maxlen - 1) {
		FRESULT res = f_read(fp, &c, 1, &br);
		if (res != FR_OK || br == 0) break;   // EOF
		if (c == '\n') { ret = 1; break; }   // EOL
		buf[i++] = c;
	}

	buf[i] = '\0';
	return ret;
}

/* **********************************************************************************************
 *
 * STRING HELPERS (UNIFIED)
 *
 * **********************************************************************************************/

char* ltrim(char* s)
{
	while (*s && isspace((unsigned char)*s)) s++;
	return s;
}

void rtrim(char* s)
{
	char* end = s + strlen(s);
	while (end > s && isspace((unsigned char)end[-1]))
		--end;
	*end = '\0';
}

void trim(char* s)
{
	char* p = ltrim(s);
	if (p != s)
		memmove(s, p, strlen(p) + 1);
	rtrim(s);
}

char strToBoolChar(const char *s)
{
	if (!s) return 0;
	if (!strcasecmp(s, "true") || !strcmp(s, "1"))
		return 1;
	return 0;
}

/* **********************************************************************************************
 *
 * ENUM SEARCH (UNIFIED)
 *
 * **********************************************************************************************/
char findEnumValueGeneric(const ENUM_MAP *map,
                          int16 mapCount,
                          const char *name,
                          int *outValue)
{
    if (!map || !name || !outValue)
        return 0;

    for (int i = 0; i < mapCount; i++) {
        if (!strcmp(map[i].name, name)) {
            *outValue = map[i].value;
            return 1;
        }
    }
    return 0;
}

/* **********************************************************************************************
 *
 * Parser to fill guiTherapyCtx from a file (.cfg) according to sections:
 *   [MODE]
 *   [MODE.STEP_X]
 *
 * The file format is like .ini files with key=value lines.
 *
 * **********************************************************************************************/
char matchModeStep(const char *section, int *outMode, int *outStep)
{
	for (int i = 0; i < modeMapCount; i++) {
		const char *prefix = modeMap[i].name;
		size_t len = strlen(prefix);

		if (!strncmp(section, prefix, len) &&
				!strncmp(section + len, ".STEP_", 6)) {
			*outMode = modeMap[i].value;
			*outStep = atoi(section + len + 6);
			return 1;
		}
	}
	return 0;
}

void processSection(ModCtx *p, const char *section)
{
    p->inModeSection = 0;
    p->inStepSection = 0;
    p->currentMode   = NO_MODE;

    int mode, step;

    /* [MODE] section */
    if (findEnumValueGeneric(modeMap, modeMapCount, section, &mode))
    {
        p->currentMode = (OPERATION_MODE_E)mode;
        p->inModeSection = 1;
        return;
    }

    /* [MODE.STEP_X] section */
    if (matchModeStep(section, &mode, &step))
    {
        p->currentMode = (OPERATION_MODE_E)mode;
        p->currentStep = step;
        p->inStepSection = 1;
    }
}

void processKeyValue(ModCtx* p, const char *key, const char *val)
{
	if (!p || p->currentMode != p->selectedMode)
		return;

	THERAPY_CTX *ctx = p->ctx;

	if (p->inModeSection) {
		if      (!strcmp(key,"END_DOSE_VISIBLE"))      ctx->endTotalDoseVisible   = strToBoolChar(val);
		else if (!strcmp(key,"END_VOLUME_VISIBLE"))    ctx->endTotalVolumeVisible = strToBoolChar(val);
		else if (!strcmp(key,"END_TIME_VISIBLE"))      ctx->endTotalTimeVisible   = strToBoolChar(val);
		else if (!strcmp(key,"OK_BTN_VISIBLE"))        ctx->okButtonVisible       = strToBoolChar(val);
		else if (!strcmp(key,"BIG_PAUSE_BTN_VISIBLE")) ctx->bigPauseButtonVisible = strToBoolChar(val);
		else if (!strcmp(key,"MAX_PRESSURE"))          ctx->maxPressure = atoi(val);
	}
	else if (p->inStepSection) {
		int s = p->currentStep;
		if (s < 0 || s >= MAX_THERAPY_STEPS) return;

		if      (!strcmp(key,"MIN_VAL"))            ctx->minValue[s] = atoi(val);
		else if (!strcmp(key,"MAX_VAL"))            ctx->maxValue[s] = atoi(val);
		else if (!strcmp(key,"DEF_VAL"))            ctx->defValue[s] = atoi(val);
		else if (!strcmp(key,"SEL_STEP"))           ctx->step[s]     = atoi(val);
		else if (!strcmp(key,"SECOND_SEL_VISIBLE")) ctx->secondSelectionVisible[s] = strToBoolChar(val);
		else if (!strcmp(key,"NEGATIVE"))           ctx->negativeValue[s] = strToBoolChar(val);
	}
}

/* **********************************************************************************************
 *
 * Main parser (.mod)
 *
 * **********************************************************************************************/

uint8_t loadTherapyFromFile(OPERATION_MODE_E mode, THERAPY_CTX *guiTherapyCtx)
{
	printf("Loading therapy config from file...\n");

	FIL File;       /* File object */
	int8_t ext[] = "mod";
//	int8_t dirPath[] = "1:/Config/Modes";  //TODO  remove when tested
	int8_t dirPath[] = "/Config/Modes";
	int8_t fileName[100];
	ModCtx mtx;
	uint8_t foundModeSection = 0;

	// Initialize parser context
	memset(&mtx,0,sizeof(mtx));   // TODO: keep an eye on this memset
	mtx.selectedMode = mode;
	mtx.currentMode  = NO_MODE;
	mtx.currentStep  = -1;
	mtx.ctx          = guiTherapyCtx;

	if (FindFirstFileWithExt((const char*)dirPath, (const char*)ext, (char*)fileName, sizeof(fileName)) <= 0)
	{
		printf("ERROR: could not find .%s file\n", ext);
		return 0;
	}
	else
	{
		printf("Using %s file\n", fileName);
	}

	if (f_open(&File, (const char*)fileName, FA_READ) != FR_OK) {
		printf("ERROR: could not open configuration mode file \n");
		return 0;
	}

	int8 line[LINE_BUF_SIZE];

	while (get_line(&File, line, sizeof(line))) {
		trim(line);

		if (line[0]=='\0' || line[0]==';' || line[0]=='#')
			continue;

		if (line[0]=='[') {
			char *e = strchr(line,']');
			if (!e) continue;
			*e = 0;
			processSection(&mtx, line+1);
			if (mtx.currentMode == mode && mtx.inModeSection)
				foundModeSection = 1;
			continue;
		}

		char *eq = strchr(line,'=');
		if (!eq) continue;

		*eq = 0;
		trim(line);
		trim(eq+1);
		processKeyValue(&mtx, line, eq+1);
	}

	f_close(&File);
	return foundModeSection;
}

/* **********************************************************************************************
 *
 * Debug print
 *
 * **********************************************************************************************/

void print_guiTherapyCtx(const THERAPY_CTX *ctx)
{
#if 1
	printf("Therapy context:\n");
	printf(" stepsNum=%d\n", ctx->stepsNum);
	printf(" mainIcon=%d\n", ctx->mainIcon);
	printf(" endTotalDoseVisible=%d\n", ctx->endTotalDoseVisible ? 1 : 0);
	printf(" endTotalVolumeVisible=%d\n", ctx->endTotalVolumeVisible ? 1 : 0);
	printf(" endTotalTimeVisible=%d\n", ctx->endTotalTimeVisible ? 1 : 0);
	printf(" okButtonVisible=%d\n", ctx->okButtonVisible ? 1 : 0);
	printf(" bigPauseButtonVisible=%d\n", ctx->bigPauseButtonVisible ? 1 : 0);
	printf(" maxPressure=%d\n", ctx->maxPressure);

	for (int i=0; i<MAX_THERAPY_STEPS; i++) {
		printf("\n STEP %d:\n", i);
		printf("   icon=%d\n", ctx->icon[i]);
		printf("   minValue=%lu\n", ctx->minValue[i]);
		printf("   maxValue=%lu\n", ctx->maxValue[i]);
		printf("   negativeValue=%u\n", ctx->negativeValue[i]);
		printf("   defValue=%lu\n", ctx->defValue[i]);
		printf("   step=%d\n", ctx->step[i]);
		printf("   units=%s\n", ctx->units[i]);
		printf("   secondSelectionVisible=%d\n", ctx->secondSelectionVisible[i] ? 1 : 0);
		printf("   therapyTargetValue=%d\n", ctx->therapyTargetValue[i]);
		printf("   delayIndicatorTime=%lu\n", ctx->delayIndicatorTime[i]);
		printf("   selectAvailable=%d\n", ctx->selectAvailable[i] ? 1 : 0);
		printf("   okAction=%p\n", ctx->okAction[i]);
		printf("   sliderAction=%p\n", ctx->sliderAction[i]);
	}
#endif
}

/* **********************************************************************************************
 *
 * Parser to fill main menu from a file (.mnu) information.
 * The file format is like .ini files with key=value lines.
 *
 * **********************************************************************************************/

int16 loadMainMenu(signed char * decode_out, int16 maxDevTherapies)
{
    FRESULT res;
    FIL File;
    int8 ext[] = "mnu";
//  int8 dirPath[] = "1:/Config/Menu"; //TODO  remove when tested
    int8 dirPath[] = "/Config/Menu"; //TODO  remove when tested
    int8 fileName[100];

    printf("Decoding configuration menu file...\n");

    if (FindFirstFileWithExt((const char*)dirPath, (const char*)ext,
            (char*)fileName, sizeof(fileName)) <= 0)
    {
        printf("ERROR: could not find .%s file\n", ext);
        return -1;
    }
    else
    {
        printf("Using %s file\n", fileName);
    }

    for (int16 i = 0; i < (maxDevTherapies + 1); i++)
        decode_out[i] = (int8) EMPTY_VALUE;

    res = f_open(&File, (const char*)fileName, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: could not open configuration menu file\n");
        return -1;
    }

    int8 line[LINE_BUF_SIZE];
    int16 lineNumber = 0;
    int16 errors = 0;

    while (get_line(&File, line, sizeof(line)))
    {
        lineNumber++;
        trim(line);

        // Skip empty lines or comments
        if (line[0] == '\0' || line[0] == '#')
            continue;

        int8 key[64];
        int8 value[64];

        // Flexible format:
        // "Posic1=Mode", "Posic1 = Mode", " Posic1    =Mode"
        if (sscanf((const char *)line, " %63[^=] = %63s", key, value) != 2)
        {
            printf("ERROR L%d: invalid format -> %s\n", lineNumber, line);
            errors++;
            continue;
        }

        trim(key);
        trim(value);

        // Expect keys like: Pos_00, Pos_01, ...
        if (strncmp((const char *)key, "Pos_", 4) != 0)
        {
            printf("ERROR L%d: invalid key -> %s\n", lineNumber, key);
            errors++;
            continue;
        }

        int index = atoi((const char *)key + 4);
        if (index < 0 || index >= (maxDevTherapies + 1))
        {
            printf("ERROR L%d: index out of range -> %d\n", lineNumber, index);
            errors++;
            continue;
        }

        if (decode_out[index] != EMPTY_VALUE)
        {
            printf("ERROR L%d: Posic%d is duplicated\n", lineNumber, index);
            errors++;
            continue;
        }

        int enumVal;
        if (!findEnumValueGeneric(modeMap, modeMapCount, (const char *)value, &enumVal))
        {
            printf("ERROR L%d: unrecognized mode '%s'\n", lineNumber, value);
            errors++;
            continue;
        }

        decode_out[index] = (int8)enumVal;
    }

    f_close(&File);

    if (errors > 0)
    {
        printf("\nThe configuration .mnu file contains %d error(s).\n", errors);
        return -1;
    }

    printf("Configuration .mnu successfully parsed.\n");

    // Display parsed values
#if 0
    for (int i = 0; i < (maxDevTherapies + 1); i++)
    {
        if (decode_out[i] != EMPTY_VALUE)
            printf("Posic%d = %d\n", i, decode_out[i]);
    }
#endif
    return 0;
}

/* **********************************************************************************************
 *
 * Parser to fill HW_CONFIG_T from a file (.hwr)
 *
 * **********************************************************************************************/
int16 loadHardwareConfig(HW_CONFIG_T *cfg)
{
    FRESULT res;
    FIL File;
    int8 ext[] = "hwr";
//  int8 dirPath[] = "1:/Config/HW";  //TODO  remove when tested
    int8 dirPath[] = "/Config/HW";
    int8 fileName[100];

    printf("Decoding hardware configuration file...\n");

    if (!cfg)
        return -1;

    memset(cfg, 0, sizeof(*cfg));

    if (FindFirstFileWithExt((const char*)dirPath, (const char*)ext,
            (char*)fileName, sizeof(fileName)) <= 0)
    {
        printf("ERROR: could not find .%s file\n", ext);
        return -1;
    }
    else
    {
        printf("Using %s file\n", fileName);
    }

    res = f_open(&File, (const char*)fileName, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: could not open hardware configuration file\n");
        return -1;
    }

    int8 line[LINE_BUF_SIZE];
    int16 lineNumber = 0;
    int16 errors = 0;

    while (get_line(&File, line, sizeof(line)))
    {
        lineNumber++;
        trim(line);

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *eq = strchr((char*)line, '=');
        if (!eq)
        {
            printf("ERROR L%d: invalid format -> %s\n", lineNumber, line);
            errors++;
            continue;
        }

        *eq = '\0';
        char *key = (char*)line;
        char *val = eq + 1;

        trim(key);
        trim(val);

        int enumVal;
        if (!findEnumValueGeneric(hwMap, hwMapCount, val, &enumVal))
        {
            printf("ERROR L%d: invalid value '%s'\n", lineNumber, val);
            errors++;
            continue;
        }

        if      (!strcmp(key, "C_BOARD"))     cfg->CBoard      = (uint8)enumVal;
        else if (!strcmp(key, "O3_SENSOR"))   cfg->O3Sensor    = (uint8)enumVal;
        else if (!strcmp(key, "TRANSFORMER")) cfg->Transformer = (uint8)enumVal;
        else if (!strcmp(key, "O3_TUBE"))     cfg->O3Tube      = (uint8)enumVal;
        else if (!strcmp(key, "V_PUMP"))      cfg->VPump       = (uint8)enumVal;
        else if (!strcmp(key, "LED_STRIP"))   cfg->LEDStrip    = (uint8)enumVal;
        else if (!strcmp(key, "COMM"))        cfg->Comm        = (uint8)enumVal;
        else
        {
            printf("ERROR L%d: unknown key '%s'\n", lineNumber, key);
            errors++;
        }
    }

    f_close(&File);

    if (errors > 0)
    {
        printf("\nThe configuration .hwr file contains %d error(s).\n", errors);
        return -1;
    }

    printf("Hardware configuration successfully parsed.\n");
    printf(" C_BOARD=%d\n", cfg->CBoard);
    printf(" O3_SENSOR=%d\n", cfg->O3Sensor);
    printf(" TRANSFORMER=%d\n", cfg->Transformer);
    printf(" O3_TUBE=%d\n", cfg->O3Tube);
    printf(" V_PUMP=%d\n", cfg->VPump);
    printf(" LED_STRIP=%d\n", cfg->LEDStrip);
    printf(" COMM=%d\n", cfg->Comm);

    return 0;
}

/* **********************************************************************************************
 *
 * Parser to fill syringe configuration from a file (.syr)
 *
 * **********************************************************************************************/
int16 loadSyringeConfig(uint16 *cfg)
{
    FRESULT res;
    FIL File;
    int8 ext[] = "syr";
    //  int8 dirPath[] = "1:/Config/Syringe";  //TODO  remove when tested
    int8 dirPath[] = "/Config/Syringe";
    int8 fileName[100];

    printf("Decoding syringe configuration file...\n");

    if (!cfg)
        return -1;

    if (FindFirstFileWithExt((const char*)dirPath, (const char*)ext,
                             (char*)fileName, sizeof(fileName)) <= 0)
    {
        printf("ERROR: could not find .%s file\n", ext);
        return -1;
    }
    else
    {
        printf("Using %s file\n", fileName);
    }

    res = f_open(&File, (const char*)fileName, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: could not open syringe configuration file\n");
        return -1;
    }

    int8 line[LINE_BUF_SIZE];
    int16 lineNumber = 0;
    int16 errors = 0;

    /* Initialize output */
    for (int i = 0; i < 5; i++)
        cfg[i] = 0;

    while (get_line(&File, line, sizeof(line)))
    {
        lineNumber++;
        trim(line);

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *eq = strchr((char*)line, '=');
        if (!eq)
        {
            printf("ERROR L%d: invalid format -> %s\n", lineNumber, line);
            errors++;
            continue;
        }

        *eq = '\0';
        char *key = (char*)line;
        char *val = eq + 1;

        trim(key);
        trim(val);

        uint16 value = (uint16)atoi(val);

        if      (!strcmp(key, "SYRINGE_STOP_5ML"))   cfg[0] = value;
        else if (!strcmp(key, "SYRINGE_STOP_10ML"))  cfg[1] = value;
        else if (!strcmp(key, "SYRINGE_STOP_20ML"))  cfg[2] = value;
        else if (!strcmp(key, "SYRINGE_STOP_50ML"))  cfg[3] = value;
        else if (!strcmp(key, "SYRINGE_STOP_100ML")) cfg[4] = value;
        else
        {
            printf("ERROR L%d: unknown key '%s'\n", lineNumber, key);
            errors++;
        }
    }

    f_close(&File);

    if (errors > 0)
    {
        printf("\nThe configuration .syr file contains %d error(s).\n", errors);
        return -1;
    }

    printf("Syringe configuration successfully parsed.\n");
    printf(" 5ml   = %u\n", cfg[0]);
    printf(" 10ml  = %u\n", cfg[1]);
    printf(" 20ml  = %u\n", cfg[2]);
    printf(" 50ml  = %u\n", cfg[3]);
    printf(" 100ml = %u\n", cfg[4]);

    return 0;
}

int16 loadUserConfig(USR_CONFIG_T *cfg)
{
    FRESULT res;
    FIL File;
    int8 ext[] = "usr";
//  int8 dirPath[] = "1:/Config/User"; //TODO  remove when tested
    int8 dirPath[] = "/Config/User";

    printf("Decoding software configuration file...\n");

    if (!cfg)
        return -1;

    memset(cfg, 0, sizeof(*cfg));

    if (FindFirstFileWithExt((const char*)dirPath,
                             (const char*)ext,
                             (char*)lastUserfileName,
                             sizeof(lastUserfileName)) <= 0)
    {
        printf("ERROR: could not find .%s file\n", ext);
        return -1;
    }
    else
    {
        printf("Using %s file\n", lastUserfileName);
    }

    res = f_open(&File, (const char*)lastUserfileName, FA_READ);
    if (res != FR_OK)
    {
        printf("ERROR: could not open user configuration file\n");
        return -1;
    }

    int8 line[LINE_BUF_SIZE];
    int16 lineNumber = 0;
    int16 errors = 0;

    while (get_line(&File, line, sizeof(line)))
    {
        lineNumber++;
        trim(line);

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *eq = strchr((char*)line, '=');
        if (!eq)
        {
            printf("ERROR L%d: invalid format -> %s\n", lineNumber, line);
            errors++;
            continue;
        }

        *eq = '\0';
        char *key = (char*)line;
        char *val = eq + 1;

        trim(key);
        trim(val);

        int value = atoi(val);

        if      (!strcmp(key, "GENERATION_MODE")) cfg->userGenerationMode = (uint8)value;
        else if (!strcmp(key, "RFU_0"))           cfg->rfu_0            = (uint8)value;
        else if (!strcmp(key, "RFU_1"))           cfg->rfu_1            = (uint8)value;
        else
        {
            printf("ERROR L%d: unknown key '%s'\n", lineNumber, key);
            errors++;
        }
    }

    f_close(&File);

    if (errors > 0)
    {
        printf("\nThe configuration .usr file contains %d error(s).\n", errors);
        return -1;
    }

    printf("User configuration successfully parsed.\n");
    printf(" Generation mode=%d\n", cfg->userGenerationMode);
    printf(" rfu_0=%d\n", cfg->rfu_0);
    printf(" rfu_1=%d\n", cfg->rfu_1);

    return 0;
}

int16 saveUserConfig(const USR_CONFIG_T *cfg)
{
    FRESULT res;
    FIL file;
    UINT bw;
    char line[80];

    if (!cfg)
        return -1;

    printf("Saving user configuration file...\n");

    /* Create / overwrite file */
    res = f_open(&file, lastUserfileName, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        printf("ERROR: (%d) could not create .usr file (%s)\n", res, lastUserfileName);
        return -1;
    }

    /* Optional header */
    snprintf(line, sizeof(line), "# User configuration file\n");
    res = f_write(&file, line, strlen(line), &bw);

    snprintf(line, sizeof(line), "# Generated by application\n\n");
    res = f_write(&file, line, strlen(line), &bw);

    /* Write parameters */
    snprintf(line, sizeof(line), "GENERATION_MODE = %d\n", cfg->userGenerationMode);
    res = f_write(&file, line, strlen(line), &bw);
    snprintf(line, sizeof(line), "RFU_0           = %d\n", cfg->rfu_0);
    res = f_write(&file, line, strlen(line), &bw);
    snprintf(line, sizeof(line), "RFU_1           = %d\n", cfg->rfu_1);
    res = f_write(&file, line, strlen(line), &bw);

    res = f_close(&file);
    if (res != FR_OK)
    {
        printf("ERROR: could not close .usr file\n");
        return -1;
    }

    printf("User configuration successfully saved.\n");
    return 0;
}
