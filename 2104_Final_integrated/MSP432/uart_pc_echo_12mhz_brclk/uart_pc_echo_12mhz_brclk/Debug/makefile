################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./startup_msp432p401r_ccs.obj" \
"./system_msp432p401r.obj" \
"./uart_pc_echo_12mhz_brclk.obj" \
"./Init/initComms.obj" \
"./Init/initMapping.obj" \
"./Init/initMovement.obj" \
"./Init/initSpeed.obj" \
"./Init/initUltrasound.obj" \
"./Init/printf.obj" \
"C:/Users/Marcus/Desktop/sit Y2S1/2104/lab1/msp432_driverlib_3_21_00_05/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/ccs/msp432p4xx_driverlib.lib" \
"../uart_pc_echo_12mhz_brclk_ccs.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Init/subdir_vars.mk
-include subdir_rules.mk
-include Init/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
uart_pc_echo_12mhz_brclk.out \

EXE_OUTPUTS__QUOTED += \
"uart_pc_echo_12mhz_brclk.out" \

BIN_OUTPUTS += \
uart_pc_echo_12mhz_brclk.bin \

BIN_OUTPUTS__QUOTED += \
"uart_pc_echo_12mhz_brclk.bin" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "uart_pc_echo_12mhz_brclk.out"

# Tool invocations
uart_pc_echo_12mhz_brclk.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number -z -m"uart_pc_echo_12mhz_brclk.map" --heap_size=0x1600 --stack_size=512 -i"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/lib" -i"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="uart_pc_echo_12mhz_brclk_linkInfo.xml" --rom_model -o "uart_pc_echo_12mhz_brclk.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

uart_pc_echo_12mhz_brclk.bin: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armhex"  -o "uart_pc_echo_12mhz_brclk.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "startup_msp432p401r_ccs.obj" "system_msp432p401r.obj" "uart_pc_echo_12mhz_brclk.obj" "Init\initComms.obj" "Init\initMapping.obj" "Init\initMovement.obj" "Init\initSpeed.obj" "Init\initUltrasound.obj" "Init\printf.obj" 
	-$(RM) "startup_msp432p401r_ccs.d" "system_msp432p401r.d" "uart_pc_echo_12mhz_brclk.d" "Init\initComms.d" "Init\initMapping.d" "Init\initMovement.d" "Init\initSpeed.d" "Init\initUltrasound.d" "Init\printf.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/ti/ccs930/ccs/utils/tiobj2bin/tiobj2bin uart_pc_echo_12mhz_brclk.out uart_pc_echo_12mhz_brclk.bin C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armofd C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armhex C:/ti/ccs930/ccs/utils/tiobj2bin/mkhex4bin
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

