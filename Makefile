obj-m+=myChrDev.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) test.c -o read
	$(CC) write.c -o write
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm read 
	rm write