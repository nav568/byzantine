################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/ConnectionManager.o \
../src/CustomException.o \
../src/InputParser.o \
../src/StartProcess.o 

CPP_SRCS += \
../src/ConnectionManager.cpp \
../src/InputParser.cpp \
../src/StartProcess.cpp 

OBJS += \
./src/ConnectionManager.o \
./src/InputParser.o \
./src/StartProcess.o 

CPP_DEPS += \
./src/ConnectionManager.d \
./src/InputParser.d \
./src/StartProcess.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


