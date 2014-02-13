obj-m = HRT.o Squeue.o

all:
	make -C /home/esp/Kernel_Source_Trees/Angstrom_Linux/kernel/kernel/ M=$(PWD) modules
	arm-angstrom-linux-gnueabi-gcc -o main_2 main_2.c -pthread

clean:
	make -C /home/esp/Kernel_Source_Trees/Angstrom_Linux/kernel/kernel/ M=$(PWD) clean
