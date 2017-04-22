#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>


MODULE_LICENSE("GPL");


#define DRIVER_NAME "x-gpio-nct5104d"

#define NCT5104D_DEVICE_ADDR            0x2E
#define NCT5104D_LDEV_SELECT     		0x07	
#define NCT5104D_SUPERIO_ENABLE	    	0x87	
#define NCT5104D_SUPERIO_DISABLE		0xAA	

#define NCT5104D_REG_BASE               0x67
#define NCT5104D_REG_GPIO_IO            ( NCT5104D_REG_BASE_ADDR +1 )
#define NCT5104D_REG_GPIO_DATA          ( NCT5104D_REG_BASE_ADDR +2 )
#define NCT5104D_REG_GPIO_INVERSION     ( NCT5104D_REG_BASE_ADDR +3 )
#define NCT5104D_REG_GPIO_STATUS        ( NCT5104D_REG_BASE_ADDR +4 )


/* Power on the device. */
static void my_device_power_on(struct my_device_platform_data* pdata)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}

/* Power Off the device. */
static void my_device_power_off(struct my_device_platform_data* pdata)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}

/* Reset the device. */
static void my_device_reset(struct my_device_platform_data* pdata)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}

/* Data structure for the platform data of ntc chip*/
struct platform_data_ntc5104d {
 int reset_gpio;
 int power_on_gpio;
 void (*power_on)(struct platform_data_ntc5104d* ppdata);
 void (*power_off)(struct platform_data_ntc5104d* ppdata);
 void (*reset)(struct platform_data_ntc5104d* pdata);
};

/* "ntc5104d device" platform data */
static struct platform_data_ntc5104d device_pdata_ntc5104d = {
 .reset_gpio = 100,
 .power_on_gpio = 101,
 .power_on = my_device_power_on,
 .power_off = my_device_power_off,
 .reset = my_device_reset
};

/**************/ 
static int sample_drv_probe(struct platform_device *pdev){
}
static int sample_drv_remove(struct platform_device *pdev){
}

static struct platform_driver sample_pldriver = {
    .probe          = sample_drv_probe,
    .remove         = sample_drv_remove,
    .driver = {
            .name  = DRIVER_NAME,
    },
};



/* Specifying my resources information */
static struct resource sample_resources[] = {
        {
                .start          = RESOURCE1_START_ADDRESS,
                .end            = RESOURCE1_END_ADDRESS,
                .flags          = IORESOURCE_MEM,
        },
        {
                .start          = RESOURCE2_START_ADDRESS,
                .end            = RESOURCE2_END_ADDRESS,
                .flags          = IORESOURCE_MEM,
        },
    {
                .start          = SAMPLE_DEV_IRQNUM,
                .end            = SAMPLE_DEV_IRQNUM,
                .flags          = IORESOURCE_IRQ,
        }

    };    

static struct platform_device sample_device = {
        .name           = DRIVER_NAME,
        .id             = -1,
        .num_resources  = ARRAY_SIZE(sample_resources),
        .resource       = sample_resources,
};




/*--------  CORE functions  --------*/

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


// enable_my_superio_dev() {
// 	request_muxed_region(0x44, 0x02, "superio:watchdog");
// 	outb() ..sequence to enable chip
// }

// disable_my_superio_dev() {
// 	outb() .. sequence of disable chip
// 	release_region(0x44, 0x02);
// }


/**************/  

int ourinitmodule(void)
{
	int err;
	u8 val;

    printk(KERN_ALERT "\n RAFTECH: Welcome to sample Platform driver.... \n");

   	err = nct5104d_enable( NCT5104D_DEVICE_ADDR );
	if (err)
		return err; // #TODO pointer error?


	val = nct5104d_readb( NCT5104D_DEVICE_ADDR,0x61);
	printk(KERN_ALERT "RAFTECH: value before is  0x%04x\n",val);


	nct5104d_writeb( NCT5104D_DEVICE_ADDR,0x61,0x67);
	printk(KERN_ALERT "RAFTECH: wrote to device");

	val = nct5104d_readb( NCT5104D_DEVICE_ADDR,0x61);
	printk(KERN_ALERT "RAFTECH: value after is  0x%04x\n",val);
	



    return 0;
}

void ourcleanupmodule(void)
{
    printk(KERN_ALERT "\n Thanks....Exiting sample Platform driver... \n");
    return;
}

module_init(ourinitmodule);
module_exit(ourcleanupmodule);