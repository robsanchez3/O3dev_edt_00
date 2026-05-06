/*============================================================================
 * iap.c -- IAP host library implementation. See iap.h for the contract.
 *===========================================================================*/
#include "iap.h"

/*--- Internal knobs (not part of public API) -------------------------------*/
#define IAP_TO_ACK_MS        500u   /* most commands: ACK within ~50 ms   */
#define IAP_TO_WRITE_MS     1000u   /* flash erase+write per row <100 ms  */
#define IAP_TO_RESET_MS      200u   /* ACK before WDT reset               */
#define IAP_WRITE_RETRIES      3u   /* per-block retry budget             */

/*--- Small helpers ---------------------------------------------------------*/

static void log_msg(const iap_hal_t* hal, const char* msg)
{
    if (hal->log) hal->log("IAP", msg);
}

/* Send exactly n bytes. Returns IAP_OK or IAP_E_IO. */
static iap_rc_t tx(const iap_hal_t* hal, const uint8_t* buf, size_t n)
{
    int w = hal->serial_write(buf, n);
    if (w < 0 || (size_t)w != n) return IAP_E_IO;
    return IAP_OK;
}

/* Receive exactly n bytes within timeout_ms total. */
static iap_rc_t rx(const iap_hal_t* hal, uint8_t* buf, size_t n, uint32_t to_ms)
{
    uint32_t start = hal->millis();
    size_t   got   = 0;
    while (got < n) {
        uint32_t elapsed = hal->millis() - start;
        if (elapsed >= to_ms) return IAP_E_TIMEOUT;
        int r = hal->serial_read(buf + got, n - got, to_ms - elapsed);
        if (r < 0) return IAP_E_IO;
        got += (size_t)r;
    }
    return IAP_OK;
}

/* Read one byte and map to IAP_OK (ACK) / IAP_E_NAK / IAP_E_PROTO. */
static iap_rc_t expect_ack(const iap_hal_t* hal, uint32_t to_ms)
{
    uint8_t b;
    iap_rc_t rc = rx(hal, &b, 1, to_ms);
    if (rc != IAP_OK) return rc;
    if (b == IAP_ACK) return IAP_OK;
    if (b == IAP_NAK) return IAP_E_NAK;
    return IAP_E_PROTO;
}

/* Send {magic, cmd}. */
static iap_rc_t tx_cmd(const iap_hal_t* hal, uint8_t cmd)
{
    uint8_t h[2] = { IAP_MAGIC, cmd };
    return tx(hal, h, sizeof h);
}

/*--- Public utilities -------------------------------------------------------*/

uint8_t iap_xor8(const uint8_t* buf, size_t n)
{
    uint8_t x = 0;
    while (n--) x ^= *buf++;
    return x;
}

const char* iap_strerror(iap_rc_t rc)
{
    switch (rc) {
        case IAP_OK:        return "OK";
        case IAP_E_TIMEOUT: return "timeout";
        case IAP_E_NAK:     return "NAK from bootloader";
        case IAP_E_PROTO:   return "unexpected byte";
        case IAP_E_RANGE:   return "block id out of range";
        case IAP_E_IO:      return "serial I/O error";
        case IAP_E_SRC:     return "image source error";
        case IAP_E_CHK:     return "checksum mismatch";
        default:            return "unknown";
    }
}

/*--- Low-level commands ----------------------------------------------------*/

iap_rc_t iap_enter(const iap_hal_t* hal)
{
    iap_rc_t rc = tx_cmd(hal, IAP_CMD_ENTER);
    if (rc != IAP_OK) return rc;
    return expect_ack(hal, IAP_TO_ACK_MS);
}

iap_rc_t iap_invalidate(const iap_hal_t* hal)
{
    iap_rc_t rc = tx_cmd(hal, IAP_CMD_INVAL);
    if (rc != IAP_OK) return rc;
    return expect_ack(hal, IAP_TO_ACK_MS);
}

iap_rc_t iap_write_blk(const iap_hal_t* hal,
                       uint16_t block_id,
                       const uint8_t data[IAP_BLOCK_SIZE])
{
    if (block_id < IAP_APP_FIRST_BLK || block_id >= IAP_APP_LAST_BLK)
        return IAP_E_RANGE;

    /* Header: BB 03 BH BL */
    uint8_t  hdr[4] = {
        IAP_MAGIC, IAP_CMD_WRITE,
        (uint8_t)(block_id >> 8),
        (uint8_t)(block_id & 0xFF)
    };
    iap_rc_t rc = tx(hal, hdr, sizeof hdr);
    if (rc != IAP_OK) return rc;

    /* Payload: 64 bytes */
    rc = tx(hal, data, IAP_BLOCK_SIZE);
    if (rc != IAP_OK) return rc;

    /* XOR trailer */
    uint8_t x = iap_xor8(data, IAP_BLOCK_SIZE);
    rc = tx(hal, &x, 1);
    if (rc != IAP_OK) return rc;

    return expect_ack(hal, IAP_TO_WRITE_MS);
}

iap_rc_t iap_checksum(const iap_hal_t* hal, uint8_t* got)
{
    iap_rc_t rc = tx_cmd(hal, IAP_CMD_CHKSUM);
    if (rc != IAP_OK) return rc;

    /* BL sends <XOR> then ACK. */
    uint8_t reply[2];
    rc = rx(hal, reply, 2, IAP_TO_WRITE_MS);
    if (rc != IAP_OK) return rc;
    if (reply[1] != IAP_ACK)
        return (reply[1] == IAP_NAK) ? IAP_E_NAK : IAP_E_PROTO;

    if (got) *got = reply[0];
    return IAP_OK;
}

iap_rc_t iap_validate(const iap_hal_t* hal)
{
    iap_rc_t rc = tx_cmd(hal, IAP_CMD_VALIDATE);
    if (rc != IAP_OK) return rc;
    return expect_ack(hal, IAP_TO_ACK_MS);
}

iap_rc_t iap_reset(const iap_hal_t* hal)
{
    iap_rc_t rc = tx_cmd(hal, IAP_CMD_RESET);
    if (rc != IAP_OK) return rc;
    return expect_ack(hal, IAP_TO_RESET_MS);
}

iap_rc_t iap_read_blk(const iap_hal_t* hal,
                      uint16_t block_id,
                      uint8_t  buf[IAP_BLOCK_SIZE])
{
    uint8_t hdr[4] = {
        IAP_MAGIC, IAP_CMD_READ,
        (uint8_t)(block_id >> 8),
        (uint8_t)(block_id & 0xFF)
    };
    iap_rc_t rc = tx(hal, hdr, sizeof hdr);
    if (rc != IAP_OK) return rc;

    iap_rc_t ack = expect_ack(hal, IAP_TO_ACK_MS);
    if (ack != IAP_OK) return ack;

    return rx(hal, buf, IAP_BLOCK_SIZE, IAP_TO_ACK_MS);
}

/*--- Orchestrator ----------------------------------------------------------*/

/* Retry wrapper around iap_write_blk: retries on NAK / timeout / I/O.
 * IAP_E_RANGE is not retried (permanent). */
static iap_rc_t write_blk_retry(const iap_hal_t* hal,
                                uint16_t block_id,
                                const uint8_t data[IAP_BLOCK_SIZE])
{
    iap_rc_t rc = IAP_OK;
    for (unsigned i = 0; i < IAP_WRITE_RETRIES; ++i) {
        rc = iap_write_blk(hal, block_id, data);
        if (rc == IAP_OK)      return IAP_OK;
        if (rc == IAP_E_RANGE) return rc;
    }
    return rc;
}

iap_rc_t iap_run(const iap_hal_t* hal,
                 iap_block_src_t  src,
                 void*            src_ctx,
                 uint8_t          expected_xor)
{
    iap_rc_t rc;

    log_msg(hal, "ENTER");
    rc = iap_enter(hal);
    if (rc != IAP_OK) return rc;

    log_msg(hal, "INVALIDATE");
    rc = iap_invalidate(hal);
    if (rc != IAP_OK) return rc;

    /* Stream blocks until the source sets is_last. */
    for (;;) {
        uint8_t  buf[IAP_BLOCK_SIZE];
        uint16_t block_id = 0;
        int      is_last  = 0;

        if (src(src_ctx, &block_id, buf, &is_last) != 0)
            return IAP_E_SRC;

        rc = write_blk_retry(hal, block_id, buf);
        if (rc != IAP_OK) return rc;     /* flag stays invalid -> safe */

        if (is_last) break;
    }

    log_msg(hal, "CHECKSUM");
    uint8_t got = 0;
    rc = iap_checksum(hal, &got);
    if (rc != IAP_OK)           return rc;
    if (got != expected_xor)    return IAP_E_CHK;

    log_msg(hal, "VALIDATE");
    rc = iap_validate(hal);
    if (rc != IAP_OK) return rc;

    log_msg(hal, "RESET");
    return iap_reset(hal);
}
