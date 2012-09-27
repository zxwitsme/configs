ifneq ($(KERNELRELEASE),)
	obj-m := thread4.o
a:
	echo defined
else
	KDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
default:
	echo no defined
	$(MAKE) -C $(KDIR) M=$(PWD) modules
endif

clean:
	rm -f *.o *.ko *.mod.c *order *symvers
