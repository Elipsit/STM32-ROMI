################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include App/Scr/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Sonar_Romi.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Sonar_Romi.list \

OBJCOPY_BIN += \
Sonar_Romi.bin \


# All Target
all: Sonar_Romi.elf secondary-outputs

# Tool invocations
Sonar_Romi.elf: $(OBJS) $(USER_OBJS) C:\github\STM32-ROMI\Code\STM32F405RG\CodeBlocks\Sonar-Romi\STM32F405RGTX_FLASH.ld
	arm-none-eabi-gcc -o "Sonar_Romi.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\github\STM32-ROMI\Code\STM32F405RG\CodeBlocks\Sonar-Romi\STM32F405RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Sonar_Romi.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Sonar_Romi.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Sonar_Romi.list"
	@echo 'Finished building: $@'
	@echo ' '

Sonar_Romi.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Sonar_Romi.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
