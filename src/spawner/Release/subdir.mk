################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../json_inout.o \
../main.o \
../periodicity.o \
../sched_new.o \
../task.o 

C_SRCS += \
../json_inout.c \
../main.c \
../periodicity.c \
../sched_new.c \
../task.c 

OBJS += \
./json_inout.o \
./main.o \
./periodicity.o \
./sched_new.o \
./task.o 

C_DEPS += \
./json_inout.d \
./main.d \
./periodicity.d \
./sched_new.d \
./task.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


