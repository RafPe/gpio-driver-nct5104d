#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>


MODULE_LICENSE("GPL");


#define DRIVER_NAME "nct5104d"

#define NCT5104D_BUS_ADDR               0x2E
#define NCT5104D_LDEV_SELECT     		0x07	
#define NCT5104D_SUPERIO_ENABLE	    	0x87	
#define NCT5104D_SUPERIO_DISABLE		0xAA	



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
	if (!request_muxed_region(base, 2, DRVNAME)) {
		pr_err(DRVNAME "I/O address 0x%04x already in use\n", base);
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
/**************/  

int ourinitmodule(void)
{
    printk(KERN_ALERT "\n RAFTECH: Welcome to sample Platform driver.... \n");
    return 0;
}

void ourcleanupmodule(void)
{
    printk(KERN_ALERT "\n Thanks....Exiting sample Platform driver... \n");
    return;
}

module_init(ourinitmodule);
module_exit(ourcleanupmodule);