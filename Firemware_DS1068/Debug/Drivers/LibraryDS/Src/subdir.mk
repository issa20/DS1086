################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LibraryDS/Src/DS1086.c 

OBJS += \
./Drivers/LibraryDS/Src/DS1086.o 

C_DEPS += \
./Drivers/LibraryDS/Src/DS1086.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LibraryDS/Src/%.o Drivers/LibraryDS/Src/%.su: ../Drivers/LibraryDS/Src/%.c Drivers/LibraryDS/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F042x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/elhomaini.i/STM32CubeIDE/workspace_1.11.0/FiremwareTEST/Drivers/LibraryDS/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-LibraryDS-2f-Src

clean-Drivers-2f-LibraryDS-2f-Src:
	-$(RM) ./Drivers/LibraryDS/Src/DS1086.d ./Drivers/LibraryDS/Src/DS1086.o ./Drivers/LibraryDS/Src/DS1086.su

.PHONY: clean-Drivers-2f-LibraryDS-2f-Src

