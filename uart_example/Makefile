CC = xtensa-lx106-elf-gcc
CFLAGS = -I/home/abhi/Projects/esp-open-sdk/include -L/home/abhi/Projects/esp-open-sdk/lib -mlongcalls -I.
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lgcc
#LDFLAGS = -Teagle.app.v6.ld

blinky-0x00000.bin: blinky
	esptool.py elf2image $^

blinky: blinky.o uart.o 

%.o: %.c
	$(CC)  $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ -c $<

flash: blinky-0x00000.bin
	esptool.py write_flash 0 blinky-0x00000.bin 0x40000 blinky-0x40000.bin

clean:
	rm -f blinky blinky.o blinky-0x00000.bin blinky-0x40000.bin
