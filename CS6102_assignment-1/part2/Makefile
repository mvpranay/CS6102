PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

TARGET = multiplier

# Compiler flags
CFLAGS = -mcpu=cortex-m4
CFLAGS += -mthumb
CFLAGS += -mfloat-abi=soft
CFLAGS += -O2
CFLAGS += -Wall -Wextra
CFLAGS += -g
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS = -T stm32.ld -nostartfiles -Wl,--gc-sections -Wl,-Map=$(TARGET).map
# Source Files
SRCS = benchmark.c
SRCS += startup.c
SRCS += syscalls.c
SRCS += multiplier.c


# Object files to be built from SRCS
OBJS = $(SRCS:.c=.o)

all: $(TARGET).elf $(TARGET).bin size

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link:
$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) multiplier_optimized.o -lm -o $@

# Create binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Print size
size: $(TARGET).elf
	@echo "Size information:"
	@$(SIZE) $(TARGET).elf
	
clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).map
	
# Run in QEMU
run: $(TARGET).elf
	qemu-system-arm -M lm3s6965evb -cpu cortex-m4 -nographic -kernel $(TARGET).elf
	


.PHONY: all clean run debug size