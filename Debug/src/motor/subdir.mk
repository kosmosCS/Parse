################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/motor/BLDC.c \
../src/motor/PID.c \
../src/motor/UART_ProcessData.c 

OBJS += \
./src/motor/BLDC.o \
./src/motor/PID.o \
./src/motor/UART_ProcessData.o 

C_DEPS += \
./src/motor/BLDC.d \
./src/motor/PID.d \
./src/motor/UART_ProcessData.d 


# Each subdirectory must supply rules for building sources it contributes
src/motor/%.o: ../src/motor/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


