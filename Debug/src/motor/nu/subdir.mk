################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/motor/nu/Main.c \
../src/motor/nu/Retarget.c 

OBJS += \
./src/motor/nu/Main.o \
./src/motor/nu/Retarget.o 

C_DEPS += \
./src/motor/nu/Main.d \
./src/motor/nu/Retarget.d 


# Each subdirectory must supply rules for building sources it contributes
src/motor/nu/%.o: ../src/motor/nu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


