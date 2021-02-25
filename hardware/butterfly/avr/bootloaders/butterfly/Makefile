TARGET = butterfly
AVR_ARCHI = atmega169
CLK = 1000000L

SRC = $(wildcard *.c *.S *.s *.cpp)
OBJ = $(SRC:=.o)

CC = avr-gcc
CXX = avr-g++
GDB = avr-gdb
FLAGS = -g -std=gnu99 -Os -Wall  -mmcu=$(AVR_ARCHI) -DF_CPU=$(CLK) -c
CXXFLAGS = -fno-threadsafe-statics
ASMFLAGS = -x assembler-with-cpp

CFLAGS += -Wall -gdwarf-2 -std=gnu11 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF $(@F).d

#LFLAGS = -Os -mmcu=$(AVR_ARCHI) -Wl,--gc-sections,--relax -Wl,--start-group -lm -Wl,--end-group
LFLAGS = -nodefaultlibs -nostartfiles  -Wl,--section-start=.text=0x3C00 -Wl,-Map=$(TARGET).map -mmcu=$(AVR_ARCHI)
AVR_ARCHI_SIZE_FLAGS = --mcu=$(AVR_ARCHI) -C -d
OBJCPY = avr-objcopy
OBJCPY_FLAGS = -O ihex -R .eeprom

all: $(OBJ)
	$(CC) $^ $(LFLAGS) -o $(TARGET).elf
	$(OBJCPY) $(OBJCPY_FLAGS) $(TARGET).elf $(TARGET).hex

	@echo
	@avr-size -C --mcu=$(AVR_ARCHI) ${TARGET}.elf

	@avr-objdump -h -S $(TARGET).elf > $(TARGET).lss


%.s.o  : %.s
	$(CC) $(ASMFLAGS) $(CFLAGS) $^ -o $@

%.S.o  : %.S
	$(CC) $(FLAGS) $(ASMFLAGS) $(CFLAGS) $^ -o $@

%.cpp.o: %.cpp
	$(CXX) $(FLAGS) $(CXXFLAGS) $^ -o $@

%.c.o  : %.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -f *.o *.d $(TARGET).elf $(TARGET).map $(TARGET).hex $(TARGET).lss

# LOCK writing to Bootloader from Application
LFUSE = 0x62

# change HFUSE for 512 bytes of boot memory and move reset vector to boot memory

HFUSE = 0xD2

LOCK = 0x2F

.PHONY: program
program: $(TARGET).hex
	avrdude -C ../avrdude.conf -c stk500v2 -P usb -p m169 -U flash:w:$(TARGET).hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCK):m
