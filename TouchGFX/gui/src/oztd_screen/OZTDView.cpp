#include <gui/oztd_screen/OZTDView.hpp>

#include <stdio.h>

#ifndef SIMULATOR
//#include <../../../../../Drivers/O3/TargetApp/fsm_o3_operation.h>/*TODO......  improve path*/
#include <../../../../../Drivers/O3/Fsm_o3/fsm_o3_operation.h>/*TODO......  improve path*/
extern "C" FSM_O3_OPERATION_T GLB_fsm_o3;
#endif



OZTDView::OZTDView()
{

}

void OZTDView::setupScreen()
{
    OZTDViewBase::setupScreen();
}

void OZTDView::tearDownScreen()
{
    OZTDViewBase::tearDownScreen();
}

void OZTDView::handleTickEvent()
{
#ifndef SIMULATOR
	if(GLB_fsm_o3.RefreshScreen)
	{
		printf("Updating remaining seconds value: %2d\n", GLB_fsm_o3.RemainingSeconds);
		Unicode::snprintf(RemainingSecondsBuffer, sizeof(RemainingSeconds), "%2d", GLB_fsm_o3.RemainingSeconds);
		RemainingSeconds.invalidate();
		GLB_fsm_o3.RefreshScreen = 0;
	}
#endif
}

void OZTDView::StartGeneration(void)
{
	presenter->StartGeneration();
}

void OZTDView::StopGeneration(void)
{
	presenter->StopGeneration();
}

void OZTDView::UpdateTargetFlow(int value)
{
	presenter->UpdateTargetFlow(value);
}

void OZTDView::FlowSliderValueChanged(int value)
{
	// Update text field
		Unicode::snprintf(FlowSliderValueBuffer, sizeof(FlowSliderValue), "%d", value);
		FlowSliderValue.invalidate();
}
