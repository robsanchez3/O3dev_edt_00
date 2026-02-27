#include <gui/configmenu_screen/ConfigMenuView.hpp>

#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/TypedText.hpp>

#include <BitmapDatabase.hpp>

//sw_therapyContainer& contLog;

//bool firstInit = true;
int16_t userSelection = -1;


ConfigMenuView::ConfigMenuView() :
//sl_positionItemSelectedCallback(this, &ConfigMenuView::sl_positionItemSelectedHandler),
//sl_position_1ItemSelectedCallback(this, &ConfigMenuView::sl_positionItemSelectedHandler),
  sl_positionItemPressedCallback(this, &ConfigMenuView::sl_positionItemPressedHandler),
  sl_position_1ItemPressedCallback(this, &ConfigMenuView::sl_position_1ItemPressedHandler)
{

}

void ConfigMenuView::setupScreen()
{
	int8_t i = 0;
//	firstInit = true;
    userSelection=-1;

    ConfigMenuViewBase::setupScreen();
    //	sl_position.setItemSelectedCallback(sl_positionItemSelectedCallback);
    //	sl_position_1.setItemSelectedCallback(sl_positionItemSelectedCallback);
	sl_position.setItemPressedCallback(sl_positionItemPressedCallback);
	sl_position_1.setItemPressedCallback(sl_position_1ItemPressedCallback);

	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
		menuMap[i] = -1;
	}

	rb_shift_0.setSelected(true);
  //  printf("Init sw_therapies...\n");
  //  sw_therapies.setNumberOfItems(12);
  //  sw_therapies.initialize();

 //   printf("Init sl_position...\n");
 //   sl_position.setNumberOfItems(7);
  //  sl_position.initialize();
  //  printf("Init sl_position...\n");
  //  sl_position_1.setNumberOfItems(7);
  //  sl_position_1.initialize();


//    firstInit = false;
}

void ConfigMenuView::tearDownScreen()
{
    ConfigMenuViewBase::tearDownScreen();
}

void ConfigMenuView::sl_positionUpdateItem(sl_positionContainer& item, int16_t itemIndex)
{
	printf("ConfigMenuView::sl_positionUpdateItem: itemIndex %d\n", itemIndex);

	Unicode::UnicodeChar buff[20];
	Unicode::itoa(itemIndex + 1, buff, 20, 10);

	if((itemIndex + 1) == 7)
	{
		Unicode::snprintf(buff, sizeof(buff), " ");
	}
	item.setItem(buff);
}

void ConfigMenuView::sl_position_1UpdateItem(sl_positionContainer& item, int16_t itemIndex)
{
	printf("ConfigMenuView::sl_position_1UpdateItem: itemIndex %d\n", itemIndex);

	Unicode::UnicodeChar buff[20];
	Unicode::itoa(itemIndex + 7, buff, 20, 10);

	if((itemIndex + 1) == 7)
	{
		Unicode::snprintf(buff, sizeof(buff), " ");
	}
    item.setItem(buff);
}
/*
void ConfigMenuView::test()
{
	for (int i = 0; i < sw_therapies.getNumberOfItems(); ++i) {

		sw_therapies.itemChanged(i);
	}
}
*/
void ConfigMenuView::sw_therapiesUpdateItem(sw_therapyContainer& item, int16_t itemIndex)
{

	Unicode::UnicodeChar buff[20];
    printf("Sw_therapiesUpdateItem: %d of %d\n", itemIndex, sw_therapies.getNumberOfItems());


	// in the case of first initialization
	/*
	if(itemIndex < 3)
	{
		Unicode::itoa(itemIndex + 1, buff, 20, 10);
		menuMap[itemIndex] = itemIndex + 1;
	}
	else
	{
		Unicode::snprintf(buff, sizeof(buff), " ");
		menuMap[itemIndex] = -1;
	}
	if(firstInit)
	{
	    item.setItem(buff, selectTherapyIcon(itemIndex));
	    printf("a Sw_therapiesUpdateItem: itemIndex %d - userSel: %d - menuMap[itemIndex]: %d\n", itemIndex, userSelection, menuMap[itemIndex]);
	}
	else
	{
	    item.setItem(selectTherapyIcon(itemIndex));
	}
	*/
	// in the case of user action
	if(userSelection > -1)
	{
		if(userSelection == (sw_therapies.getNumberOfItems() + 1))
		{
			Unicode::snprintf(buff, sizeof(buff), " ");
			menuMap[itemIndex] = -1;
		}
		else
		{
//			Unicode::itoa(userSelection + (userSelection > 6 ? 0 : 1), buff, 20, 10);
	//		menuMap[itemIndex] = userSelection + (userSelection > 6 ? 0 : 1);
			Unicode::itoa(userSelection + 1, buff, 20, 10);
			menuMap[itemIndex] = userSelection + 1;
		}
	    item.setItem(buff, selectTherapyIcon(itemIndex));

	    printf("User  - itemIndex %d - userSel: %d - menuMap[%d]: %d\n", itemIndex, userSelection, itemIndex, menuMap[itemIndex]);
	    printf("[0]:%d, [1]:%d, [2]:%d, [3]:%d, [4]:%d, [5]:%d, [6]:%d, [7]:%d, [8]:%d, [9]:%d, [10]:%d, [11]:%d, [12]:%d, [13]:%d, [14]:%d, [15]:%d, [16]:%d, [17]:%d, [18]:%d, [19]:%d, [20]:%d\n", menuMap[0], menuMap[1], menuMap[2], menuMap[3], menuMap[4], menuMap[5], menuMap[6], menuMap[7], menuMap[8], menuMap[9], menuMap[10], menuMap[11], menuMap[12], menuMap[13], menuMap[14], menuMap[15], menuMap[16], menuMap[17], menuMap[18], menuMap[19], menuMap[20]);

		userSelection = -1;
	}
	else
	{
	    item.setItem(selectTherapyIcon(itemIndex));
	    printf("Scroll- itemIndex %d - userSel: %d - menuMap[%d]: %d\n", itemIndex, userSelection, itemIndex, menuMap[itemIndex]);
	    printf("[0]:%d, [1]:%d, [2]:%d, [3]:%d, [4]:%d, [5]:%d, [6]:%d, [7]:%d, [8]:%d, [9]:%d, [10]:%d, [11]:%d, [12]:%d, [13]:%d, [14]:%d, [15]:%d, [16]:%d, [17]:%d, [18]:%d, [19]:%d, [20]:%d\n", menuMap[0], menuMap[1], menuMap[2], menuMap[3], menuMap[4], menuMap[5], menuMap[6], menuMap[7], menuMap[8], menuMap[9], menuMap[10], menuMap[11], menuMap[12], menuMap[13], menuMap[14], menuMap[15], menuMap[16], menuMap[17], menuMap[18], menuMap[19], menuMap[20]);
	}
}

Bitmap ConfigMenuView::selectTherapyIcon(int16_t itemIndex)
{
	switch(itemIndex)
	{
	case SYRINGE_MODE:
		return BITMAP_JERINGA_01_ID;
		break;
	case SYRINGE_AUTO_MODE:
		return BITMAP_JERINGAAUTO_01_ID;
		break;
	case SYRINGE_MANUAL_MODE:
		return BITMAP_JERINGAMANUAL_01_ID;
		break;
	case CONTINUOUS_MODE:
		return BITMAP_CONTINUO_01_ID;
		break;
	case INSUFFLATION_MODE:
		return BITMAP_INSUFLACION_01_ID;
		break;
	case INSUFFLATION_R_MODE:
		return BITMAP_RECTALINSUFFLATION_01_ID;
		break;
	case INSUFFLATION_V_MODE:
		return BITMAP_VAGINALINSUFFLATION_01_ID;
		break;
	case MANUAL_MODE:
		return BITMAP_MANUAL_01_ID;
		break;
	case DENTAL_MODE:
		return BITMAP_GUN_01_ID;
		break;
	case VACUUM_MODE:
		return BITMAP_VACUUM_01_ID;
		break;
	case VACUUM_TIME_MODE:
		return BITMAP_VACUUM_T_01_ID;
		break;
	case VACUUM_PRESSURE_MODE:
		return BITMAP_VACUUM_P_01_ID;
		break;
	case BAG_MODE:
		return BITMAP_BAG_01_ID;
		break;
	case CLOSED_BAG_MODE:
		return BITMAP_CLOSEDBAG_01_ID;
		break;
	case OPEN_BAG_MODE:
		return BITMAP_OPENBAG_01_ID;
		break;
	case DOSE_MODE:
		return BITMAP_DOSE_01_ID;
		break;
	case SALINE_MODE:
		return BITMAP_SALINE_01_ID;
		break;
	default:
		return BITMAP_QUESTION_MARK_01_ID;
	}
}

void ConfigMenuView::sl_positionItemSelectedHandler(int16_t itemSelected)
{
	//printf("sl_position selected handler call back: %d\n", itemSelected);
}

void ConfigMenuView::sl_position_1ItemSelectedHandler(int16_t itemSelected)
{
	//printf("sl_position_1 selected handler call back: %d\n", itemSelected);
}

void ConfigMenuView::sl_positionItemPressedHandler(int16_t itemPressed)
{
	//printf("sl_position pressed handler call back: %d\n", itemPressed);

    userSelection = itemPressed;
    sw_therapies.itemChanged(sw_therapies.getSelectedItem()); // force sw_therapies update

//	Unicode::snprintf(textArea1Buffer, sizeof(textArea1), "%d", itemPressed);
//  textArea1.invalidate();
}
void ConfigMenuView::sl_position_1ItemPressedHandler(int16_t itemPressed)
{
//	printf("sl_position_1 pressed handler call back: %d\n", itemPressed);

    userSelection = itemPressed + 6;
    sw_therapies.itemChanged(sw_therapies.getSelectedItem()); // force sw_therapies update

//	Unicode::snprintf(textArea1Buffer, sizeof(textArea1), "%d", itemPressed);
//  textArea1.invalidate();
}
void ConfigMenuView::clearClicked()
{
    userSelection = sw_therapies.getNumberOfItems() + 1;
    sw_therapies.itemChanged(sw_therapies.getSelectedItem()); // force sw_therapies update
}

void ConfigMenuView::okClicked()
{
//	printf("Edition end. Reconfiguring menu...\n");
	application().gotoMainMenuScreenNoTransition();

	buildMainMenu();

}

void ConfigMenuView::buildMainMenu()
{
	int8_t i;               // general counter
	int8_t icon;            // icon counter
	int8_t order;           // order counter
	int8_t menuElement = 0; // menu element counter
	int8_t end_shift = MMF_END;
	int8_t *devConfig = presenter->getDeviceConfig();


	if     (rb_shift_0.getSelected())   end_shift = MMF_END;
	else if(rb_shift_0_5.getSelected()) end_shift = MMF_END_SHIFT_0_5;
	else if(rb_shift_1.getSelected())   end_shift = MMF_END_SHIFT_1;
	else if(rb_shift_2.getSelected())   end_shift = MMF_END_SHIFT_2;

	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
//		printf("Dev config before %d: %d\n", i, *(devConfig + i));
	}

	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
//		printf("Menu map %d: %d\n", i, menuMap[i]);
		*(devConfig + i) = end_shift;
	}
/*
	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
		if((menuMap[i] - 1) == SYRINGE_AUTO_MODE)
		{
			*(devConfig + (menuElement++)) = SYRINGE_AUTO_MODE;
		}
		else if((menuMap[i] - 1) == CONTINUOUS_MODE)
		{
			*(devConfig + (menuElement++)) = CONTINUOUS_MODE;
		}
		else if((menuMap[i] - 1) == TT_INSUFFLATION_R)
		{
			*(devConfig + (menuElement++)) = TT_RINSUFFLATION_R;
		}
		else if((menuMap[i] - 1) == TT_MANUAL)
		{
			*(devConfig + (menuElement++)) = TT_MANUAL;
		}
		else if((menuMap[i] - 1) == DENTAL_MODE)
		{
			*(devConfig + (menuElement++)) = DENTAL_MODE;
		}
	}
*/
	for(order = 1; order< (MAX_DEV_THERAPIES + 1); order++ )
	{
		for(icon = 0; icon< (MAX_DEV_THERAPIES + 1); icon++ )
		{
			if((menuMap[icon]) == order){
				*(devConfig + (menuElement++)) = icon;
				break;
			}
		}
	}

	/*
	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
		if((menuMap[i] - 1) >= 0)
		{
			*(devConfig + (menuElement++)) = (menuMap[i] - 1);
		}

	}
	*/
	for(i = 0; i< (MAX_DEV_THERAPIES + 1); i++ )
	{
//		printf("Dev config after %d: %d\n", i, *(devConfig + i));
	}

}
















