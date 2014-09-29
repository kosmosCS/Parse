################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LPC17xx/crp.c \
../LPC17xx/system_LPC17xx.c 

OBJS += \
./LPC17xx/crp.o \
./LPC17xx/system_LPC17xx.o 

C_DEPS += \
./LPC17xx/crp.d \
./LPC17xx/system_LPC17xx.d 


# Each subdirectory must supply rules for building sources it contributes
LPC17xx/%.o: ../LPC17xx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


