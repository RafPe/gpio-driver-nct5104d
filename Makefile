obj-m+=nct5104d_gpio.o

all:
        make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
        make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
app-pin: 
        gcc -Wall -o nct5104dpin nct5104dpin.c
app-reg: 
        gcc -Wall -o nct5104dreg nct5104dreg.c
