#KERNDIR = /usr/src/linux

obj-m += iz.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) iztest.c -o test	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm test

