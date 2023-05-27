################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx.c \
../ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx_ra_driver.c 

C_DEPS += \
./ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx.d \
./ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx_ra_driver.d 

OBJS += \
./ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx.o \
./ra/fsp/src/rm_zmod4xxx/rm_zmod4xxx_ra_driver.o 

SREC += \
ZMOD4XXX.srec 

MAP += \
ZMOD4XXX.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/rm_zmod4xxx/%.o: ../ra/fsp/src/rm_zmod4xxx/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RENESAS_RA_ -D_RA_CORE=CM33 -I"D:/code/e2studio/ZMOD4XXX/src" -I"D:/code/e2studio/ZMOD4XXX/ra/fsp/inc" -I"D:/code/e2studio/ZMOD4XXX/ra/fsp/inc/api" -I"D:/code/e2studio/ZMOD4XXX/ra/fsp/inc/instances" -I"D:/code/e2studio/ZMOD4XXX/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"D:/code/e2studio/ZMOD4XXX/ra_gen" -I"D:/code/e2studio/ZMOD4XXX/ra_cfg/fsp_cfg/bsp" -I"D:/code/e2studio/ZMOD4XXX/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

