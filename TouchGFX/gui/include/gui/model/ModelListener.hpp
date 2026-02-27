#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

enum SCREEN_ID {SID_NON, SID_ERROR, SID_MAIN_MENU, SID_STARTING, SID_RUNNING, SID_END, SID_WASHING, SID_SYRINGE_FILL, SID_ADJUSTING, SID_CALIBRATION};

/**
 * ModelListener is the interface through which the Model can inform the currently
 * active presenter of events. All presenters should derive from this class.
 * It also provides a model pointer for the presenter to interact with the Model.
 *
 * The bind function is called automatically.
 *
 * Add the virtual functions Model should be able to call on the active presenter to this class.
 */
class ModelListener
{
public:
    ModelListener() : model(0) {}

    virtual ~ModelListener() {}

    virtual void RS232CharReceived(uint8_t *data , uint16_t len) {}
    virtual void CANReceived(uint8_t *data) {}
    virtual void GetModelTemperature() {}
    virtual void PopUpWindowGPIO1() {}
    virtual void ShowGPIO_0_State() {}
    virtual uint16_t getVisibleScreen() {return SID_NON;}
    virtual void updateProgressRange(int min, int max){}
    virtual void onPinValidated(bool success) {}

#ifndef SIMULATOR
    virtual void PushCanToView(uint8_t *candata , uint16_t Size){}
#endif    



    /**
     * Sets the model pointer to point to the Model object. Called automatically
     * when switching screen.
     */
    void bind(Model* m)
    {
        model = m;
    }
protected:
    Model* model;
};

#endif /* MODELLISTENER_HPP */
