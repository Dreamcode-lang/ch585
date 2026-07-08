################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/I2C/bsp_i2c_bus.c 

C_DEPS += \
./Bsp/I2C/bsp_i2c_bus.d 

OBJS += \
./Bsp/I2C/bsp_i2c_bus.o 

DIR_OBJS += \
./Bsp/I2C/*.o \

DIR_DEPS += \
./Bsp/I2C/*.d \

DIR_EXPANDS += \
./Bsp/I2C/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Bsp/I2C/%.o: ../Bsp/I2C/%.c
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -DDEBUG=0 -DCH58xBLE -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/StdPeriphDriver/inc" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/RVMSIS" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/HAL/include" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/LIB" -I"d:/Other/嵌赛/git_ch585/code/App/BLE" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

