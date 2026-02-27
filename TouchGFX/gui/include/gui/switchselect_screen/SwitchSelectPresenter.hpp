#ifndef SWITCHSELECTPRESENTER_HPP
#define SWITCHSELECTPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SwitchSelectView;

class SwitchSelectPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SwitchSelectPresenter(SwitchSelectView& v);

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

    virtual ~SwitchSelectPresenter() {};

    void setGuiTherapy(int8_t value);
    void generationModeClicked(bool state);
    bool getGenerationMode();


private:
    SwitchSelectPresenter();

    SwitchSelectView& view;
};

#endif // SWITCHSELECTPRESENTER_HPP
