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

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~DevUpdatePresenter() {}

private:
    DevUpdatePresenter();

    DevUpdateView& view;
};

#endif // DEVUPDATEPRESENTER_HPP
