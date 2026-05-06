#include <gui/devupdate_screen/DevUpdateView.hpp>
#include <gui/devupdate_screen/DevUpdatePresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

#ifndef SIMULATOR
#include "../../../../Drivers/O3/IAP/gen_updater.h"
#include <stdio.h>
#endif

DevUpdatePresenter::DevUpdatePresenter(DevUpdateView& v)
    : view(v)
{
}

void DevUpdatePresenter::activate()
{
#ifndef SIMULATOR
    if (gen_upd_get_state() == GEN_UPD_FAIL_RESOURCES) {
        view.updateInfo("ERROR: .hex not found in GEN_UPDATE");
        view.showButtons(false, true);
    } else {
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
    view.showButtons(false, false);
    if (gen_upd_get_state() == GEN_UPD_SUCCESS)
        gen_upd_confirm_reset();
    else
        gen_upd_start();
#endif
}

void DevUpdatePresenter::cancelClicked()
{
#ifndef SIMULATOR
    gen_upd_cancel();
#endif
    static_cast<FrontendApplication*>(Application::getInstance())->gotoStartingScreenNoTransition();
}

void DevUpdatePresenter::onGenUpdTick(uint8_t progress, int state, const char* msg)
{
#ifndef SIMULATOR
    char display[96];

    switch ((gen_upd_state_t)state)
    {
    case GEN_UPD_IN_PROGRESS:
        snprintf(display, sizeof display, "%s  %u%%", msg, (unsigned)progress);
        view.updateInfo(display);
        view.showButtons(false, false);
        break;

    case GEN_UPD_SUCCESS:
        view.updateInfo("Programming complete!\n\nRemove USB drive.\nPress Confirm to restart.");
        view.showButtons(true, false);
        break;

    case GEN_UPD_FAIL_RESOURCES:
    case GEN_UPD_FAIL_COMM:
    case GEN_UPD_FAIL_FLASH:
        view.updateInfo(msg);
        view.showButtons(false, true);
        break;

    default:
        break;
    }
#endif
}
