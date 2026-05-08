/*
 * ParseUpdates.hpp
 *
 *  Created on: 2026
 *      Author: Roberto.Sanchez
 *
 * Functions to import configuration updates from a USB pen drive
 * into the SD card filesystem.
 *
 * Convention:
 *   srcDir  - source directory on USB pen drive  (e.g. "1:/service/menu")
 *   dstDir  - destination directory on SD card   (e.g. "0:/config/menu")
 */

#ifndef APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEUPDATES_HPP_
#define APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEUPDATES_HPP_

#include <stdint.h>

/*
 * ImportDirFromUSB
 *
 * If srcDir exists on the USB pen drive, replaces the entire content of
 * dstDir on the SD card with the files found in srcDir.
 * Existing files in dstDir are deleted before copying.
 *
 * Returns:
 *   > 0  - number of files successfully copied
 *   0    - srcDir not found on USB (no update needed)
 *  -1    - error opening srcDir
 *  -2    - error clearing dstDir
 *  -3    - one or more files failed to copy (partial update)
 */
int16_t ImportDirFromUSB(const char* srcDir, const char* dstDir);

/*
 * ExportDirToUSB
 *
 * Recursively copies srcDir (SD card) into dstDir (USB pen drive).
 * dstDir is created if it does not exist; existing content is preserved
 * (caller is responsible for clearing dstDir beforehand if needed).
 *
 * Returns:
 *   >= 0  - number of files successfully copied
 *   < 0   - error
 */
int16_t ExportDirToUSB(const char* srcDir, const char* dstDir);

/*
 * ClearDirRecursive
 *
 * Recursively deletes all files and subdirectories inside dirPath,
 * then removes dirPath itself.
 * Returns 0 on success, -1 on error.
 */
int16_t ClearDirRecursive(const char* dirPath);

#endif /* APPLICATION_USER_TOUCHGFX_GUI_INCLUDE_GUI_MODEL_PARSEUPDATES_HPP_ */
