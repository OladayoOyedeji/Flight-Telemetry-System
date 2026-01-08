# Toolchain setup
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Project Paths
KERNEL_DIR = ./FreeRTOS-Kernel
SRC_DIR = ./src
INC_DIR = ./include

# Compilation Flags
CFLAGS = -mcpu=cortex-m3 -mthumb -g -O0
# Added -I$(SRC_DIR) so the compiler finds FreeRTOSConfig.h
CFLAGS += -I$(INC_DIR) \
          -I$(SRC_DIR) \
          -I$(KERNEL_DIR)/include \
          -I$(KERNEL_DIR)/portable/GCC/ARM_CM3

# Source Files
SRCS = $(SRC_DIR)/main.c \
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
	$(CC) $(CFLAGS) $(SRCS) -T $(SRC_DIR)/qemu.ld -o $@

build/telemetry.bin: build/telemetry.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build

run: build/telemetry.bin
	qemu-system-arm -machine lm3s6965evb -nographic -kernel $<
