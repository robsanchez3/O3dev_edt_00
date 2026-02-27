#include <gui/containers/sw_therapyContainer.hpp>

sw_therapyContainer::sw_therapyContainer()
{

}

void sw_therapyContainer::initialize()
{
    sw_therapyContainerBase::initialize();
}

void sw_therapyContainer::setItem(Unicode::UnicodeChar *value, Bitmap icon)
{
	printf("SetItem original value:%s icon:%d...\n", (char*)value, (int)icon);
	image.setBitmap(icon);
	image.setWidthHeight(90, 90); // force scale bitmap
	image.invalidate();
	Unicode::snprintf(textAreaBuffer, sizeof(textArea), "%s", value);
	textArea.invalidate();
}
void sw_therapyContainer::setItem(Bitmap icon)
{
	printf("SetItem overloaded  icon:%d...\n", (int)icon);
	image.setBitmap(icon);
	image.setWidthHeight(90, 90); // force scale bitmap
	image.invalidate();
}
