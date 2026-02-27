#ifndef OZTDPRESENTER_HPP
#define OZTDPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class OZTDView;

class OZTDPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    OZTDPresenter(OZTDView& v);

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

    virtual ~OZTDPresenter() {};

    void StartGeneration(void);
    void StopGeneration(void);
    void UpdateTargetFlow(int);
    void FlowSliderValueChanged(int);
private:
    OZTDPresenter();

    OZTDView& view;
};

#endif // OZTDPRESENTER_HPP
