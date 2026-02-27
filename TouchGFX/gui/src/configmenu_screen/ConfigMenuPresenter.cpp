#include <gui/configmenu_screen/ConfigMenuView.hpp>
#include <gui/configmenu_screen/ConfigMenuPresenter.hpp>

ConfigMenuPresenter::ConfigMenuPresenter(ConfigMenuView& v)
    : view(v)
{

}

void ConfigMenuPresenter::activate()
{

}

void ConfigMenuPresenter::deactivate()
{

}

int8_t * ConfigMenuPresenter::getDeviceConfig()
{
	return model->getDeviceConfig();
}
