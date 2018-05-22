# below two compilers, pick one you like

#CC = arm-linux-gnueabihf-gcc
CC = gcc

default:
	$(CC) -g3 -O0 -o fb fb.c

clean:
	rm fb
