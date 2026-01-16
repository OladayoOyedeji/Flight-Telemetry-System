# --- Toolchain Setup ---
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# --- Project Paths ---
BUILD_DIR  = build
KERNEL_DIR = FreeRTOS-Kernel
ARCH_DIR   = src/arch
INC_DIR    = include
SCRIPT_DIR = scripts

# --- Compilation Flags ---
CFLAGS  = -mcpu=cortex-m3 -mthumb -g -O0
CFLAGS += --specs=nosys.specs
CFLAGS += -fno-exceptions -fno-unwind-tables 
CFLAGS += -ffunction-sections -fdata-sections

# Include Paths (Add more -I flags here if you add new folders)
CFLAGS += -I$(INC_DIR) \
          -I$(ARCH_DIR) \
          -I$(KERNEL_DIR)/include \
          -I$(KERNEL_DIR)/portable/GCC/ARM_CM3

# --- Linker Flags ---
LDFLAGS  = -T $(SCRIPT_DIR)/standalone.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--defsym=__bss_start__=_bss
LDFLAGS += -Wl,--defsym=__bss_end__=_ebss
LDFLAGS += -Wl,--defsym=end=_ebss

# --- Source Files ---
# Set the path to wherever your main.c is currently located (e.g., ./main.c)
MAIN_SRC = main.c 

SRCS  = $(MAIN_SRC) \
        $(ARCH_DIR)/startup.c \
        $(KERNEL_DIR)/tasks.c \
        $(KERNEL_DIR)/list.c \
        $(KERNEL_DIR)/queue.c \
        $(KERNEL_DIR)/timers.c \
        $(KERNEL_DIR)/portable/MemMang/heap_4.c \
        $(KERNEL_DIR)/portable/GCC/ARM_CM3/port.c

SRCS  += drivers/uart.c

CFLAGS += -Idrivers
# --- Object File Logic ---
# This translates source paths into build-directory object paths
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

# --- Build Rules ---
all: $(BUILD_DIR)/telemetry.bin

# Generate the final binary
$(BUILD_DIR)/telemetry.bin: $(BUILD_DIR)/telemetry.elf
	$(OBJCOPY) -O binary $< $@

# Link the objects into an ELF
$(BUILD_DIR)/telemetry.elf: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	@echo "Build Complete: $@"

# Compile .c files into .o files (handles any subdirectory)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Utilities ---
clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/telemetry.elf
	qemu-system-arm -machine lm3s6965evb -nographic -kernel $<
