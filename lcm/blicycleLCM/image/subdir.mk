################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../imageTest.cpp 

OBJS += \
./imageTest.o 

CPP_DEPS += \
./imageTest.d 


# Each subdirectory must supply rules for building sources it contributes
imageTest.o: ../imageTest.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/blicycle/Desktop/blicycle/software/build/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -I/home/blicycle/Desktop/blicycle/software/libbot2/bot2-core/src/bot_core -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"imageTest.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


