#include <gui/containers/sl_positionContainer.hpp>

sl_positionContainer::sl_positionContainer()
{

}

void sl_positionContainer::initialize()
{
    sl_positionContainerBase::initialize();
}

void sl_positionContainer::setItem(Unicode::UnicodeChar *value)
{
	Unicode::snprintf(textAreaBuffer, sizeof(textArea), "%s", value);
	textArea.invalidate();
}
