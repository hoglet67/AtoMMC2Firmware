# Paths that will need changing
SREC_CAT=srec_cat
GAWK=gawk

# AVR dev environment
MCU=atmega103
F_CPU=16000000
CC=avr-gcc
OBJCOPY=avr-objcopy

# Use these flags to include SDDOS (missing out serial console)
CFLAGS=-DF_CPU=${F_CPU}UL -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues

# Use these flags to enable serial console (missing out SDDOS)
# CFLAGS=-DF_CPU=${F_CPU}UL -DSERIAL_STATUS -DCOOKED_SERIAL -DNOUSART1 -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues

OBJECTS=AVRFirmware.o atomio.o status.o ff.o diskio.o mmcio.o atmmc2core.o atmmc2wfn.o wildcard.o

avr_progmem.mem: avr_progmem.hex
	$(SREC_CAT)  $< -Intel -Byte_Swap 2  -Data_Only -o tmp.mem -vmem 8
	$(GAWK) ' BEGIN{FS=" ";} { $$1= ""; print}' tmp.mem > $@
	rm tmp.mem

avr_progmem.hex : avr_progmem.out 
	$(OBJCOPY) -R .eeprom -O ihex avr_progmem.out avr_progmem.hex 
avr_progmem.out : $(OBJECTS)
	$(CC) $(CFLAGS) -o avr_progmem.out -Wl,-Map,avr_progmem.map $^
%.o : %.c 
	$(CC) $(CFLAGS) -Os -c $<
%.o : %.S
	$(CC) $(CFLAGS) -Os -c $<

.phony: clean

clean:
	rm -f avr_progmem.mem avr_progmem.hex avr_progmem.out avr_progmem.map *.o

