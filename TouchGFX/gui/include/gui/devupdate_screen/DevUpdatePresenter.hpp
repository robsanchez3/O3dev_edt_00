#ifndef DEVUPDATEPRESENTER_HPP
#define DEVUPDATEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DevUpdateView;

class DevUpdatePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DevUpdatePresenter(DevUpdateView& v);

    virtual void activate();
    virtual void deactivate();
    virtual ~DevUpdatePresenter() {}

    virtual uint16_t getVisibleScreen() { return SID_DEV_UPDATE; }

    virtual void onGenUpdTick(uint8_t progress, int state, const char* msg);

    void cancelClicked();
    void okClicked1();

private:
    DevUpdatePresenter();

    DevUpdateView& view;
};

#endif // DEVUPDATEPRESENTER_HPP
