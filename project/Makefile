# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

all:gameC.elf gameA.elf

#additional rules for files
gameC.elf: ${COMMON_OBJECTS} main.o buzzer.o wdt_handler.o playSoundC.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

gameA.elf: ${COMMON_OBJECTS} main.o buzzer.o wdt_handler.o playSoundA.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

loadC: gameC.elf
	msp430loader.sh $^

loadA: gameA.elf
	msp430loader.sh $^

clean:
	rm -f *.o *.elf *.c~ *.h~ *.s~

buzzer.c: buzzer.h
main.c: buzzer.h
