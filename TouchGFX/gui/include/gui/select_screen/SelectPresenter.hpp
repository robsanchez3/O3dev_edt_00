#ifndef SELECT_PRESENTER_HPP
#define SELECT_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectView;

class SelectPresenter : public Presenter, public ModelListener
{
public:
    SelectPresenter(SelectView& v);

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

    virtual ~SelectPresenter() {};

private:
    SelectPresenter();

    SelectView& view;
};


#endif // SELECT_PRESENTER_HPP
