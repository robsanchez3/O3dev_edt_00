################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/RTOS2/CMSIS/RTOS2/Heap/Heap_4/subdir.mk
-include Middlewares/RTOS2/CMSIS/RTOS2/Core/TZ_Non_Supported/subdir.mk
-include Drivers/Vendor/USB_DEVICE/Target/subdir.mk
-include Drivers/Vendor/USB_DEVICE/App/subdir.mk
-include Drivers/Vendor/STM32_USB_Device_Library/subdir.mk
-include Drivers/Vendor/FatFs/source/subdir.mk
-include Drivers/Vendor/Driver/subdir.mk
-include Drivers/Vendor/Components/subdir.mk
-include Drivers/Vendor/subdir.mk
-include Drivers/STM32U5xx_HAL_Driver/subdir.mk
-include Drivers/O3/Timer/subdir.mk
-include Drivers/O3/Syringe/subdir.mk
-include Drivers/O3/Protocol/subdir.mk
-include Drivers/O3/Fsm_o3/subdir.mk
-include Drivers/O3/Dependencies/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Application/User/gui/subdir.mk
-include Application/User/generated/subdir.mk
-include Application/User/TouchGFX/target/generated/subdir.mk
-include Application/User/TouchGFX/target/subdir.mk
-include Application/User/TouchGFX/App/subdir.mk
-include Application/User/Startup/subdir.mk
-include Application/User/FATFS/subdir.mk
-include Application/User/Core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := O3dev_edt_00
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
O3dev_edt_00.elf \

MAP_FILES += \
O3dev_edt_00.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
O3dev_edt_00.list \

OBJCOPY_HEX += \
O3dev_edt_00.hex \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: O3dev_edt_00.elf secondary-outputs

# Tool invocations
O3dev_edt_00.elf O3dev_edt_00.map: $(OBJS) $(USER_OBJS) C:\Roberto\O3\O3dev_edt_00\STM32CubeIDE\STM32U599NIHXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "O3dev_edt_00.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Roberto\O3\O3dev_edt_00\STM32CubeIDE\STM32U599NIHXQ_FLASH.ld" --specs=nosys.specs -Wl,-Map="O3dev_edt_00.map" -Wl,--gc-sections -static -L../../Middlewares/ST/touchgfx/lib/core/cortex_m33/gcc -L../../Middlewares/ST/touchgfx_components/gpu2d/NemaGFX/lib/core/cortex_m33/gcc -L../../Middlewares/ST/touchgfx_components/gpu2d/TouchGFXNema/lib/cortex_m33/gcc -Wl,--no-warn-rwx-segment --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

O3dev_edt_00.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "O3dev_edt_00.list"
	@echo 'Finished building: $@'
	@echo ' '

O3dev_edt_00.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "O3dev_edt_00.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) O3dev_edt_00.elf O3dev_edt_00.hex O3dev_edt_00.list O3dev_edt_00.map default.size.stdout
	-@echo ' '

post-build:
	arm-none-eabi-objcopy.exe -v --remove-section=ExtFlashSection "O3dev_edt_00.elf" "IntFlash.elf"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
