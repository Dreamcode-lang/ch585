################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/BLE/battservice.c \
../App/BLE/devinfoservice.c \
../App/BLE/hiddev.c \
../App/BLE/hidmouse.c \
../App/BLE/hidmouseservice.c \
../App/BLE/scanparamservice.c 

C_DEPS += \
./App/BLE/battservice.d \
./App/BLE/devinfoservice.d \
./App/BLE/hiddev.d \
./App/BLE/hidmouse.d \
./App/BLE/hidmouseservice.d \
./App/BLE/scanparamservice.d 

OBJS += \
./App/BLE/battservice.o \
./App/BLE/devinfoservice.o \
./App/BLE/hiddev.o \
./App/BLE/hidmouse.o \
./App/BLE/hidmouseservice.o \
./App/BLE/scanparamservice.o 

DIR_OBJS += \
./App/BLE/*.o \

DIR_DEPS += \
./App/BLE/*.d \

DIR_EXPANDS += \
./App/BLE/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
App/BLE/%.o: ../App/BLE/%.c
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -DDEBUG=0 -DCH58xBLE -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/StdPeriphDriver/inc" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/RVMSIS" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/HAL/include" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/LIB" -I"d:/Other/嵌赛/git_ch585/code/App/BLE" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

