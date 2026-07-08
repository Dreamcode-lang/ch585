################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/WCH/CH585/BLE/HAL/KEY.c \
../Libraries/WCH/CH585/BLE/HAL/LED.c \
../Libraries/WCH/CH585/BLE/HAL/MCU.c \
../Libraries/WCH/CH585/BLE/HAL/RTC.c \
../Libraries/WCH/CH585/BLE/HAL/SLEEP.c 

C_DEPS += \
./Libraries/WCH/CH585/BLE/HAL/KEY.d \
./Libraries/WCH/CH585/BLE/HAL/LED.d \
./Libraries/WCH/CH585/BLE/HAL/MCU.d \
./Libraries/WCH/CH585/BLE/HAL/RTC.d \
./Libraries/WCH/CH585/BLE/HAL/SLEEP.d 

OBJS += \
./Libraries/WCH/CH585/BLE/HAL/KEY.o \
./Libraries/WCH/CH585/BLE/HAL/LED.o \
./Libraries/WCH/CH585/BLE/HAL/MCU.o \
./Libraries/WCH/CH585/BLE/HAL/RTC.o \
./Libraries/WCH/CH585/BLE/HAL/SLEEP.o 

DIR_OBJS += \
./Libraries/WCH/CH585/BLE/HAL/*.o \

DIR_DEPS += \
./Libraries/WCH/CH585/BLE/HAL/*.d \

DIR_EXPANDS += \
./Libraries/WCH/CH585/BLE/HAL/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Libraries/WCH/CH585/BLE/HAL/%.o: ../Libraries/WCH/CH585/BLE/HAL/%.c
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -DDEBUG=0 -DCH58xBLE -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/StdPeriphDriver/inc" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/RVMSIS" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/HAL/include" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/LIB" -I"d:/Other/嵌赛/git_ch585/code/App/BLE" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

