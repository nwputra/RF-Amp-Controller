# Name: Makefile
# Project: hid-mouse example
# Author: Christian Starkjohann
# Creation Date: 2008-04-07
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id$

DEVICE  = atmega168
CPU	= m168p
F_CPU   = 20000000	# in Hz


OBJ2HEX	=	/usr/bin/avr-objcopy 
PROG	=	/usr/bin/avrdude 
TARGET	=	ampctrl
VPATH	=	ampctrl
AVRDUDE	=	$(PROG) -c usbasp -P usb -p $(CPU) -B8
CFLAGS	= 	-I. -g -Os -Wall -mcall-prologues -mmcu=$(DEVICE) -Wl,-Map,${TARGET}.map 
OBJECTS = 	eeprom-data.o timer.o twi.o lcd.o adc.o linearize.o scheduler.o keyboard.o keymake.o keybreak.o hwio.o ampproc.o ampctrl.o
COMPILE = 	avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) 

# Internal 8 MHz
# FUSE_L=0xE2
# FUSE_H=0xDF
#
# Internal 8 MHz
# Do not erase EEPROM
# Output clock to PORTB for debugging of clock speed
# FUSE_L=0xA2
# FUSE_H=0xD7
#
# External 20 MHz
# No brownout detect
# Do not erase EEPROM
FUSE_L=0xEF
FUSE_H=0xD7
#
# External 20 MHz
# No brownout detect
# Do not erase EEPROM
# Output clock to PORTB for debugging of clock speed
# FUSE_L=0xAF
# FUSE_H=0xD7


# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex

program: flash fuse

# rule for programming fuse bits:
fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

# rule for uploading firmware:
flash: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

# rule for uploading eeprom:
eeprom: main.eep
	$(AVRDUDE) -U eeprom:w:main.eep:i

# rule for deleting dependent files (those which can be built by Make):
# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f *.hex *.lst *.obj *.cof *.list *.map *.eep.hex *.elf *.o *.s

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@ 
	# $(COMPILE) -c $< -o $@ -Wa,-ahls=$@.lst

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:


main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-objcopy -j .eeprom -O ihex --change-section-lma .eeprom=0 main.elf main.eep
	avr-size main.hex

# debugging targets:

disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
