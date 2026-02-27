#ifndef PROCESSINGPRESENTER_HPP
#define PROCESSINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ProcessingView;

class ProcessingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ProcessingPresenter(ProcessingView& v);

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

    virtual ~ProcessingPresenter() {};

    void setGuiTherapy(int8_t value);
    int8_t getGuiTherapy();
    void periodCalibrationStart();
    void loadParameterStart();
    void saveParameterStart();
    void userCancelled();
    void userOk();
    uint8_t getFsmState();
    uint8_t getCalibrationSucessState();
private:
    ProcessingPresenter();

    ProcessingView& view;
};

#endif // PROCESSINGPRESENTER_HPP
