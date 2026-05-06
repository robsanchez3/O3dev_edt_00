#include <gui/genupdate_screen/GenUpdateView.hpp>
#include <gui/genupdate_screen/GenUpdatePresenter.hpp>

GenUpdateView::GenUpdateView() {}

void GenUpdateView::setupScreen()    { GenUpdateViewBase::setupScreen(); }
void GenUpdateView::tearDownScreen() { GenUpdateViewBase::tearDownScreen(); }
void GenUpdateView::updateInfo(const char*) {}
void GenUpdateView::showButtons(bool, bool) {}
void GenUpdateView::cancelClicked()  {}
void GenUpdateView::okClicked1()     {}
