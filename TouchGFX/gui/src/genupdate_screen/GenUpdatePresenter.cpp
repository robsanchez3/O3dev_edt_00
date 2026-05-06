#include <gui/genupdate_screen/GenUpdateView.hpp>
#include <gui/genupdate_screen/GenUpdatePresenter.hpp>

GenUpdatePresenter::GenUpdatePresenter(GenUpdateView& v)
    : view(v)
{
}

void GenUpdatePresenter::activate()   {}
void GenUpdatePresenter::deactivate() {}
void GenUpdatePresenter::okClicked1() {}
void GenUpdatePresenter::cancelClicked() {}
void GenUpdatePresenter::onGenUpdTick(uint8_t, int, const char*) {}
