#ifndef CONFIGMENUVIEW_HPP
#define CONFIGMENUVIEW_HPP

#include <gui_generated/configmenu_screen/ConfigMenuViewBase.hpp>
#include <gui/configmenu_screen/ConfigMenuPresenter.hpp>

class ConfigMenuView : public ConfigMenuViewBase
{
public:
    ConfigMenuView();
    virtual ~ConfigMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void sl_positionUpdateItem(sl_positionContainer& item, int16_t itemIndex);
    virtual void sl_position_1UpdateItem(sl_positionContainer& item, int16_t itemIndex);
    virtual void sw_therapiesUpdateItem(sw_therapyContainer& item, int16_t itemIndex);

    Bitmap selectTherapyIcon(int16_t itemIndex);
    void okClicked();
    void clearClicked();
    void buildMainMenu();

protected:
    // Callback which is executed when a item in the scroll list is selected.
    // The parameter itemSelected is the selected item.
    Callback<ConfigMenuView, int16_t> sl_positionItemSelectedCallback;
    void sl_positionItemSelectedHandler(int16_t itemSelected);
    Callback<ConfigMenuView, int16_t> sl_position_1ItemSelectedCallback;
    void sl_position_1ItemSelectedHandler(int16_t itemSelected);

    // Callback which is executed when a item in the scroll list is pressed.
    // The parameter itemPressed is the selected item.
    Callback<ConfigMenuView, int16_t> sl_positionItemPressedCallback;
    void sl_positionItemPressedHandler(int16_t itemPressed);
    Callback<ConfigMenuView, int16_t> sl_position_1ItemPressedCallback;
    void sl_position_1ItemPressedHandler(int16_t itemPressed);

private:
    int8_t menuMap[MAX_DEV_THERAPIES + 1];

};

#endif // CONFIGMENUVIEW_HPP
