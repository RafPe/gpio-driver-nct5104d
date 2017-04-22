#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>

#include "nct5104d_gpio.h"

MODULE_LICENSE("GPL");

static struct platform_data_ntc5104d device_pdata_ntc5104d = {
 .chip_addr = NCT5104D_DEVICE_ADDR ,
 .num_gpio  = 16 ,
 .gpio_access_addr = NCT5104D_REG_BASE , 
};

static struct platform_device device_pdevice_ntc5104d = {
        .name           = DRIVER_NAME,
        .id             = 0,
        // .num_resources  = ARRAY_SIZE(sample_resources),
        // .resource       = sample_resources,
		.dev            = {
			.platform_data	= &device_pdata_ntc5104d
		},
};


/*--------  CORE communication functions  --------*/
static int nct5104d_readw(int base, int reg)
{
	int val;

	outb(reg++, base);
	val = inb(base + 1) << 8;
	outb(reg, base);
	val |= inb(base + 1);

	return val;
}
static inline int nct5104d_readb(int base, int reg)
{
	outb(reg, base);
	return inb(base + 1);
}
static inline void nct5104d_writeb(int base, int reg, int val)
{
	outb(reg, base);
	outb(val, base + 1);
}
static inline int nct5104d_enable(int base)
{
	if (!request_muxed_region(base, 2, DRIVER_NAME)) {
		pr_err(DRIVER_NAME "I/O address 0x%04x already in use\n", base);
		return -EBUSY;
	}

	outb(NCT5104D_SUPERIO_ENABLE, base);
	outb(NCT5104D_SUPERIO_ENABLE, base);

	return 0;
}

static inline void nct5104d_select(int base, int ld)
{
	outb(NCT5104D_LDEV_SELECT, base);
	outb(ld, base + 1);
}

static inline void nct5104d_disable(int base)
{
	outb(NCT5104D_SUPERIO_DISABLE, base);
	release_region(base, 2);
}

/*--------  initialization  --------*/
static int __init nct5104d_driver_init(void)
{
	int err;
	u8 val;

    printk(KERN_ALERT "\n RAFTECH: Welcome to sample Platform driver.... \n");

	retval = platform_device_register(&device_pdevice_ntc5104d);
	if (retval)
		return retval; // #TODO pointer error?
	printk(KERN_ALERT "nct5104d:registered platform device device");

   	err = nct5104d_enable( NCT5104D_DEVICE_ADDR );
	if (err)
		return err; // #TODO pointer error?
	printk(KERN_ALERT "nct5104d: is now enabled ... \n");

	val = nct5104d_readb( NCT5104D_DEVICE_ADDR,0x61);
	printk(KERN_ALERT "nct5104d: value before is  0x%04x\n",val);


	nct5104d_writeb( NCT5104D_DEVICE_ADDR,0x61,0x67);
	printk(KERN_ALERT "nct5104d: wrote to device");

	val = nct5104d_readb( NCT5104D_DEVICE_ADDR,0x61);
	printk(KERN_ALERT "nct5104d: value after is  0x%04x\n",val);
	



    return 0;
}


/*--------  exit cleanup  --------*/
static void __exit nct5104d_driver_exit(void)
{
    printk(KERN_ALERT "\n Thanks....Exiting sample Platform driver... \n");
    return;
}

module_init(nct5104d_driver_init);
module_exit(nct5104d_driver_exit);