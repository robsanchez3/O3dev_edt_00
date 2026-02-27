#include <gui/i2c_screen/I2CView.hpp>
#include <gui/i2c_screen/I2CPresenter.hpp>

I2CPresenter::I2CPresenter(I2CView& v)
    : view(v)
{
}

void I2CPresenter::activate()
{

}

void I2CPresenter::deactivate()
{

}

void I2CPresenter::ReadTemperatureOnce()
{
//  model->ReadTemperatureSensor();
}

void I2CPresenter::GetModelTemperature()  
{
  //view.setTemperature(model ->getDevTemperature());
}
