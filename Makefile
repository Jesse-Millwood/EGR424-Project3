# Modify as appropriate
STELLARISWARE=/home/jesse/StellarisWare
GNUPATH=/home/jesse/CodeSourcery/Sourcery_G++_Lite_2010/bin
OCDBOARDSCRIPT = /usr/local/share/openocd/scripts/board/ek-lm3s6965.cfg

BINPATH = ./gcc
TARGETNAME=lockdemo
PROGRAMSTRING = program $(BINPATH)/$(TARGETNAME).elf verify reset exit

CC=$(GNUPATH)/arm-none-eabi-gcc -Wall -Os -march=armv7-m -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd -Wl,--gc-sections

all: elf

elf: lockdemo.c syscalls.c startup_gcc.c threads.c create.S rit128x96x4.c
	mkdir -p $(BINPATH)
	${CC} -o $(BINPATH)/$(TARGETNAME).elf -I${STELLARISWARE} -L${STELLARISWARE}/driverlib/gcc-cm3 -Tlinkscript.x -Wl,-Map,lockdemo.map -Wl,--entry,ResetISR lockdemo.c startup_gcc.c syscalls.c rit128x96x4.c create.S threads.c -ldriver-cm3

flash: 
	sudo openocd -f $(OCDBOARDSCRIPT) -c "$(PROGRAMSTRING)"
sim:
	qemu-system-arm -machine lm3s6965evb -kernel $(BINPATH)/$(TARGETNAME).elf -m 128M

.PHONY: clean
clean:
	rm -f *.elf *.map

# vim: noexpandtab  
