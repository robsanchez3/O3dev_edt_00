#include <gui/oztd_screen/OZTDView.hpp>
#include <gui/oztd_screen/OZTDPresenter.hpp>

#include <stdio.h>

#ifndef SIMULATOR
#include "usbd_cdc_if.h"
#endif

extern "C" void LaunchTherapy(void); /*TODO......  improve path*/
extern "C" void Experimental_StopTherapy(void); /*TODO......  improve path*/

#ifndef SIMULATOR
//#include <../../../../../Drivers/O3/TargetApp/fsm_o3_operation.h>/*TODO......  improve path*/
#include <../../../../../Drivers/O3/Fsm_o3/fsm_o3_operation.h>/*TODO......  improve path*/
extern "C" FSM_O3_OPERATION_T GLB_fsm_o3;
#endif




int debValue1 = 0;
uint8_t USB_CDC_Tx_Debug_Buffer1[100];

OZTDPresenter::OZTDPresenter(OZTDView& v)
    : view(v)
{

}

void OZTDPresenter::activate()
{

}

void OZTDPresenter::deactivate()
{

}

void OZTDPresenter::StartGeneration(void)
{
	printf("Start generation...\n");
#ifndef SIMULATOR
	LaunchTherapy();
#endif
}

void OZTDPresenter::StopGeneration(void)
{
	printf("Stop generation...\n");
//	Experimental_StopTherapy();
	printf("Trying to send %d over CDC_USB\n", debValue1);
    sprintf((char *) USB_CDC_Tx_Debug_Buffer1, "Debug USB = %d\n\r", debValue1++);
//    CDC_Transmit_FS(USB_CDC_Tx_Debug_Buffer1, (uint16_t) strlen ((char *)USB_CDC_Tx_Debug_Buffer1));

#ifndef SIMULATOR
    CDC_Transmit_FS((uint8_t*)"hello man!\n", (uint16_t) strlen ((char *)"hello man!\n"));
#endif
}

void OZTDPresenter::UpdateTargetFlow(int value)
{
	printf("Updating Target flow to %d\n", value);
#ifndef SIMULATOR
	GLB_fsm_o3.ConfiguredFlow = value;
#endif
}
