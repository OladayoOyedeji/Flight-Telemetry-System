# Toolchain setup
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Project Paths
KERNEL_DIR = ./FreeRTOS-Kernel
SRC_DIR = ./src
INC_DIR = ./include
SCRIPT_DIR = ./scripts

# Compilation Flags
# -ffunction-sections & -fdata-sections allow the linker to delete unused code
CFLAGS = -mcpu=cortex-m3 -mthumb -g -O0
CFLAGS += --specs=nosys.specs
CFLAGS += -fno-exceptions -fno-unwind-tables 
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(INC_DIR) \
          -I$(SRC_DIR) \
          -I$(KERNEL_DIR)/include \
          -I$(KERNEL_DIR)/portable/GCC/ARM_CM3

# Linker Flags
# --defsym translates Demo names to Modern Compiler names
# --gc-sections removes the overlapping .eh_frame data automatically
LDFLAGS = -T $(SCRIPT_DIR)/standalone.ld
LDFLAGS += -Wl,--defsym=__bss_start__=_bss
LDFLAGS += -Wl,--defsym=__bss_end__=_ebss
LDFLAGS += -Wl,--defsym=end=_ebss
LDFLAGS += -Wl,--gc-sections

# Source Files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/startup.c \
       $(KERNEL_DIR)/tasks.c \
       $(KERNEL_DIR)/list.c \
       $(KERNEL_DIR)/queue.c \
       $(KERNEL_DIR)/timers.c \
       $(KERNEL_DIR)/portable/MemMang/heap_4.c \
       $(KERNEL_DIR)/portable/GCC/ARM_CM3/port.c

# Build Rules
all: build/telemetry.bin

build/telemetry.elf: $(SRCS)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

build/telemetry.bin: build/telemetry.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build

r run: build/telemetry.elf
	qemu-system-arm -machine lm3s6965evb -nographic -kernel $<
