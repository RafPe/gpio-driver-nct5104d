# Custom GPIO driver for nct5104d
GPIO driver for nct5104d with PCEngines APU board support. This driver has been build by the need of customised home automation approach. 

I would like to make one big and important notice here *this is my first driver on linux* so it might be that some approaches could have been done better - contribution welcome :) 

Driver exposes registry access for shared devices configuration registries as well as GPIO control. Driver is created with only GPIO management hence it is by default using 
```
logical device id 0x07 
```

# Driver
I originally have build the driver to run on Centos machines. Therefore the master branch does contain the most recent build for that. If there are any other compilations then they should be listed as '\<DistroName>_\<kernel-ver>' in the branch tree i.e. 
```
CentOs_3.10.0-514.16.1.el7.x86_64
```

## Compiling driver 
Start by checking out the source code ( if you are using centos you should be good to pull master )
```
git clone git@github.com:RafPe/gpio-driver-nct5104d.git
```
Navigate to the directory 
```
cd gpio-driver-nct5104d
```
Run make file 
```
make 
```

Once completed you should be just able to stream it with the use of *insmod*
```
insmod nct5104d_gpio.ko
```

If everything went fine you will see your driver in the lodaded modules 
```
>  lsmod | grep nct5104d_gpio
Module                  Size  Used by
nct5104d_gpio          13230  0
```

Examing the *dmesg* to check device initialised properly
```
> dmesg | grep nct5104d_gpio:
[  363.782812] nct5104d_gpio: Initialiazing device ...
[  363.787826] nct5104d_gpio: Registered correctly with major number 248
[  363.800784] nct5104d_gpio: Succesfuly registered char device
[  363.809339] nct5104d_gpio: Found device with chip ID (APU)  : 0xc452
[  363.815803] nct5104d_gpio: Succesfuly registered platform device
```

You should also see device under the *\/dev* space
```
> ls -l /dev/nct*
crw-------. 1 root root 248, 0 Apr 29 18:07 /dev/nct5104d_gpio
```

## Debugging
If for whatever reason something would not be working as expected you can compile driver with debug mode enabled. 

For that you need to uncomment define in *nct5104d_gpio.h* file 
```
/*--------  uncomment below to enable debugging  --------*/

//#define DRIVER_DEBUG
```

Then in *dmesg* you will have much more output based on interactions you try with the driver.


# Pin management - nct5104dpin 
I have made the life bit easier by providng binary app for pin management called *nct5104dpin*.
In order to use it navigate to nct5104dpin folder and execute the following:
```
make 
make install
```
which should compile and install the application under the */usr/local/bin/nct5104dpin*

## Examples of pin management 
```
nct5104dpin [ --get|--set ] [--pin ] --val < 0|1 > --dir <direction_for_gpio>

Get a pin value: nct5104dpin --pin 7 
Get a pin value: nct5104dpin --get --pin 14
Set a pin value: nct5104dpin --set --pin 14 --val 1
Set a pin direction: nct5104dpin --set --pin 14 --dir out
```

## Return data example
```
nct5104dpin --pin 1 | jq
[
  {
    "pin": 1,
    "value": 0
  }
]
```


## Bulk modification of pin status
This is possible however with use of registry management. You can directly write desired values to specific rgistries using *nct5104dreg* app


# Registry management - nct5104dreg
I have made the life bit easier by providng binary app for pin management called *nct5104dreg*.
In order to use it navigate to nct5104dreg folder and execute the following:
```
make 
make install
```
which should compile and install the application under the */usr/local/bin/nct5104dreg*

## Examples of registry management 
```
nct5104dreg [ --get|--set ] [--reg ] <HEX> --val <DECIMAL> 

Get a reg value: nct5104dreg --reg 0x07 
Get a reg value: nct5104dreg --get --reg 0x07
Set a reg value: nct5104dreg --set --reg 0xE0 --val 252
```

## Return data example 
```
nct5104dreg --reg 0xE1 | jq
[
  {
    "registry": "0xe1",
    "value": 248
  }
]
```

# References 
* Originally based and inspired by [https://github.com/tasanakorn/linux-gpio-nct5104d](https://github.com/tasanakorn/linux-gpio-nct5104d)
