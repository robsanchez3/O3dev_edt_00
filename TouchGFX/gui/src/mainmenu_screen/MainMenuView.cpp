#include <gui/mainmenu_screen/MainMenuView.hpp>
#include <gui/selector_screen/SelectorView.hpp>
#include <stdio.h>

#include <BitmapDatabase.hpp>

MainMenuView::MainMenuView()
{

}

void MainMenuView::setupScreen()
{
    MainMenuViewBase::setupScreen();

    updateSyringeButonsAlpha();
    updateAlphaDependingOnVaccum();

    setupMenu();
}

void MainMenuView::tearDownScreen()
{
    MainMenuViewBase::tearDownScreen();
}

void MainMenuView::syringeStart()
{
    if(!presenter->isCalibrationErrorDuringStartUp())
    {
		printf("General syringe start (auto).\n");
		presenter->setGuiTherapy(SYRINGE_MODE);
		application().gotoSelectorScreenNoTransition();
    }
    else
    {
    	printf("Calibration error during start up. Syringe therapy not allowed...\n");
    }
}

void MainMenuView::syringeAutoStart()
{
    if(!presenter->isCalibrationErrorDuringStartUp())
    {
		printf("Syringe auto start.\n");
		presenter->setGuiTherapy(SYRINGE_AUTO_MODE);
		application().gotoSelectorScreenNoTransition();
    }
    else
    {
    	printf("Calibration error during start up. Syringe therapy not allowed...\n");
    }
}

void MainMenuView::syringeManualStart()
{
    if(!presenter->isCalibrationErrorDuringStartUp())
    {
		printf("Syringe manual start.\n");
		presenter->setGuiTherapy(SYRINGE_MANUAL_MODE);
		application().gotoSelectorScreenNoTransition();
    }
    else
    {
    	printf("Calibration error during start up. Syringe therapy not allowed...\n");
    }
}

void MainMenuView::continuousStart()
{
	printf("Continuous start.\n");
	presenter->setGuiTherapy(CONTINUOUS_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::manualStart()
{
	printf("Manual start.\n");
	presenter->setGuiTherapy(MANUAL_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::dentalStart()
{
	printf("Dental start.\n");
	presenter->setGuiTherapy(DENTAL_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::doseStart()
{
	printf("Dose start.\n");
	presenter->setGuiTherapy(DOSE_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::vacuumStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("General vacuum start (time).\n");
		presenter->setGuiTherapy(VACUUM_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}
void MainMenuView::vacuumTimeStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("Vacuum time start.\n");
		presenter->setGuiTherapy(VACUUM_TIME_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}
void MainMenuView::vacuumPressureStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("Vacuum pressure start.\n");
		presenter->setGuiTherapy(VACUUM_PRESSURE_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}
void MainMenuView::bagStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("General bag start (closed).\n");
		presenter->setGuiTherapy(BAG_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}

void MainMenuView::bagClosedStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("Closed bag start.\n");
		presenter->setGuiTherapy(CLOSED_BAG_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}

void MainMenuView::openBagStart()
{
	if(presenter->isVaccumAvailable())
	{
		printf("Open bag start.\n");
		presenter->setGuiTherapy(OPEN_BAG_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		printf("Vacuum not available for this device configuration.\n");
	}
}

void MainMenuView::insufflationStart()
{
	printf("General insufflation (rectal) start.\n");
	presenter->setGuiTherapy(INSUFFLATION_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::insufflation_r_start()
{
	printf("Rectal insufflation start.\n");
	presenter->setGuiTherapy(INSUFFLATION_R_MODE);
	application().gotoSelectorScreenNoTransition();
}
void MainMenuView::insufflation_v_start()
{
	printf("Vaginal insufflation start.\n");
	presenter->setGuiTherapy(INSUFFLATION_V_MODE);
	application().gotoSelectorScreenNoTransition();
}

void MainMenuView::salineStart()
{
	printf("Saline start.\n");
	presenter->setGuiTherapy(SALINE_MODE);
	application().gotoSelectorScreenNoTransition();
}


/*
 * Usage instructions
 *
 * Fill up to 12 therapies in "deviceConfig[]" array (at Model.hpp at the moment of writing this note).
 * Use "THERAPY_TYPES" and "MAIN_MENU_FORMATING" enums to fill "deviceConfig[]" (at Model.hpp at the moment of writing this note).
 *
 * After last menu therapy include "MAIN_MENU_FORMATING" enum type to indicate menu end and right shift on last row if needed (the
 * rest of array element values are not relevant).
 *
 * The menu icons will be presented from left to right and top to bottom in accordance with array offset.
 *
 * Bitmaps for every therapy is assigned on "MainMenuView::setupMenu" function. 200x200 and 130x130 pixels resolution needed for each one.
 *
 * Replace "TT_RFU_X" symbols for new therapies in the case it needed
 *
 */

				//#include <ff_gen_drv.h>
				//#include "edt_f7xx_sdcard.h"


void MainMenuView::setupMenu()
{
	int8_t i = 0;
	int8_t maxTherapies = 0;
	int8_t shiftType = 0;
	int8_t *devConfig = presenter->getDeviceConfig();


				//if(EDT_SD_IsDetected() == SD_PRESENT)
				//{
				//	FIL MyFile;     /* File object */
				//	f_open(&MyFile, "EDT_FatFs.TXT", FA_CREATE_ALWAYS | FA_WRITE);
				//}


	// look for number of therapies included
	while(*(devConfig + (maxTherapies++)) >= 0){}
	maxTherapies--;

	// look for shift in the case its needed
	shiftType = *(devConfig + maxTherapies);

	hideMenuButtons();

//	printf("Max therapies: %d, Shift type: %d\n", maxTherapies, shiftType);

//  configure menu
	if(maxTherapies)
	{
		for(i = 0; i < maxTherapies; i++)
		{
			switch(*(devConfig + i))
			{
			case SYRINGE_MODE:
				configButton( i, maxTherapies, shiftType, &bt_syringe,         BITMAP_JERINGA_10_ID,             BITMAP_JERINGA_PRESSED_10_ID,             BITMAP_JERINGA_01_ID,             BITMAP_JERINGA_PRESSED_01_ID);
				break;
			case SYRINGE_AUTO_MODE:
				configButton( i, maxTherapies, shiftType, &bt_syringe_auto,    BITMAP_JERINGAAUTO_10_ID,         BITMAP_JERINGAAUTO_PRESSED_10_ID,         BITMAP_JERINGAAUTO_01_ID,         BITMAP_JERINGAAUTO_PRESSED_01_ID);
				break;
			case SYRINGE_MANUAL_MODE:
				configButton( i, maxTherapies, shiftType, &bt_syringe_manual,  BITMAP_JERINGAMANUAL_10_ID,       BITMAP_JERINGAMANUAL_PRESSED_10_ID,       BITMAP_JERINGAMANUAL_01_ID,       BITMAP_JERINGAMANUAL_PRESSED_01_ID);
				break;
			case CONTINUOUS_MODE:
				configButton( i, maxTherapies, shiftType, &bt_continuous,      BITMAP_CONTINUO_10_ID,            BITMAP_CONTINUO_PRESSED_10_ID,            BITMAP_CONTINUO_01_ID,            BITMAP_CONTINUO_PRESSED_01_ID);
				break;
			case INSUFFLATION_MODE:
				configButton( i, maxTherapies, shiftType, &bt_insuflation,     BITMAP_INSUFLACION_10_ID,         BITMAP_INSUFLACION_PRESSED_10_ID,         BITMAP_INSUFLACION_01_ID,         BITMAP_INSUFLACION_PRESSED_01_ID);
				break;
			case INSUFFLATION_R_MODE:
				configButton( i, maxTherapies, shiftType, &bt_insuflation_r,   BITMAP_RECTALINSUFFLATION_10_ID,  BITMAP_RECTALINSUFFLATION_PRESSED_10_ID,  BITMAP_RECTALINSUFFLATION_01_ID,  BITMAP_RECTALINSUFFLATION_PRESSED_01_ID);
				break;
			case INSUFFLATION_V_MODE:
				configButton( i, maxTherapies, shiftType, &bt_insuflation_v,   BITMAP_VAGINALINSUFFLATION_10_ID, BITMAP_VAGINALINSUFFLATION_PRESSED_10_ID, BITMAP_VAGINALINSUFFLATION_01_ID, BITMAP_VAGINALINSUFFLATION_PRESSED_01_ID);
				break;
			case MANUAL_MODE:
				configButton( i, maxTherapies, shiftType, &bt_manual,          BITMAP_MANUAL_10_ID,              BITMAP_MANUAL_PRESSED_10_ID,              BITMAP_MANUAL_01_ID,              BITMAP_MANUAL_PRESSED_01_ID);
				break;
			case VACUUM_MODE:
				configButton( i, maxTherapies, shiftType, &bt_vacuum,          BITMAP_VACUUM_10_ID,              BITMAP_VACUUM_PRESSED_10_ID,              BITMAP_VACUUM_01_ID,              BITMAP_VACUUM_PRESSED_01_ID);
				break;
			case VACUUM_TIME_MODE:
				configButton( i, maxTherapies, shiftType, &bt_vacuum_time,     BITMAP_VACUUM_T_10_ID,            BITMAP_VACUUM_T_PRESSED_10_ID,            BITMAP_VACUUM_T_01_ID,            BITMAP_VACUUM_T_PRESSED_01_ID);
				break;
			case VACUUM_PRESSURE_MODE:
				configButton( i, maxTherapies, shiftType, &bt_vacuum_pressure, BITMAP_VACUUM_P_10_ID,            BITMAP_VACUUM_P_PRESSED_10_ID,            BITMAP_VACUUM_P_01_ID,            BITMAP_VACUUM_P_PRESSED_01_ID);
				break;
			case BAG_MODE:
				configButton( i, maxTherapies, shiftType, &bt_bag,             BITMAP_BAG_10_ID,                 BITMAP_BAG_PRESSED_10_ID,                 BITMAP_BAG_01_ID,                 BITMAP_BAG_PRESSED_01_ID);
				break;
			case CLOSED_BAG_MODE:
				configButton( i, maxTherapies, shiftType, &bt_closed_bag,      BITMAP_CLOSEDBAG_10_ID,           BITMAP_CLOSEDBAG_PRESSED_10_ID,           BITMAP_CLOSEDBAG_01_ID,           BITMAP_CLOSEDBAG_PRESSED_01_ID);
				break;
			case OPEN_BAG_MODE:
				configButton( i, maxTherapies, shiftType, &bt_open_bag,        BITMAP_OPENBAG_10_ID,             BITMAP_OPENBAG_PRESSED_10_ID,             BITMAP_OPENBAG_01_ID,             BITMAP_OPENBAG_PRESSED_01_ID);
				break;
			case DENTAL_MODE:
				configButton( i, maxTherapies, shiftType, &bt_dental,          BITMAP_GUN_10_ID,                 BITMAP_GUN_PRESSED_10_ID,                 BITMAP_GUN_01_ID,                 BITMAP_GUN_PRESSED_01_ID);
				break;
			case DOSE_MODE:
				configButton( i, maxTherapies, shiftType, &bt_dose,            BITMAP_DOSE_10_ID,                BITMAP_DOSE_PRESSED_10_ID,                BITMAP_DOSE_01_ID,                BITMAP_DOSE_PRESSED_01_ID);
				break;
			case SALINE_MODE:
				configButton( i, maxTherapies, shiftType, &bt_saline,          BITMAP_SALINE_10_ID,              BITMAP_SALINE_PRESSED_10_ID,              BITMAP_SALINE_01_ID,              BITMAP_SALINE_PRESSED_01_ID);
				break;
			case RFU_1_MODE:
				configButton( i, maxTherapies, shiftType, &bt_rfu_1,           BITMAP_QUESTION_MARK_10_ID,       BITMAP_QUESTION_MARK_10_ID,               BITMAP_QUESTION_MARK_01_ID,       BITMAP_QUESTION_MARK_01_ID);
				break;
//			case TT_RFU_2:
//				configButton( i, maxTherapies, shiftType, &bt_rfu_2,           BITMAP_QUESTION_MARK_10_ID,       BITMAP_QUESTION_MARK_10_ID,               BITMAP_QUESTION_MARK_01_ID,       BITMAP_QUESTION_MARK_01_ID);
//				break;
			}
		}
	}
}

void MainMenuView::hideMenuButtons()
{
    bt_vacuum_pressure.setVisible(false);
    bt_rfu_2.setVisible(false);
    bt_rfu_1.setVisible(false);
    bt_dental.setVisible(false);
    bt_saline.setVisible(false);
    bt_vacuum_time.setVisible(false);
    bt_bag.setVisible(false);
    bt_manual.setVisible(false);
    bt_insuflation.setVisible(false);
    bt_continuous.setVisible(false);
    bt_syringe.setVisible(false);
    bt_dose.setVisible(false);
    bt_open_bag.setVisible(false);
    bt_insuflation_v.setVisible(false);
    bt_syringe_manual.setVisible(false);
    bt_insuflation_r.setVisible(false);
    bt_closed_bag.setVisible(false);
    bt_syringe_auto.setVisible(false);
    bt_vacuum.setVisible(false);
}

void MainMenuView::configButton(int8_t position, int8_t maxButtons, int8_t shiftType, touchgfx::Button *button, uint16_t bitmapPressBig, uint16_t bitmapReleaseBig, uint16_t bitmapPressSmall, uint16_t bitmapReleaseSmall)
{
	bool lastRow = false;
	int16_t x, y;
	int16_t rows, cols;
	int16_t hMargin, vMargin;

	// detect menu structure
	if (maxButtons < 4 ) // 3 * row matrix (col - row)
	{
		button->setBitmaps(Bitmap(bitmapPressBig), Bitmap(bitmapReleaseBig));

		rows = 1;
		cols = 3;
		lastRow = true;
	}
	else if (maxButtons == 4 ) // 2 * 2 matrix (col - row)
	{
		button->setBitmaps(Bitmap(bitmapPressBig), Bitmap(bitmapReleaseBig));

		rows = 2;
		cols = 2;
		lastRow = false;
	}
	else if (maxButtons < 7 ) // 3 * 2 matrix (col - row)
	{
		button->setBitmaps(Bitmap(bitmapPressBig), Bitmap(bitmapReleaseBig));

		rows = 2;
		cols = 3;
		lastRow = (position > 2 ) ? true : false;
	}
	else if (maxButtons < 10 ) // 3 * 3 matrix (col - row)
	{
		button->setBitmaps(Bitmap(bitmapPressSmall), Bitmap(bitmapReleaseSmall));

		rows = 3;
		cols = 3;
		lastRow = (position > 5 ) ? true : false;
	}
	else // 4 * 3 matrix (col - row)
	{
		button->setBitmaps(Bitmap(bitmapPressSmall), Bitmap(bitmapReleaseSmall));

	    rows = 3;
		cols = 4;
		lastRow = (position > 7 ) ? true : false;
	}

	// compute margins
	hMargin = ( SCREEN_WIDTH - (cols * button->getWidth()) ) / (cols + 1);
	vMargin = ( SCREEN_HEIGHT - (rows * button->getWidth()) ) / (rows + 1);

	// compute coodinates
	x = hMargin + ( (hMargin + button->getWidth()) * (position % cols));
	y = vMargin + ( (vMargin + button->getWidth()) * (position / cols));

	// process extra left margin on last row
	if(lastRow){
		switch(shiftType){
		case MMF_END_SHIFT_0_5:
			x =  x + ((hMargin + button->getWidth()) / 2); // shift 0.5
			break;
		case MMF_END_SHIFT_1:
			x =  x +  (hMargin + button->getWidth());      // shift 1
			break;
		case MMF_END_SHIFT_2:
			x =  x + ((hMargin + button->getWidth()) * 2); // shift 2
			break;
		}
	}

	button->setX(x);
	button->setY(y);
	button->setVisible(true);
	button->invalidate();

//	printf("(%dx%d) btn %02d - x: %03d, y: %03d, last row: %s\n", cols, rows, position, x, y, lastRow ? "true" : "false");
}

void MainMenuView::updateSyringeButonsAlpha()
{
    bt_syringe.setAlpha(presenter->isCalibrationErrorDuringStartUp() ? 25 : 255);
    bt_syringe_auto.setAlpha(presenter->isCalibrationErrorDuringStartUp() ? 25 : 255);
    bt_syringe_manual.setAlpha(presenter->isCalibrationErrorDuringStartUp() ? 25 : 255);
}

void MainMenuView::updateAlphaDependingOnVaccum()
{
    bt_vacuum.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
    bt_vacuum_time.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
    bt_vacuum_pressure.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
    bt_bag.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
    bt_open_bag.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
    bt_closed_bag.setAlpha(presenter->isVaccumAvailable() ? 255 : 25);
/*
    bt_dental;
    bt_saline;
    bt_manual;
    bt_continuous;
    bt_dose;
    bt_syringe;
    bt_syringe_auto;
    bt_syringe_manual;
    bt_insuflation;
    bt_insuflation_v;
    bt_insuflation_r;
    */
}
