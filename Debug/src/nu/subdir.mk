################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/nu/usbcore_old.c \
../src/nu/usbhw_old.c 

OBJS += \
./src/nu/usbcore_old.o \
./src/nu/usbhw_old.o 

C_DEPS += \
./src/nu/usbcore_old.d \
./src/nu/usbhw_old.d 


# Each subdirectory must supply rules for building sources it contributes
src/nu/%.o: ../src/nu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


