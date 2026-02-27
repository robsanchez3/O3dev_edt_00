#ifndef SW_THERAPYCONTAINER_HPP
#define SW_THERAPYCONTAINER_HPP

#include <gui_generated/containers/sw_therapyContainerBase.hpp>

class sw_therapyContainer : public sw_therapyContainerBase
{
public:
    sw_therapyContainer();
    virtual ~sw_therapyContainer() {}

    virtual void initialize();
    virtual void setItem(Unicode::UnicodeChar *value, Bitmap icon);
    virtual void setItem(Bitmap icon);

protected:
};

#endif // SW_THERAPYCONTAINER_HPP
