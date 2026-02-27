#ifndef SELECT_VIEW_HPP
#define SELECT_VIEW_HPP

#include <gui_generated/select_screen/SelectViewBase.hpp>
#include <gui/select_screen/SelectPresenter.hpp>

class SelectView : public SelectViewBase
{
public:
    SelectView();
    virtual ~SelectView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SELECT_VIEW_HPP
