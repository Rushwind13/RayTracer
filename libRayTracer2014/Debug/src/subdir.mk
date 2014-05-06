################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Raytracer2014.cpp 

OBJS += \
./src/Raytracer2014.o 

CPP_DEPS += \
./src/Raytracer2014.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/jiharris/Documents/Personal/code/workspace/Raytracer2014/glm-0.9.5.2" -I/usr/local/include -I/opt/local/include/freetype2 -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


