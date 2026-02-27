#ifndef HIDESCREENPRESENTER_HPP
#define HIDESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class HideScreenView;

class HideScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    HideScreenPresenter(HideScreenView& v);

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

    virtual ~HideScreenPresenter() {};

private:
    HideScreenPresenter();

    HideScreenView& view;
};

#endif // HIDESCREENPRESENTER_HPP
