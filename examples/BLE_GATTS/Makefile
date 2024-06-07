# Prefix for older riscv gcc is  risv-none-embed
# Prefix for newer riscv gcc is  risv-none-elf
# TOOLCHAIN_PREFIX := riscv-none-embed
TOOLCHAIN_PREFIX := ~/.local/xPacks/riscv-none-elf-gcc/xpack-riscv-none-elf-gcc-13.2.0-2/bin/riscv-none-elf

APP_C_SRCS += \
  ./src/main.c \
  ./src/devinfoservice.c \
  ./src/gattprofile.c \
  ./src/peripheral.c \

SDK_BLE_HAL_C_SRCS := \
  ../../sdk/BLE/HAL/MCU.c \
  ../../sdk/BLE/HAL/RTC.c \
  ../../sdk/BLE/HAL/SLEEP.c

SDK_STDPERIPHDRIVER_C_SRCS += \
  ../../sdk/StdPeriphDriver/CH59x_adc.c \
  ../../sdk/StdPeriphDriver/CH59x_clk.c \
  ../../sdk/StdPeriphDriver/CH59x_gpio.c \
  ../../sdk/StdPeriphDriver/CH59x_sys.c \
  ../../sdk/StdPeriphDriver/CH59x_uart1.c \


SDK_RVMSIS_C_SRCS += \
  ../../sdk/RVMSIS/core_riscv.c

SDK_BLE_LIB_S_UPPER_SRCS += \
  ../../sdk/BLE/LIB/ble_task_scheduler.S
SDK_STARTUP_S_UPPER_SRCS += \
  ../../sdk/Startup/startup_CH592.S

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
  $(patsubst ../../sdk/%.c,obj/sdk/%.o,$(SDK_BLE_HAL_C_SRCS)) \
  $(patsubst ../../sdk/%.c,obj/sdk/%.o,$(SDK_STDPERIPHDRIVER_C_SRCS)) \
  $(patsubst ../../sdk/%.c,obj/sdk/%.o,$(SDK_RVMSIS_C_SRCS)) \
  $(patsubst ../../sdk/%.S,obj/sdk/%.o,$(SDK_BLE_LIB_S_UPPER_SRCS)) \
  $(patsubst ../../sdk/%.S,obj/sdk/%.o,$(SDK_STARTUP_S_UPPER_SRCS))

MAKEFILE_DEPS := \
  $(foreach obj,$(OBJS),$(patsubst %.o,%.d,$(obj)))

STDPERIPHDRIVER_LIBS := -L"../../sdk/StdPeriphDriver" -lISP592
BLE_LIB_LIBS := -L"../../sdk/BLE/LIB" -lCH59xBLE
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
	    -T "../../sdk/Ld/Link.ld" \
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
	    -I"../../sdk/StdPeriphDriver/inc" \
	    -I"../../sdk/RVMSIS" \
	    -I"../../sdk/BLE/LIB" \
	    -I"../../sdk/BLE/HAL/include" \
	    -std=gnu99 \
	    -MMD \
	    -MP \
	    -MF"$(@:%.o=%.d)" \
	    -MT"$(@)" \
	    -c \
	    -o "$@" "$<"

obj/sdk/%.o: ../../sdk/%.c
	@ mkdir --parents $(dir $@)
	@ ${TOOLCHAIN_PREFIX}-gcc \
	    $(CFLAGS_COMMON) \
	    -DDEBUG=1 \
	    -I"src/include" \
	    -I"../../sdk/StdPeriphDriver/inc" \
	    -I"../../sdk/RVMSIS" \
	    -I"../../sdk/BLE/LIB" \
	    -I"../../sdk/BLE/HAL/include" \
	    -std=gnu99 \
	    -MMD \
	    -MP \
	    -MF"$(@:%.o=%.d)" \
	    -MT"$(@)" \
	    -c \
	    -o "$@" "$<"

obj/sdk/%.o: ../../sdk/%.S
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
