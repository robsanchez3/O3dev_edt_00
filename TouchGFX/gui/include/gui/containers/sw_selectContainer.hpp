#ifndef SW_SELECTCONTAINER_HPP
#define SW_SELECTCONTAINER_HPP

#include <gui_generated/containers/sw_selectContainerBase.hpp>

class sw_selectContainer : public sw_selectContainerBase
{
public:
    sw_selectContainer();
    virtual ~sw_selectContainer() {}

    virtual void initialize();
    virtual void setItem(Unicode::UnicodeChar *value);
    virtual void setTextWidth(int16_t width);
    int16_t getTextWidth();
protected:
};

#endif // SW_SELECTCONTAINER_HPP
