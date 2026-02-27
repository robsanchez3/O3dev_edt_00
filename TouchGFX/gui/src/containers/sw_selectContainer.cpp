#include <gui/containers/sw_selectContainer.hpp>


sw_selectContainer::sw_selectContainer()
{

}

void sw_selectContainer::initialize()
{
    sw_selectContainerBase::initialize();
}

void sw_selectContainer::setItem(Unicode::UnicodeChar *value)
{
	Unicode::snprintf(textAreaBuffer, sizeof(textArea), "%s", value);
	textArea.invalidate();
}

void sw_selectContainer::setTextWidth(int16_t width)
{
	textArea.setWidth(width);
}

int16_t sw_selectContainer::getTextWidth()
{
	return textArea.getWidth();
}

