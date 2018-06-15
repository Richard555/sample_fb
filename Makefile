# below two compilers, pick one you like

#CC ?= arm-linux-gnueabihf-gcc
CC ?= gcc

default:
	$(CC) -g3 -O0 -o fb  fb.c
	$(CC) -g3 -O0 -o fb2 fb2.c
	$(CC) -g3 -O0 -o fb3 fb3.c

clean:
	rm fb
	rm fb2

