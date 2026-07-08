################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Libraries/WCH/CH585/StdPeriphDriver/subdir.mk
-include Libraries/WCH/CH585/Startup/subdir.mk
-include Libraries/WCH/CH585/BLE/LIB/subdir.mk
-include Libraries/WCH/CH585/BLE/HAL/subdir.mk
-include Bsp/TouchPad/subdir.mk
-include Bsp/Motor/subdir.mk
-include Bsp/IMU/subdir.mk
-include Bsp/I2C/subdir.mk
-include Bsp/HR/subdir.mk
-include Bsp/Disp/subdir.mk
-include Bsp/subdir.mk
-include App/BLE/subdir.mk
-include App/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
code.hex \

SECONDARY_LIST += \
code.lst \

SECONDARY_SIZE += \
code.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: code.elf secondary-outputs

# Tool invocations
code.elf: $(OBJS) $(USER_OBJS_ESCAPE)
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -T "d:\Other\嵌赛\git_ch585\code\Libraries\WCH\CH585\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"d:\Other\嵌赛\git_ch585\code" -L"d:\Other\嵌赛\git_ch585\code\Libraries\WCH\CH585\StdPeriphDriver" -L"d:\Other\嵌赛\git_ch585\code\Libraries\WCH\CH585\BLE\LIB" -Xlinker --print-memory-usage -Wl,-Map,"code.map" --specs=nano.specs --specs=nosys.specs -o "code.elf" $(OBJS) $(LIBS)
code.hex: code.elf
	@	riscv-wch-elf-objcopy -O ihex "code.elf" "code.hex"
code.lst: code.elf
	@	riscv-wch-elf-objdump --source --all-headers --demangle -M xw --line-numbers --wide "code.elf" > "code.lst"
code.siz: code.elf
	riscv-wch-elf-size --format=berkeley "code.elf"

# Other Targets
clean:
	-$(RM) $(DIR_OBJS) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE) code.elf
	-$(RM) $(DIR_EXPANDS) $(CALLGRAPH_DOT)
	-$(RM) $(DIR_DEPS)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets