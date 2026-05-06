/*============================================================================
 * iap.h -- In-Application Programming host library (public contract)
 *
 *  Talks to the PSoC1 CY8C29466 ControlBoard bootloader (bootloader.asm).
 *  Portable C99: no malloc, no float, no globals. Same .h/.c compiles for
 *  PC emulator (Win32/POSIX serial) and future embedded host (UART driver).
 *
 *  Wire protocol (must match bootloader.asm PROTOCOL section):
 *     0xBB 0x01                        -> ENTER        ACK
 *     0xBB 0x02                        -> INVALIDATE   ACK
 *     0xBB 0x03 BH BL D[64] XOR        -> WRITE        ACK | NAK
 *     0xBB 0x04                        -> CHECKSUM     <XOR> ACK
 *     0xBB 0x05                        -> VALIDATE     ACK
 *     0xBB 0x06                        -> RESET        ACK, then WDT reset
 *
 *  ACK = 0xAA, NAK = 0x55, UART = 57600 8N1.
 *===========================================================================*/
#ifndef IAP_H
#define IAP_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--- Protocol constants (exposed so HAL/app can log/diagnose) --------------*/
#define IAP_MAGIC           0xBBu
#define IAP_CMD_ENTER       0x01u
#define IAP_CMD_INVAL       0x02u
#define IAP_CMD_WRITE       0x03u
#define IAP_CMD_CHKSUM      0x04u
#define IAP_CMD_VALIDATE    0x05u
#define IAP_CMD_RESET       0x06u
#define IAP_CMD_READ        0x07u
#define IAP_ACK             0xAAu
#define IAP_NAK             0x55u

#define IAP_BLOCK_SIZE      64u
#define IAP_APP_FIRST_BLK   0x0014u    /* first writable block (blk 20 = 0x0500, start of unlocked IVT) */
#define IAP_APP_LAST_BLK    0x01F9u    /* one past last writable (blk 0x01F8 = 0x7E00-0x7E3F is app area) */

#define IAP_UART_BAUD       57600u

/*--- Return codes -----------------------------------------------------------*/
typedef enum {
    IAP_OK        = 0,
    IAP_E_TIMEOUT = 1,   /* no reply within expected window               */
    IAP_E_NAK     = 2,   /* bootloader returned NAK                       */
    IAP_E_PROTO   = 3,   /* unexpected byte (neither ACK nor NAK)         */
    IAP_E_RANGE   = 4,   /* block id outside [FIRST, LAST)                */
    IAP_E_IO      = 5,   /* HAL serial_write / serial_read failed         */
    IAP_E_SRC     = 6,   /* image source callback reported error          */
    IAP_E_CHK     = 7    /* checksum mismatch (vs. caller's expectation)  */
} iap_rc_t;

/*--- Hardware abstraction --------------------------------------------------
 *  One instance per target (PC or embedded). All pointers non-NULL.
 *   serial_write : blocking write, returns n_written or <0 on error
 *   serial_read  : blocking read with timeout; returns n_read (may be 0 on
 *                  timeout) or <0 on error. n requested is always small.
 *   millis       : monotonic ms clock, wrap-safe arithmetic in caller
 *   log          : optional; may be NULL. tag = "IAP"|"HAL"|...
 *---------------------------------------------------------------------------*/
typedef struct iap_hal_s {
    int      (*serial_write)(const uint8_t* buf, size_t n);
    int      (*serial_read) (uint8_t* buf, size_t n, uint32_t timeout_ms);
    uint32_t (*millis)(void);
    void     (*log)(const char* tag, const char* msg);
} iap_hal_t;

/*--- Image source callback --------------------------------------------------
 *  Called by iap_run() to get the next block to program. Source owns the
 *  iteration: it decides which block_id to return and when it is done.
 *
 *   ctx        : opaque caller context (hex parser state, file handle, ...)
 *   block_id   : [out] 16-bit flash block number of the returned data
 *   out        : [out] IAP_BLOCK_SIZE bytes of block payload
 *   is_last    : [out] non-zero means this is the final block of the image
 *
 *   return 0 on success, non-zero to abort (library maps to IAP_E_SRC).
 *   The source must have at least one block; is_last may be set on the
 *   first call if the image is exactly one block long.
 *---------------------------------------------------------------------------*/
typedef int (*iap_block_src_t)(void*     ctx,
                               uint16_t* block_id,
                               uint8_t   out[IAP_BLOCK_SIZE],
                               int*      is_last);

/*--- Low-level API: one command per call -----------------------------------
 *  All return IAP_OK on success, iap_rc_t error otherwise. None of these
 *  retry internally; callers implement retry policy if needed.
 *---------------------------------------------------------------------------*/

/* 0xBB 0x01 -> ACK. Also used as wake-up trigger in the BL's 100 ms window. */
iap_rc_t iap_enter     (const iap_hal_t* hal);

/* 0xBB 0x02 -> ACK. Clears EEPROM valid flag; next reset stays in BL.       */
iap_rc_t iap_invalidate(const iap_hal_t* hal);

/* 0xBB 0x03 BH BL D[64] XOR -> ACK|NAK. XOR computed internally over data.  *
 * block_id must satisfy IAP_APP_FIRST_BLK <= block_id < IAP_APP_LAST_BLK.   */
iap_rc_t iap_write_blk (const iap_hal_t* hal,
                        uint16_t block_id,
                        const uint8_t data[IAP_BLOCK_SIZE]);

/* 0xBB 0x04 -> <XOR> ACK. Returns 8-bit XOR of flash [0x0500, 0x7E00) in    *
 * *got. Caller compares with locally-computed expectation.                  */
iap_rc_t iap_checksum  (const iap_hal_t* hal, uint8_t* got);

/* 0xBB 0x05 -> ACK. Writes 0x55AA to EEPROM flag; next reset jumps to app.  */
iap_rc_t iap_validate  (const iap_hal_t* hal);

/* 0xBB 0x06 -> ACK, then BL triggers WDT reset. After this call the link    *
 * goes silent for ~20 ms; caller should close/reopen port if needed.        */
iap_rc_t iap_reset     (const iap_hal_t* hal);

/* 0xBB 0x07 BH BL -> ACK + 64 bytes. Reads flash block without writing.    *
 * block_id: any value (no range check in BL; use same id as iap_write_blk). *
 * buf must be IAP_BLOCK_SIZE bytes.                                          */
iap_rc_t iap_read_blk  (const iap_hal_t* hal,
                         uint16_t block_id,
                         uint8_t  buf[IAP_BLOCK_SIZE]);

/*--- High-level orchestrator -----------------------------------------------
 *  Runs the full IAP cycle:
 *     enter -> invalidate -> [write_blk x N] -> checksum -> validate -> reset
 *
 *  - Retries each write up to IAP_WRITE_RETRIES (see iap.c) on NAK/timeout.
 *  - Aborts on IAP_WRITE_RETRIES consecutive failures, leaving flag INVALID
 *    so next power-up stays in BL (safe).
 *  - Before validate, reads BL checksum and compares against expected_xor.
 *    If mismatch -> returns IAP_E_CHK without validating (device stays in BL).
 *  - Never calls iap_reset() if any prior step failed (safer to keep BL).
 *
 *   src / src_ctx  : source of block data, iterated until is_last != 0
 *   expected_xor   : XOR of the full app range the caller expects to see
 *                    after programming (see note in iap.c on gaps)
 *---------------------------------------------------------------------------*/
iap_rc_t iap_run(const iap_hal_t* hal,
                 iap_block_src_t  src,
                 void*            src_ctx,
                 uint8_t          expected_xor);

/*--- Utilities (stateless, pure) -------------------------------------------*/

/* 8-bit XOR of buffer. */
uint8_t  iap_xor8(const uint8_t* buf, size_t n);

/* Human-readable code name, never NULL. */
const char* iap_strerror(iap_rc_t rc);

#ifdef __cplusplus
}
#endif

#endif /* IAP_H */
