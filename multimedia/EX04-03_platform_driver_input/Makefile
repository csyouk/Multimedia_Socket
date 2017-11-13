obj-m := devtest.o

KERNELDIR := ~/work/linux-3.4.39
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-none-linux-gnueabi-gcc -o inputtest inputtest.c
	sudo cp devtest.ko inputtest /nfsroot

clean:
	rm -f *.o *.ko *.mod.c modules.order Module.symvers
	rm -f .*.*.cmd .*.*.*.cmd
	rm -rf .tmp_versions
	rm -f inputtest

app:
	arm-none-linux-gnueabi-gcc -o inputtest inputtest.c

