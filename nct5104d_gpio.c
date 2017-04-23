#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>

#include "nct5104d_gpio.h"

MODULE_LICENSE("GPL");

static void my_device_reset(struct platform_data_ntc5104d* pdata);


/*--------  Platform data/platform and driver  --------*/

static struct platform_data_ntc5104d device_pdata_ntc5104d = 
{
 .chip_addr = NCT5104D_DEVICE_ADDR ,
 .num_gpio  = 16 ,
 .gpio_access_addr = NCT5104D_REG_BASE , 
 .reset = my_device_reset ,
};

/* Reset the device. */
static void my_device_reset(struct platform_data_ntc5104d* pdata)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}


static struct platform_device device_pdevice_ntc5104d = 
{
        .name           = DRIVER_NAME,
        .id             = 0,
        // .num_resources  = ARRAY_SIZE(sample_resources),
        // .resource       = sample_resources,
		.dev            = {
			.platform_data	= &device_pdata_ntc5104d
		},
};

static int ntc5104d_drv_probe(struct platform_device *pdev)
{
	struct platform_data_ntc5104d *pdata = dev_get_platdata(&pdev->dev);

	printk(KERN_ALERT "nct5104d_gpio: platform data - chip addr        : %d\n",pdata->chip_addr );
	printk(KERN_ALERT "nct5104d_gpio: platform data - chip addr        : 0x%04x\n",pdata->chip_addr);
	printk(KERN_ALERT "nct5104d_gpio: platform data - num GPIO         : %d\n",pdata->num_gpio);
	printk(KERN_ALERT "nct5104d_gpio: platform data - gpio access addr : 0x%04x\n",pdata->gpio_access_addr);
	printk(KERN_ALERT "nct5104d_gpio: platform data - gpio access addr : %d\n",pdata->gpio_access_addr);

	return 0;
}

static int ntc5104d_drv_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver ntc5104d_pldriver = {
    .probe          = ntc5104d_drv_probe,
    .remove         = ntc5104d_drv_remove,
    .driver = {
            .name  = DRIVER_NAME,
			.owner = THIS_MODULE,
    },
};

/*--------  CORE communication functions  --------*/
static int nct5104d_readw(int reg)
{
	int val;

	outb(reg++, NCT5104D_DEVICE_ADDR);
	val = inb(NCT5104D_DEVICE_ADDR + 1) << 8;
	outb(reg, NCT5104D_DEVICE_ADDR);
	val |= inb(NCT5104D_DEVICE_ADDR + 1);

	return val;
}
static inline int nct5104d_readb(int reg)
{
	outb(reg, NCT5104D_DEVICE_ADDR);
	return inb(NCT5104D_DEVICE_ADDR + 1);
}
static inline void nct5104d_writeb(int reg, int val)
{
	outb(reg, NCT5104D_DEVICE_ADDR);
	outb(val, NCT5104D_DEVICE_ADDR + 1);
}
static inline int nct5104d_efm_enable(void)
{
	if (!request_muxed_region(NCT5104D_DEVICE_ADDR, 2, DRIVER_NAME)) {
		pr_err(DRIVER_NAME "I/O address 0x%04x already in use\n", base);
		return -EBUSY;
	}

	outb(NCT5104D_EFM_ENABLE, NCT5104D_DEVICE_ADDR);
	outb(NCT5104D_EFM_ENABLE, NCT5104D_DEVICE_ADDR);

	return 0;
}

static inline void nct5104d_efm_disable(void)
{
	outb(NCT5104D_EFM_DISABLE, NCT5104D_DEVICE_ADDR);
	release_region(NCT5104D_DEVICE_ADDR, 2);
}


static inline void nct5104d_select_logical_device(int ld)
{
	outb(NCT5104D_REG_LDEVICE, NCT5104D_DEVICE_ADDR);
	outb(ld, NCT5104D_DEVICE_ADDR + 1);
}



/*--------  initialization  --------*/
void __init nct5104d_init_platform_data(void)
{
    /* Register "nct5104d platform device" with the OS. */
	platform_device_register(&device_pdevice_ntc5104d);
}

static int __init nct5104d_driver_init(void)
{
	int err;
	u8 val;
	nct5104d_init_platform_data();
	printk(KERN_ALERT "%s: registered platform device",DRIVER_NAME);

	platform_driver_probe(&ntc5104d_pldriver, ntc5104d_drv_probe); //TODO check return value

   	err = nct5104d_efm_enable();
	if (err)
		return err; // #TODO pointer error?
	printk(KERN_ALERT "nct5104d: EFM is now enabled ... \n");

	val = nct5104d_readb(0x61);
	printk(KERN_ALERT "nct5104d: value before is  0x%04x\n",val);


	nct5104d_writeb(0x61,NCT5104D_REG_BASE);
	printk(KERN_ALERT "nct5104d: wrote to device");

	val = nct5104d_readb(0x61);
	printk(KERN_ALERT "nct5104d: value after is  0x%04x\n",val);

    return 0;
}

/*--------  exit cleanup  --------*/
static void __exit nct5104d_driver_exit(void)
{
	printk(KERN_ALERT "nct5104d: unregistered platform device");
    return;
}

module_init(nct5104d_driver_init);
module_exit(nct5104d_driver_exit);