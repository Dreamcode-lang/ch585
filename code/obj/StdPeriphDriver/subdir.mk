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
./StdPeriphDriver/CH58x_adc.d \
./StdPeriphDriver/CH58x_clk.d \
./StdPeriphDriver/CH58x_flash.d \
./StdPeriphDriver/CH58x_gpio.d \
./StdPeriphDriver/CH58x_i2c.d \
./StdPeriphDriver/CH58x_lcd.d \
./StdPeriphDriver/CH58x_pwm.d \
./StdPeriphDriver/CH58x_pwr.d \
./StdPeriphDriver/CH58x_spi0.d \
./StdPeriphDriver/CH58x_spi1.d \
./StdPeriphDriver/CH58x_sys.d \
./StdPeriphDriver/CH58x_timer0.d \
./StdPeriphDriver/CH58x_timer1.d \
./StdPeriphDriver/CH58x_timer2.d \
./StdPeriphDriver/CH58x_timer3.d \
./StdPeriphDriver/CH58x_uart0.d \
./StdPeriphDriver/CH58x_uart1.d \
./StdPeriphDriver/CH58x_uart2.d \
./StdPeriphDriver/CH58x_uart3.d \
./StdPeriphDriver/CH58x_usbdev.d \
./StdPeriphDriver/CH58x_usbhostBase.d \
./StdPeriphDriver/CH58x_usbhostClass.d 

OBJS += \
./StdPeriphDriver/CH58x_adc.o \
./StdPeriphDriver/CH58x_clk.o \
./StdPeriphDriver/CH58x_flash.o \
./StdPeriphDriver/CH58x_gpio.o \
./StdPeriphDriver/CH58x_i2c.o \
./StdPeriphDriver/CH58x_lcd.o \
./StdPeriphDriver/CH58x_pwm.o \
./StdPeriphDriver/CH58x_pwr.o \
./StdPeriphDriver/CH58x_spi0.o \
./StdPeriphDriver/CH58x_spi1.o \
./StdPeriphDriver/CH58x_sys.o \
./StdPeriphDriver/CH58x_timer0.o \
./StdPeriphDriver/CH58x_timer1.o \
./StdPeriphDriver/CH58x_timer2.o \
./StdPeriphDriver/CH58x_timer3.o \
./StdPeriphDriver/CH58x_uart0.o \
./StdPeriphDriver/CH58x_uart1.o \
./StdPeriphDriver/CH58x_uart2.o \
./StdPeriphDriver/CH58x_uart3.o \
./StdPeriphDriver/CH58x_usbdev.o \
./StdPeriphDriver/CH58x_usbhostBase.o \
./StdPeriphDriver/CH58x_usbhostClass.o 

DIR_OBJS += \
./StdPeriphDriver/*.o \

DIR_DEPS += \
./StdPeriphDriver/*.d \

DIR_EXPANDS += \
./StdPeriphDriver/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
StdPeriphDriver/%.o: ../Libraries/WCH/CH585/StdPeriphDriver/%.c
	@	riscv-wch-elf-gcc -march=rv32imc_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common --param=highcode-gen-section-name=1 -g -DDEBUG=0 -I"../Libraries/WCH/CH585/StdPeriphDriver/inc" -I"../Libraries/WCH/CH585/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

