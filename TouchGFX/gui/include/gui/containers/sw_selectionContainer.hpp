#ifndef SW_SELECTIONCONTAINER_HPP
#define SW_SELECTIONCONTAINER_HPP

#include <gui_generated/containers/sw_selectionContainerBase.hpp>

class sw_selectionContainer : public sw_selectionContainerBase
{
public:
    sw_selectionContainer();
    virtual ~sw_selectionContainer() {}

    virtual void initialize();
    virtual void setItem(Unicode::UnicodeChar *value);
    virtual void setTextWidth(int16_t width);
    virtual void setBackWidth(int16_t width);
    int16_t getTextWidth();
protected:
};

#endif // SW_SELECTIONCONTAINER_HPP
