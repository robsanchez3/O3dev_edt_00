#ifndef MAINMENUVIEW_HPP
#define MAINMENUVIEW_HPP

#include <gui_generated/mainmenu_screen/MainMenuViewBase.hpp>
#include <gui/mainmenu_screen/MainMenuPresenter.hpp>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT	480

class MainMenuView : public MainMenuViewBase
{
public:
    MainMenuView();
    virtual ~MainMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setupMenu();
    void hideMenuButtons();
//    void configButtonPosition(int8_t position, int8_t maxButtons, touchgfx::Button *button);
    void configButton(int8_t position, int8_t maxButtons, int8_t shiftType, touchgfx::Button *button, uint16_t bitmapPressBig, uint16_t bitmapReleaseBig, uint16_t bitmapPressSmall, uint16_t bitmapReleaseSmall);
    void updateSyringeButonsAlpha();
    void updateAlphaDependingOnVaccum();

    void syringeStart();
    void continuousStart();
    void insufflationStart();
    void manualStart();
    void dentalStart();
    void doseStart();
    void vacuumStart();
    void vacuumTimeStart();
    void vacuumPressureStart();
    void bagClosedStart();
    void openBagStart();
    void bagStart();
    void insufflation_v_start();
    void insufflation_r_start();
    void syringeManualStart();
    void syringeAutoStart();
    void salineStart();



protected:
};

#endif // MAINMENUVIEW_HPP
