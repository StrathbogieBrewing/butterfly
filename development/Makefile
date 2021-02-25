TARGET = butterfly
AVR_ARCHI = atmega169
CLK = 1000000L

SRC = $(wildcard *.c *.S *.s *.cpp)
OBJ = $(SRC:=.o)

CC = avr-gcc
CXX = avr-g++
GDB = avr-gdb
FLAGS = -g -std=gnu11 -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(AVR_ARCHI) -DF_CPU=$(CLK) -c
CXXFLAGS = -fno-threadsafe-statics
ASMFLAGS = -x assembler-with-cpp
LFLAGS = -Os -mmcu=$(AVR_ARCHI) -Wl,--gc-sections,--relax -Wl,--start-group -lm -Wl,--end-group
AVR_ARCHI_SIZE_FLAGS = --mcu=$(AVR_ARCHI) -C -d
OBJCPY = avr-objcopy
OBJCPY_FLAGS = -O ihex -R .eeprom

all: $(OBJ)
	$(CC) $^ $(LFLAGS) -o $(TARGET).elf
	$(OBJCPY) $(OBJCPY_FLAGS) $(TARGET).elf $(TARGET).hex

%.s.o  : %.s
	$(CC) $(ASMFLAGS) $(CFLAGS) $^ -o $@

%.S.o  : %.S
	$(CC) $(FLAGS) $^ -o $@

%.cpp.o: %.cpp
	$(CXX) $(FLAGS) $(CXXFLAGS) $^ -o $@

%.c.o  : %.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -f *.o *.d $(TARGET).elf $(TARGET).map $(TARGET).hex

.PHONY: program
program: $(TARGET).hex
	avrdude -C avrdude.conf -c avr109 -p m169 -P /dev/ttyUSB0  -b 9600 -U flash:w:$(TARGET).hex
