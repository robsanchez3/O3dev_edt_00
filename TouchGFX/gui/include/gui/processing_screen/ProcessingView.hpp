#ifndef PROCESSINGVIEW_HPP
#define PROCESSINGVIEW_HPP

#include <gui_generated/processing_screen/ProcessingViewBase.hpp>
#include <gui/processing_screen/ProcessingPresenter.hpp>

class ProcessingView : public ProcessingViewBase
{
public:
    ProcessingView();
    virtual ~ProcessingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void cancelClicked();
    void okClicked();
    void setupProcessingScreen();
    void setupCalibratePeriod();
    void setupLoadParameters();
    void setupSaveParameters();
    void setupProcessEnd();
    void initTherapyContext(THERAPY_CTX *ctx);

protected:
    int tickCount = 0;
    int progressCount = 0;
    THERAPY_CTX *therapyCtx = NULL;
};

#endif // PROCESSINGVIEW_HPP
