obj-m += public4over6.o

all: ioctl
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

ioctl : ioctl.c
	gcc -o ioctl ioctl.c

clean:
	rm -f ioctl
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


