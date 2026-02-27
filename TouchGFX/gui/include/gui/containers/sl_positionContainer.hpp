#ifndef SL_POSITIONCONTAINER_HPP
#define SL_POSITIONCONTAINER_HPP

#include <gui_generated/containers/sl_positionContainerBase.hpp>

class sl_positionContainer : public sl_positionContainerBase
{
public:
    sl_positionContainer();
    virtual ~sl_positionContainer() {}

    virtual void initialize();
    virtual void setItem(Unicode::UnicodeChar *value);
protected:
};

#endif // SL_POSITIONCONTAINER_HPP
