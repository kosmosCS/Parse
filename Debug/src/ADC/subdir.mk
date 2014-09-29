################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADC/L1754adc.c \
../src/ADC/t_table.c 

OBJS += \
./src/ADC/L1754adc.o \
./src/ADC/t_table.o 

C_DEPS += \
./src/ADC/L1754adc.d \
./src/ADC/t_table.d 


# Each subdirectory must supply rules for building sources it contributes
src/ADC/%.o: ../src/ADC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


