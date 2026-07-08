################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Libraries/WCH/CH585/BLE/LIB/ble_task_scheduler.S 

S_UPPER_DEPS += \
./Libraries/WCH/CH585/BLE/LIB/ble_task_scheduler.d 

OBJS += \
./Libraries/WCH/CH585/BLE/LIB/ble_task_scheduler.o 

DIR_OBJS += \
./Libraries/WCH/CH585/BLE/LIB/*.o \

DIR_DEPS += \
./Libraries/WCH/CH585/BLE/LIB/*.d \

DIR_EXPANDS += \
./Libraries/WCH/CH585/BLE/LIB/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Libraries/WCH/CH585/BLE/LIB/%.o: ../Libraries/WCH/CH585/BLE/LIB/%.S
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

