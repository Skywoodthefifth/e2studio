################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_ioport/r_ioport.c 

C_DEPS += \
./ra/fsp/src/r_ioport/r_ioport.d 

OBJS += \
./ra/fsp/src/r_ioport/r_ioport.o 

SREC += \
CKRA6M5_demo.srec 

MAP += \
CKRA6M5_demo.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_ioport/%.o: ../ra/fsp/src/r_ioport/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RENESAS_RA_ -D_RA_CORE=CM33 -I"D:/code/e2studio/CKRA6M5_demo/src" -I"D:/code/e2studio/CKRA6M5_demo/ra/fsp/inc" -I"D:/code/e2studio/CKRA6M5_demo/ra/fsp/inc/api" -I"D:/code/e2studio/CKRA6M5_demo/ra/fsp/inc/instances" -I"D:/code/e2studio/CKRA6M5_demo/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"D:/code/e2studio/CKRA6M5_demo/ra_gen" -I"D:/code/e2studio/CKRA6M5_demo/ra_cfg/fsp_cfg/bsp" -I"D:/code/e2studio/CKRA6M5_demo/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
