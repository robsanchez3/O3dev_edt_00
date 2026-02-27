#include <gui/start_screen/StartView.hpp>
#include <BitmapDatabase.hpp>

StartView::StartView() :
    xAngle3D(0.0f),
    yAngle3D(0.0f),
    zAngle3D(0.0f),
    deltaXangle3D(0.032f),
    deltaYangle3D(0.029f),
    deltaZangle3D(0.032f),
    zAngle2D(0.0f),
    deltaZangle2D(0.019f),
	a(0)
{
}

void StartView::setupScreen()
{  
  textureMapperImage3D.setBitmap(Bitmap(BITMAP_SELOGO320_ID));
  
  int borderWidth = 60;
  int borderHeight = 200;
  
  int imageWidth = textureMapperImage3D.getBitmap().getWidth();
  int imageHeight = textureMapperImage3D.getBitmap().getHeight();
  
  textureMapperImage3D.setXY(180, -40);
  
  textureMapperImage3D.setWidth(imageWidth + borderWidth * 2);
  textureMapperImage3D.setHeight(imageHeight + borderHeight * 2);
  
  textureMapperImage3D.setBitmapPosition(borderWidth, borderHeight);
  
  textureMapperImage3D.setCameraDistance(300.0f);
  
  textureMapperImage3D.setOrigo(textureMapperImage3D.getBitmapPositionX() + (imageWidth / 2), textureMapperImage3D.getBitmapPositionY() + (imageHeight / 2), textureMapperImage3D.getCameraDistance());
  
  textureMapperImage3D.setCamera(textureMapperImage3D.getBitmapPositionX() + (imageWidth / 2), textureMapperImage3D.getBitmapPositionY() + (imageHeight / 2));
  
  textureMapperImage3D.setRenderingAlgorithm(TextureMapper::NEAREST_NEIGHBOR);
    
  add(textureMapperImage3D); 
}

void StartView::tearDownScreen()
{
    StartViewBase::tearDownScreen();
}

void StartView::handleTickEvent()
{
	/**************************** EDT_Logo Animation **************************/
	if (a < 196) {
		a++;
		xAngle3D += deltaXangle3D;
		yAngle3D += deltaYangle3D;
		zAngle3D += deltaZangle3D;

		textureMapperImage3D.updateAngles(xAngle3D, 0.0F, zAngle3D);
	}
	else if (a == 196) {
		textureMapperImage3D.updateAngles(0.0F, 0.0F, 0.0F);
	}
}

