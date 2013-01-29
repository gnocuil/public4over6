obj-m += public4over6.o
public4over6-objs += mapping.o tunnel.o 

all: ioctl
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -f ioctl
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ioctl: tunnel.h main.c
	gcc -o ioctl main.c
	
