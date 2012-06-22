################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../dep/json_cpp/jsoncpp.cpp 

OBJS += \
./dep/json_cpp/jsoncpp.o 

CPP_DEPS += \
./dep/json_cpp/jsoncpp.d 


# Each subdirectory must supply rules for building sources it contributes
dep/json_cpp/%.o: ../dep/json_cpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../dep/json_cpp -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


