#ifndef SELECTORPRESENTER_HPP
#define SELECTORPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectorView;

class SelectorPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SelectorPresenter(SelectorView& v);

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

    virtual ~SelectorPresenter() {};

    void EndSelection(void);
    void StartGeneration(void);
    void okClicked(uint8_t targetValueID, uint16_t value);
    void cancelClicked();
    void userCancelled();
    uint32_t getConfiguredTime();
    uint32_t getConfiguredVolume();
    void setTherapyTargetValue(uint8_t targetValueID, uint16_t value);
    void onSelectionAction(uint8_t selectionStep);
    void onSliderAction(uint8_t step, uint8_t targetValueID, uint16_t value);
private:
    SelectorPresenter();

    SelectorView& view;
};

#endif // SELECTORPRESENTER_HPP
