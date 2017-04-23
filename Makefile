obj-m+=nct5104d_gpio.o

all:
        make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
        make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
app: 
        gcc -Wall -o nct5104dctl nct5104dctl.c
