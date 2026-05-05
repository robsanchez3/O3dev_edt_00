#ifndef GENUPDATEPRESENTER_HPP
#define GENUPDATEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GenUpdateView;

class GenUpdatePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GenUpdatePresenter(GenUpdateView& v);

    virtual void activate();
    virtual void deactivate();
    virtual ~GenUpdatePresenter() {}

    /* Returns SID_GEN_UPDATE so Model::tick() can identify the active screen. */
    virtual uint16_t getVisibleScreen() { return SID_GEN_UPDATE; }

    /* Called every Model::tick() while this screen is active. */
    virtual void onGenUpdTick(uint8_t progress, int state, const char* msg);

    /* Button callbacks wired from GenUpdateViewBase. */
    void cancelClicked();
    void okClicked1();

private:
    GenUpdatePresenter();

    GenUpdateView& view;
};

#endif // GENUPDATEPRESENTER_HPP
