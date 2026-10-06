# ============================================================
#  STM32F103C8T6 (Blue Pill) - auto-scan build
#
#  Every .c / .s file under the project is discovered with
#  $(wildcard), so adding a new peripheral in CubeMX never
#  requires editing a source list again.
#
#  NOTE: ".RECIPEPREFIX = >" makes recipe lines start with '>'
#        instead of a TAB character (no more TAB/space bugs).
# ============================================================
.RECIPEPREFIX = >
SHELL := cmd.exe
.SHELLFLAGS := /c

# ---- project under test (override with: make PROJ=...) ----
PROJ ?= D:/STM32/CubeIDE_1x/test_led

# ---- toolchain (ships inside STM32CubeIDE) ----
TOOLS    := D:/STM32/CubeIDE_1x/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.0.202411081344/tools/bin
CC       := $(TOOLS)/arm-none-eabi-gcc
SZ       := $(TOOLS)/arm-none-eabi-size
LDSCRIPT := $(PROJ)/STM32F103C8TX_FLASH.ld

CPU := -mcpu=cortex-m3 -mfloat-abi=soft -mthumb
INCS := -I$(PROJ)/Core/Inc \
        -I$(PROJ)/Drivers/STM32F1xx_HAL_Driver/Inc \
        -I$(PROJ)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
        -I$(PROJ)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
        -I$(PROJ)/Drivers/CMSIS/Include

CFLAGS  := $(CPU) -std=gnu11 -g3 -O0 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB $(INCS) \
           -ffunction-sections -fdata-sections -Wall -MMD -MP --specs=nano.specs
ASFLAGS := $(CPU) -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP --specs=nano.specs
LDFLAGS := $(CPU) -T$(LDSCRIPT) --specs=nosys.specs -Wl,-Map=test_led.map \
           -Wl,--gc-sections -static --specs=nano.specs \
           -Wl,--start-group -lc -lm -Wl,--end-group

TARGET := test_led.elf

# ---------------- auto-discovered sources ----------------
C_SRCS := $(wildcard $(PROJ)/Core/Src/*.c)
C_SRCS += $(wildcard $(PROJ)/Drivers/STM32F1xx_HAL_Driver/Src/*.c)
S_SRCS := $(wildcard $(PROJ)/Core/Startup/*.s)

# flat object names inside the build folder
OBJS := $(addsuffix .o,$(basename $(notdir $(C_SRCS) $(S_SRCS))))

.PHONY: all clean list
all: $(TARGET)

$(TARGET): $(OBJS)
> @echo   LD  $@
> @$(CC) -o $@ $(OBJS) $(LDFLAGS)
> @echo.
> -@$(SZ) $@
> @echo.
> @echo   BUILD OK

# ---------------- one rule per source file ----------------
define RULE_C
$(basename $(notdir $(1))).o: $(1)
> @echo   CC  $(notdir $(1))
> @$(CC) $(CFLAGS) -c $(1) -o $(basename $(notdir $(1))).o
endef

define RULE_S
$(basename $(notdir $(1))).o: $(1)
> @echo   AS  $(notdir $(1))
> @$(CC) $(ASFLAGS) -o $(basename $(notdir $(1))).o $(1)
endef

$(foreach f,$(C_SRCS),$(eval $(call RULE_C,$(f))))
$(foreach f,$(S_SRCS),$(eval $(call RULE_S,$(f))))

# ---------------- header dependency files (-MMD -MP) ----------------
-include $(OBJS:.o=.d)

# ---------------- helpers ----------------
list:
> @echo project : $(PROJ)
> @echo sources : $(words $(C_SRCS)) .c + $(words $(S_SRCS)) .s
> @echo objects : $(words $(OBJS))

clean:
> @echo   removing objects, elf, map ...
> -@del /q *.o *.d *.elf *.map *.su *.cyclo 2>nul
> @echo   clean done
