#ifndef START_VIEW_HPP
#define START_VIEW_HPP

#include <gui_generated/start_screen/StartViewBase.hpp>
#include <gui/start_screen/StartPresenter.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>

class StartView : public StartViewBase
{
public:
    StartView();
    virtual ~StartView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    
protected:
  
private:
    TextureMapper textureMapperImage3D;
    TextureMapper textureMapperImage2D;

    float xAngle3D;
    float yAngle3D;
    float zAngle3D;

    float deltaXangle3D;
    float deltaYangle3D;
    float deltaZangle3D;

    float zAngle2D;

    float deltaZangle2D;
    int a;
};

#endif // START_VIEW_HPP
