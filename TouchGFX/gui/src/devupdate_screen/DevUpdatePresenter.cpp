#include <gui/devupdate_screen/DevUpdateView.hpp>
#include <gui/devupdate_screen/DevUpdatePresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

#ifndef SIMULATOR
#include "../../../../Drivers/IAP/gen_updater.h"
#include <images/BitmapDatabase.hpp>
#include <stdio.h>
#endif

DevUpdatePresenter::DevUpdatePresenter(DevUpdateView& v)
    : view(v)
{
}

void DevUpdatePresenter::activate()
{
#ifndef SIMULATOR
    if (dev_upd_import_is_active()) {
        view.showImage(BITMAP_UPDATE_120_00_ID);
        view.updateInfo("Service update found.\nChecking directories...");
        view.showButtons(false, false);
    } else if (dev_upd_export_is_active()) {
        view.showImage(BITMAP_LOG_120_00_ID);
        view.updateInfo("Checking for logs to export...");
        view.showButtons(false, false);
    } else if (dev_upd_get_state() == DEV_UPD_FAIL_RESOURCES) {
        view.showImage(BITMAP_BOOTLOAD_120_00_ID);
        view.updateInfo("ERROR: .hex not found in GEN_UPDATE");
        view.showButtons(false, true);
    } else {
        view.showImage(BITMAP_BOOTLOAD_120_00_ID);
        char info[128];
        snprintf(info, sizeof info, "New generator firmware available:\n\n%s\n\nContinue?",
                 gen_upd_get_hex_name());
        view.updateInfo(info);
        view.showButtons(true, true);
    }
#endif
}

void DevUpdatePresenter::deactivate()
{
}

void DevUpdatePresenter::okClicked1()
{
#ifndef SIMULATOR
    if (dev_upd_import_is_active()) {
        view.updateInfo("Importing...");
        view.showButtons(false, false);
        dev_upd_import_confirm();
    } else if (dev_upd_export_is_active()) {
        view.updateInfo("Exporting...");
        view.showButtons(false, false);
        dev_upd_export_confirm();
    } else {
        view.showButtons(false, false);
        if (dev_upd_get_state() == DEV_UPD_SUCCESS)
            gen_upd_confirm_reset();
        else
            gen_upd_start();
    }
#endif
}

void DevUpdatePresenter::cancelClicked()
{
#ifndef SIMULATOR
    if (dev_upd_import_is_active()) {
        view.updateInfo("Skipping...");
        view.showButtons(false, false);
        dev_upd_import_skip();
        return;
    } else if (dev_upd_export_is_active()) {
        view.updateInfo("Skipping...");
        view.showButtons(false, false);
        dev_upd_export_skip();
        return;
    }
    dev_upd_cancel();
    /* No explicit navigation — tick() navigates to Starting when DevUpdate has nothing active */
#endif
}

void DevUpdatePresenter::onDevUpdTick(uint8_t progress, int state, const char* msg)
{
#ifndef SIMULATOR
    char display[96];

    switch ((dev_upd_state_t)state)
    {
    case DEV_UPD_IMPORT_CONFIRM: {
        char prompt[96];
        snprintf(prompt, sizeof prompt, "Import new service configuration:\n\n%s\n\nContinue?", msg);
        view.updateInfo(prompt);
        view.showButtons(true, true);
        break;
    }
    case DEV_UPD_EXPORT_CONFIRM:
        view.updateInfo(msg);
        view.showButtons(true, true);
        break;

    case DEV_UPD_EXPORT_DONE:
        view.updateInfo(msg);
        view.showButtons(true, false);
        break;

    case DEV_UPD_IN_PROGRESS:
        snprintf(display, sizeof display, "%s  %u%%", msg, (unsigned)progress);
        view.updateInfo(display);
        view.showButtons(false, false);
        break;

    case DEV_UPD_SUCCESS:
        view.updateInfo("Programming complete!\n\nRemove USB drive.\nPress confirm to restart.");
        view.showButtons(true, false);
        break;

    case DEV_UPD_FAIL_RESOURCES:
    case DEV_UPD_FAIL_COMM:
    case DEV_UPD_FAIL_FLASH:
        view.updateInfo(msg);
        view.showButtons(false, true);
        break;

    default:
        break;
    }
#endif
}
