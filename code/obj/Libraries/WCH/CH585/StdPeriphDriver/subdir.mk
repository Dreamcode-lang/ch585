################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_adc.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_clk.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_flash.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_gpio.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_i2c.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_lcd.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwm.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwr.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi0.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi1.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_sys.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer0.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer1.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer2.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer3.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart0.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart1.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart2.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart3.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbdev.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostBase.c \
../Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostClass.c 

C_DEPS += \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_adc.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_clk.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_flash.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_gpio.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_i2c.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_lcd.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwm.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwr.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi0.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi1.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_sys.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer0.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer1.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer2.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer3.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart0.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart1.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart2.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart3.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbdev.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostBase.d \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostClass.d 

OBJS += \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_adc.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_clk.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_flash.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_gpio.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_i2c.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_lcd.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwm.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_pwr.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi0.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_spi1.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_sys.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer0.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer1.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer2.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_timer3.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart0.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart1.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart2.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_uart3.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbdev.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostBase.o \
./Libraries/WCH/CH585/StdPeriphDriver/CH58x_usbhostClass.o 

DIR_OBJS += \
./Libraries/WCH/CH585/StdPeriphDriver/*.o \

DIR_DEPS += \
./Libraries/WCH/CH585/StdPeriphDriver/*.d \

DIR_EXPANDS += \
./Libraries/WCH/CH585/StdPeriphDriver/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Libraries/WCH/CH585/StdPeriphDriver/%.o: ../Libraries/WCH/CH585/StdPeriphDriver/%.c
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -DDEBUG=0 -DCH58xBLE -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/StdPeriphDriver/inc" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/RVMSIS" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/HAL/include" -I"d:/Other/嵌赛/git_ch585/code/Libraries/WCH/CH585/BLE/LIB" -I"d:/Other/嵌赛/git_ch585/code/App/BLE" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

