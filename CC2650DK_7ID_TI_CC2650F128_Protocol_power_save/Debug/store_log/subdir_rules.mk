################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
store_log/bsp_spi.obj: ../store_log/bsp_spi.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/03468/workspace_v7/CC2650DK_7ID_TI_CC2650F128_Protocol_power_save" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/cc26xxware_2_24_02_17202" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" -g --define=ccs --define=POWER_SAVING --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="store_log/bsp_spi.d" --obj_directory="store_log" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

store_log/ext_flash.obj: ../store_log/ext_flash.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/03468/workspace_v7/CC2650DK_7ID_TI_CC2650F128_Protocol_power_save" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/cc26xxware_2_24_02_17202" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" -g --define=ccs --define=POWER_SAVING --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="store_log/ext_flash.d" --obj_directory="store_log" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

store_log/store_log.obj: ../store_log/store_log.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/03468/workspace_v7/CC2650DK_7ID_TI_CC2650F128_Protocol_power_save" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/cc26xxware_2_24_02_17202" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" -g --define=ccs --define=POWER_SAVING --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="store_log/store_log.d" --obj_directory="store_log" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


