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
	g++ -I"/Users/jiharris/Documents/Personal/code/RayTracer2014/glm-0.9.5.2" -I"/Users/jiharris/Documents/Personal/code/RayTracer2014/include" -I/opt/local/include -I/opt/X11/include/freetype2 -I/opt/X11/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


