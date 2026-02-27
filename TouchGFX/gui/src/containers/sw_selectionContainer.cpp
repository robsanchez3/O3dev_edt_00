#include <gui/containers/sw_selectionContainer.hpp>

sw_selectionContainer::sw_selectionContainer()
{

}

void sw_selectionContainer::initialize()
{
    sw_selectionContainerBase::initialize();
}

void sw_selectionContainer::setItem(Unicode::UnicodeChar *value)
{
//	printf("sw_selectionContainer::setItem... value: %s\n", value);

	Unicode::snprintf(textAreaBuffer, sizeof(textArea), "%s", value);
	textArea.invalidate();
}

void sw_selectionContainer::setTextWidth(int16_t width)
{
	textArea.setWidth(width);
}
void sw_selectionContainer::setBackWidth(int16_t width)
{
	box1.setWidth(width);
}

int16_t sw_selectionContainer::getTextWidth()
{
	return textArea.getWidth();
}
