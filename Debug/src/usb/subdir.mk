################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/usb/cdcuser.c \
../src/usb/usbcore.c \
../src/usb/usbdesc.c \
../src/usb/usbhw.c \
../src/usb/usbuser.c 

OBJS += \
./src/usb/cdcuser.o \
./src/usb/usbcore.o \
./src/usb/usbdesc.o \
./src/usb/usbhw.o \
./src/usb/usbuser.o 

C_DEPS += \
./src/usb/cdcuser.d \
./src/usb/usbcore.d \
./src/usb/usbdesc.d \
./src/usb/usbhw.d \
./src/usb/usbuser.d 


# Each subdirectory must supply rules for building sources it contributes
src/usb/%.o: ../src/usb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


