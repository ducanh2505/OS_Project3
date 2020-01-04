obj-m+=myCharDev.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) test.c -o main
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
