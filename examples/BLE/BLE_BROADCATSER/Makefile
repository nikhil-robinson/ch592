
TOOLCHAIN_PREFIX := ~/.local/xPacks/riscv-none-elf-gcc/xpack-riscv-none-elf-gcc-13.2.0-2/bin/riscv-none-elf

SDK_PATH := ../../../sdk


APP_C_SRCS += \
  ./src/main.c\
  ./src/broadcaster.c\
  ./src/devinfoservice.c

SDK_BLE_HAL_C_SRCS := \
  $(SDK_PATH)/BLE/HAL/MCU.c \
  $(SDK_PATH)/BLE/HAL/RTC.c \
  $(SDK_PATH)/BLE/HAL/SLEEP.c

SDK_STDPERIPHDRIVER_C_SRCS += \
  $(SDK_PATH)/StdPeriphDriver/CH59x_adc.c \
  $(SDK_PATH)/StdPeriphDriver/CH59x_clk.c \
  $(SDK_PATH)/StdPeriphDriver/CH59x_gpio.c \
  $(SDK_PATH)/StdPeriphDriver/CH59x_pwr.c \
  $(SDK_PATH)/StdPeriphDriver/CH59x_sys.c \
  $(SDK_PATH)/StdPeriphDriver/CH59x_uart1.c \


SDK_RVMSIS_C_SRCS += \
  $(SDK_PATH)/RVMSIS/core_riscv.c

SDK_BLE_LIB_S_UPPER_SRCS += \
  $(SDK_PATH)/BLE/LIB/ble_task_scheduler.S
SDK_STARTUP_S_UPPER_SRCS += \
  $(SDK_PATH)/Startup/startup_CH592.S

C_SRCS := \
  $(APP_C_SRCS) \
  $(SDK_BLE_HAL_C_SRCS) \
  $(SDK_STDPERIPHDRIVER_C_SRCS) \
  $(SDK_RVMSIS_C_SRCS)

S_UPPER_SRCS := \
  $(SDK_BLE_LIB_S_UPPER_SRCS) \
  $(SDK_STARTUP_S_UPPER_SRCS)

OBJS := \
  $(patsubst ./src/%.c,obj/src/%.o,$(APP_C_SRCS)) \
  $(patsubst $(SDK_PATH)/%.c,obj/sdk/%.o,$(SDK_BLE_HAL_C_SRCS)) \
  $(patsubst $(SDK_PATH)/%.c,obj/sdk/%.o,$(SDK_STDPERIPHDRIVER_C_SRCS)) \
  $(patsubst $(SDK_PATH)/%.c,obj/sdk/%.o,$(SDK_RVMSIS_C_SRCS)) \
  $(patsubst $(SDK_PATH)/%.S,obj/sdk/%.o,$(SDK_BLE_LIB_S_UPPER_SRCS)) \
  $(patsubst $(SDK_PATH)/%.S,obj/sdk/%.o,$(SDK_STARTUP_S_UPPER_SRCS))

MAKEFILE_DEPS := \
  $(foreach obj,$(OBJS),$(patsubst %.o,%.d,$(obj)))

STDPERIPHDRIVER_LIBS := -L"$(SDK_PATH)/StdPeriphDriver" -lISP592
BLE_LIB_LIBS := -L"$(SDK_PATH)/BLE/LIB" -lCH59xBLE
LIBS := $(STDPERIPHDRIVER_LIBS) $(BLE_LIB_LIBS)

SECONDARY_FLASH := main.hex
SECONDARY_LIST := main.lst
SECONDARY_SIZE := main.siz

# ARCH is rv32imac on older gcc, rv32imac_zicsr on newer gcc
# ARCH := rv32imac
ARCH := rv32imac_zicsr

CFLAGS_COMMON := \
  -march=$(ARCH) \
  -mabi=ilp32 \
  -mcmodel=medany \
  -msmall-data-limit=8 \
  -mno-save-restore \
  -Os \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections
  #-g

.PHONY: all
all: main.elf secondary-outputs

.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(MAKEFILE_DEPS)
	-rm -f $(SECONDARY_FLASH)
	-rm -f $(SECONDARY_LIST)
	-rm -f main.elf
	-rm -f main.map
	-rm -rf ./obj

.PHONY: secondary-outputs
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

main.elf: $(OBJS)
	${TOOLCHAIN_PREFIX}-gcc \
	    $(CFLAGS_COMMON) \
	    -T "$(SDK_PATH)/Ld/Link.ld" \
	    -nostartfiles \
	    -Xlinker \
	    --gc-sections \
	    -Xlinker \
	    --print-memory-usage \
	    -Wl,-Map,"main.map" \
	    -Lobj \
	    --specs=nano.specs \
	    --specs=nosys.specs \
	    -o "main.elf" \
	    $(OBJS) \
	    $(LIBS)

%.hex: %.elf
	@ ${TOOLCHAIN_PREFIX}-objcopy -O ihex "$<"  "$@"

%.lst: %.elf
	@ ${TOOLCHAIN_PREFIX}-objdump \
	    --source \
	    --all-headers \
	    --demangle \
	    --line-numbers \
	    --wide "$<" > "$@"

%.siz: %.elf
	@ ${TOOLCHAIN_PREFIX}-size --format=berkeley "$<"

obj/src/%.o: ./src/%.c
	@ mkdir --parents $(dir $@)
	@ ${TOOLCHAIN_PREFIX}-gcc \
	    $(CFLAGS_COMMON) \
	    -DDEBUG=1 \
	    -I"src/include" \
	    -I"$(SDK_PATH)/StdPeriphDriver/inc" \
	    -I"$(SDK_PATH)/RVMSIS" \
	    -I"$(SDK_PATH)/BLE/LIB" \
	    -I"$(SDK_PATH)/BLE/HAL/include" \
	    -std=gnu99 \
	    -MMD \
	    -MP \
	    -MF"$(@:%.o=%.d)" \
	    -MT"$(@)" \
	    -c \
	    -o "$@" "$<"

obj/sdk/%.o: $(SDK_PATH)/%.c
	@ mkdir --parents $(dir $@)
	@ ${TOOLCHAIN_PREFIX}-gcc \
	    $(CFLAGS_COMMON) \
	    -DDEBUG=1 \
	    -I"src/include" \
	    -I"$(SDK_PATH)/StdPeriphDriver/inc" \
	    -I"$(SDK_PATH)/RVMSIS" \
	    -I"$(SDK_PATH)/BLE/LIB" \
	    -I"$(SDK_PATH)/BLE/HAL/include" \
	    -std=gnu99 \
	    -MMD \
	    -MP \
	    -MF"$(@:%.o=%.d)" \
	    -MT"$(@)" \
	    -c \
	    -o "$@" "$<"

obj/sdk/%.o: $(SDK_PATH)/%.S
	@ mkdir --parents $(dir $@)
	@ ${TOOLCHAIN_PREFIX}-gcc \
	    $(CFLAGS_COMMON) \
	    -x assembler \
	    -MMD \
	    -MP \
	    -MF"$(@:%.o=%.d)" \
	    -MT"$(@)" \
	    -c \
	    -o "$@" "$<"

f: clean all  
	wchisp flash ./main.elf

flash: 
	wchisp flash ./main.elf
