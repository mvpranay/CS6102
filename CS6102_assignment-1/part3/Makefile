# Toolchain
PREFIX   = arm-none-eabi-
CC       = $(PREFIX)gcc
OBJCOPY  = $(PREFIX)objcopy
SIZE     = $(PREFIX)size
GDB      = gdb-multiarch

# Target
TARGET = mayo_benchmark

# ===== CPU / ABI =====
CPU = cortex-m3

# Compiler flags
CFLAGS  = -mcpu=$(CPU)
CFLAGS += -mthumb
CFLAGS += -mfloat-abi=soft
CFLAGS += -Wall -Wextra
CFLAGS += -g
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -nostdlib

# Linker flags
LDFLAGS  = -T lm3s6965.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(TARGET).map

# Source files
SRCS  = startup.c
SRCS += benchmark.c
SRCS += mayo.c
SRCS += syscalls.c
SRCS += shake256.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET).elf $(TARGET).bin size

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

# Create binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Print size
size: $(TARGET).elf
	@echo "Size information:"
	@$(SIZE) $<

clean:
	rm -f *.o *.elf *.bin *.map

run: $(TARGET).elf 
	qemu-system-arm \
	-M lm3s6965evb \
	-cpu cortex-m3 \
	-nographic \
	-semihosting \
	-icount shift=0,align=off \
	-kernel mayo_benchmark.elf

# Debug with GDB
debug: $(TARGET).elf
	qemu-system-arm \
		-M lm3s6965evb \
		-cpu cortex-m3 \
		-nographic \
		-kernel $< \
		-s -S &
	$(GDB) $< -ex "target remote :1234"

.PHONY: all clean run debug size
