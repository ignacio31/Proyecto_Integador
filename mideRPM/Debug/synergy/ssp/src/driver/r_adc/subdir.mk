################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/driver/r_adc/r_adc.c 

OBJS += \
./synergy/ssp/src/driver/r_adc/r_adc.o 

C_DEPS += \
./synergy/ssp/src/driver/r_adc/r_adc.d 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/driver/r_adc/%.o: ../synergy/ssp/src/driver/r_adc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	C:\Renesas\e2_studio\Utilities\\/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy_cfg\ssp_cfg\driver" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy\ssp\inc" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy\ssp\inc\bsp" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy\ssp\inc\driver\api" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\synergy\ssp\inc\driver\instances" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\src" -I"C:\Users\Ing Jorge Blancas\e2_studio\ControlVelocidad\mideRPM\src\synergy_gen" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


