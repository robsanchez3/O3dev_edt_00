################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.c 

C_DEPS += \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.d 

OBJS += \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_USB_Host_Library/Core/Src/%.o Middlewares/STM32_USB_Host_Library/Core/Src/%.su Middlewares/STM32_USB_Host_Library/Core/Src/%.cyclo: ../Middlewares/STM32_USB_Host_Library/Core/Src/%.c Middlewares/STM32_USB_Host_Library/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U599xx -DUSE_USB_HS -c -I../../Core/Inc -I../../Drivers/O3 -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/mx25lm51245g -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/generated/videos/include -I../../TouchGFX/gui/include -I../../Drivers/Vendor/Device -I../../Drivers/Vendor/Driver -I../../Drivers/Vendor/Board/SE21194/Config -I../../Drivers/Vendor/Device/mx25lm51245g -I"../../USB_Device/App" -I"../../USB_Device/Target" -I"../../Drivers/Vendor/STM32_USB_Device_Library/Core/Inc" -I"../../Drivers/Vendor/STM32_USB_Device_Library/Class/CDC/Inc" -I"../../FATFS/App" -I"../../FATFS/Target" -I"../../Drivers/Vendor/FatFs/source/drivers/sd" -I"../../Drivers/Vendor/FatFs/source" -I"C:/Roberto/O3/O3dev_edt_00/STM32CubeIDE/Drivers" -I../../Drivers/O3/Fsm_o3 -I../../Middlewares/ST/touchgfx_components/gpu2d/NemaGFX/include -I../../Middlewares/ST/touchgfx_components/gpu2d/TouchGFXNema/include -I../../USB_Host/App -I../..Drivers/Vendor/USB_Host/App -I../..Drivers/Vendor/USB_Host/Target -I../../USB_Host/Target -I../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../../Drivers/Vendor/Device/USB -IC:/Roberto/O3/O3dev_edt_00/Drivers/Vendor/Device/USB -I/Drivers/Vendor/FatFs/source/drivers/USB -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src

clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src:
	-$(RM) ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.su

.PHONY: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src

