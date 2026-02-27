#ifndef START_PRESENTER_HPP
#define START_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class StartView;

class StartPresenter : public Presenter, public ModelListener
{
public:
    StartPresenter(StartView& v);

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

    virtual ~StartPresenter() {};

private:
    StartPresenter();

    StartView& view;
};


#endif // START_PRESENTER_HPP
