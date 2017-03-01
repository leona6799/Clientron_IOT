################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"./CC2650DK_7ID.obj" \
"./ccfg.obj" \
"./main.obj" \
"./sensor/D103.obj" \
"./sensor/MQ137.obj" \
"./sensor/OPT3001.obj" \
"./sensor/T6700.obj" \
"./sensor/YGC160FS.obj" \
"./sensor/hdc1080.obj" \
"./source/ADC.obj" \
"./source/I2C.obj" \
"./source/catch_value.obj" \
"./source/dev_distinguish.obj" \
"./source/disconnect.obj" \
"./source/get_log.obj" \
"./source/hal_trng_wrapper.obj" \
"./source/ignore_broadcast.obj" \
"./source/lora.obj" \
"./source/process_frame.obj" \
"./source/receive_ack.obj" \
"./source/receive_fxn.obj" \
"./source/sensor_setting.obj" \
"./source/system_info.obj" \
"./source/system_start.obj" \
"./source/time_fxn.obj" \
"./source/time_sync.obj" \
"./store_log/bsp_spi.obj" \
"./store_log/ext_flash.obj" \
"./store_log/store_log.obj" \
"../CC2650DK_7ID.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/cc26xxware_2_24_02_17202/driverlib/bin/ccs/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include sensor/subdir_vars.mk
-include source/subdir_vars.mk
-include store_log/subdir_vars.mk
-include subdir_rules.mk
-include sensor/subdir_rules.mk
-include source/subdir_rules.mk
-include store_log/subdir_rules.mk
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
CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.out \

EXE_OUTPUTS__QUOTED += \
"CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.out" \

BIN_OUTPUTS += \
CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.hex \

BIN_OUTPUTS__QUOTED += \
"CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.hex" \


# All Target
all: CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.out

# Tool invocations
CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me -g --define=ccs --define=POWER_SAVING --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi -z -m"CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.map" --stack_size=256 --heap_size=0 -i"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/lib" -i"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --reread_libs --display_error_number --warn_sections --diag_wrap=off --xml_link_info="CC2650DK_7ID_TI_CC2650F128_Protocol_power_save_linkInfo.xml" --rom_model -o "CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armhex" --memwidth=8 --romwidth=8 --intel -o "CC2650DK_7ID_TI_CC2650F128_Protocol_power_save.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_CMDS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "CC2650DK_7ID.obj" "ccfg.obj" "main.obj" "sensor\D103.obj" "sensor\MQ137.obj" "sensor\OPT3001.obj" "sensor\T6700.obj" "sensor\YGC160FS.obj" "sensor\hdc1080.obj" "source\ADC.obj" "source\I2C.obj" "source\catch_value.obj" "source\dev_distinguish.obj" "source\disconnect.obj" "source\get_log.obj" "source\hal_trng_wrapper.obj" "source\ignore_broadcast.obj" "source\lora.obj" "source\process_frame.obj" "source\receive_ack.obj" "source\receive_fxn.obj" "source\sensor_setting.obj" "source\system_info.obj" "source\system_start.obj" "source\time_fxn.obj" "source\time_sync.obj" "store_log\bsp_spi.obj" "store_log\ext_flash.obj" "store_log\store_log.obj" 
	-$(RM) "CC2650DK_7ID.d" "ccfg.d" "main.d" "sensor\D103.d" "sensor\MQ137.d" "sensor\OPT3001.d" "sensor\T6700.d" "sensor\YGC160FS.d" "sensor\hdc1080.d" "source\ADC.d" "source\I2C.d" "source\catch_value.d" "source\dev_distinguish.d" "source\disconnect.d" "source\get_log.d" "source\hal_trng_wrapper.d" "source\ignore_broadcast.d" "source\lora.d" "source\process_frame.d" "source\receive_ack.d" "source\receive_fxn.d" "source\sensor_setting.d" "source\system_info.d" "source\system_start.d" "source\time_fxn.d" "source\time_sync.d" "store_log\bsp_spi.d" "store_log\ext_flash.d" "store_log\store_log.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

