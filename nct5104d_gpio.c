#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/version.h>
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/delay.h>
#include <linux/mutex.h>	         /// Required for the mutex functionality


#include "nct5104d_gpio.h"
#include "query_ioctl.h"

static DEFINE_MUTEX(ebbchar_mutex);


#define FIRST_MINOR 0
#define MINOR_CNT 1

static int 		gpio_access_addr = NCT5104D_DGA_GSR ;
static int    	majorNumber;                  

static dev_t dev;
static struct class *cl;

module_param(gpio_access_addr, int, 0644);
MODULE_PARM_DESC(gpio_access_addr, "GPIO direct access address");



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

static inline void nct5104d_writeb(int reg, u8 val)
{
	outb(reg, NCT5104D_DEVICE_ADDR);
	outb(val, NCT5104D_DEVICE_ADDR + 1);
}

static inline int nct5104d_efm_enable(void)
{
	if (!request_muxed_region(NCT5104D_DEVICE_ADDR, 2, DRIVER_NAME)) {
		pr_err(DRIVER_NAME "I/O address 0x%04x already in use\n", NCT5104D_DEVICE_ADDR);
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
	nct5104d_writeb(NCT5104D_REG_LDEVICE, ld);
}

static inline int nct5104d_get_logical_device(void)
{
	return nct5104d_readb(NCT5104D_REG_LDEVICE);
}

static inline void nct5104d_soft_reset(void)
{
	nct5104d_writeb(NCT5104D_REG_SOFT_RESET, 1);
}


/*--------  Character device  --------*/
static int nct5104d_cdev_open(struct inode *i, struct file *f)
{
	if(!mutex_trylock(&ebbchar_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
											/// returns 1 if successful and 0 if there is contention
		printk(KERN_ALERT "nct5104d_gpio: Device in use by another process");
		return -EBUSY;
	}

    return 0;
}
static int nct5104d_cdev_close(struct inode *i, struct file *f)
{
	mutex_unlock(&ebbchar_mutex);          /// Releases the mutex (i.e., the lock goes up)
    return 0;
}

static long nct5104d_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    gpio_arg_t q_gpio;
    nct5104dctl_arg_t q_ctl;

	printk(KERN_INFO "nct5104d_gpio: [DEBUG] received cmd => %d\n", cmd);

    switch (cmd)
    {
        case IOCTL_GET_REG:
            if (copy_from_user(&q_ctl, (nct5104dctl_arg_t *)arg, sizeof(nct5104dctl_arg_t)))
            {
                return -EACCES;
            }			

			if( ( q_ctl.registry > 255) || (q_ctl.registry) < 0)  return -EINVAL;

			q_ctl.value = nct5104d_readb(q_ctl.registry);
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] registry is  => %d\n", q_ctl.registry);
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] value is  => %d\n", q_ctl.value);

            if (copy_to_user((nct5104dctl_arg_t *)arg, &q_ctl, sizeof(nct5104dctl_arg_t)))
            {
                return -EACCES;
            }
            break;
        // case QUERY_CLR_VARIABLES:
        //     status = 0;
        //     dignity = 0;
        //     ego = 0;
        //     break;
        // case QUERY_SET_VARIABLES:
        //     if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
        //     {
        //         return -EACCES;
        //     }
        //     status = q.status;
        //     dignity = q.dignity;
        //     ego = q.ego;
        //     break;
        default:
            return -EINVAL;
    }
 
    return 0;
}

static struct file_operations nct5104d_driver_fops = 
{
    .owner = THIS_MODULE,
    .open = nct5104d_cdev_open,
    .release = nct5104d_cdev_close,
    .unlocked_ioctl = nct5104d_ioctl,
};

static int nct5104d_cdev_register(void)
{
    struct device *dev_ret;
 
	majorNumber = register_chrdev(0, DRIVER_NAME, &nct5104d_driver_fops);
	if (majorNumber<0){
		printk(KERN_ALERT "nct5104d_gpio: Failed to register major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "nct5104d_gpio: Registered correctly with major number %d\n", majorNumber);

    if (IS_ERR(cl = class_create(THIS_MODULE, "nct5104d")))
    {
        unregister_chrdev_region(dev, MINOR_CNT);
		printk(KERN_ALERT "nct5104d_gpio: Failed to register class\n");
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, MKDEV(majorNumber, 0), NULL, DRIVER_NAME)))
    {
        class_destroy(cl);
        unregister_chrdev_region(dev, MINOR_CNT);
		printk(KERN_ALERT "nct5104d_gpio: Failed to create device\n");
        return PTR_ERR(dev_ret);
    }

	printk(KERN_ALERT "nct5104d_gpio: Succesfuly registered char device\n");

	return 0;

}


/*--------  Platform data/platform and driver  --------*/

/* Reset the device. */
static void nct5104d_gpio_get_pin(struct platform_data_ntc5104d* pdata,u8 pin)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}

/* Reset the device. */
//TODO -----> Write support for both GPIO ports at once!
static void nct5104d_gpio_set_pin(struct platform_data_ntc5104d* pdata,u8 pin,u8 state)
{
    printk(KERN_ALERT " %s\n", __FUNCTION__);
}

static struct platform_data_ntc5104d device_pdata_ntc5104d = 
{
 .chip_addr = NCT5104D_DEVICE_ADDR ,
 .num_gpio  = 16 ,
 .get_pin = nct5104d_gpio_get_pin ,
 .set_pin = nct5104d_gpio_set_pin ,
};


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
	static int res,val;

	struct platform_data_ntc5104d *pdata = dev_get_platdata(&pdev->dev);

	printk(KERN_ALERT "nct5104d_gpio: platform data - chip addr        : 0x%02x\n",pdata->chip_addr);
	printk(KERN_ALERT "nct5104d_gpio: platform data - num GPIO         : %d\n",pdata->num_gpio);
	printk(KERN_ALERT "nct5104d_gpio: platform data - gpio access addr : 0x%02x\n",gpio_access_addr);
	
	nct5104d_soft_reset();
    mdelay(50);

   	res = nct5104d_efm_enable();
	if (res)
		return res; 
	printk(KERN_ALERT "nct5104d_gpio: EFM is now enabled ... \n");


	/**
	*
	* This part is responsible for configuration
	* of direct GPIO access 
	*/
	val = 0;
	val = nct5104d_get_logical_device();
	printk(KERN_ALERT "nct5104d_gpio: Currently selected logical device 0x%04x\n",val);

	nct5104d_select_logical_device(NCT5104D_LDEVICE_8);
	printk(KERN_ALERT "nct5104d_gpio: Switched logical device ... \n");

	val = 0;
	val = nct5104d_get_logical_device();
	printk(KERN_ALERT "nct5104d_gpio: Currently selected logical device 0x%04x\n",val);

	//TODO implement proper address config with 16byte
	nct5104d_writeb(NCT5104D_REG_GPIO_BASEADDR_L,gpio_access_addr);
	val = nct5104d_readw(NCT5104D_REG_GPIO_BASEADDR_H);
	printk(KERN_ALERT "nct5104d_gpio: DGA base configured to 0x%04x\n",val);



	// Select GPIO1 & set all to output
	nct5104d_writeb(NCT5104D_DGA_GSR, 1);
	nct5104d_writeb(NCT5104D_DGA_IO,255);

	// read current data state 
    val = 0;
	val = nct5104d_readb(NCT5104D_DGA_DATA);
	printk(KERN_ALERT "nct5104d_gpio: DGA[1] data is  0x%02x\n",val);
	
	// write 255 to all pins 
	nct5104d_writeb(NCT5104D_DGA_DATA,255);

	// read current data state 
    val = 0;
	val = nct5104d_readb(NCT5104D_DGA_DATA);
	printk(KERN_ALERT "nct5104d_gpio: DGA[1] data is now....  0x%02x\n",val);



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


/*--------  initialization  --------*/
void __init nct5104d_init_platform_data(void)
{
    /* Register "nct5104d platform device" with the OS. */
	platform_device_register(&device_pdevice_ntc5104d);


	//TODO - implement the following registration for platform device

	// 	pdev = platform_device_alloc("mydev", id);
	// if (pdev) {
	// 	err = platform_device_add_resources(pdev, &resources,
	// 					    ARRAY_SIZE(resources));
	// 	if (err == 0)
	// 		err = platform_device_add_data(pdev, &platform_data,
	// 					       sizeof(platform_data));
	// 	if (err == 0)
	// 		err = platform_device_add(pdev);
	// } else {
	// 	err = -ENOMEM;
	// }
	// if (err)
	// 	platform_device_put(pdev);
}

static int __init nct5104d_driver_init(void)
{
	int res=0;
	
	printk(KERN_ALERT "nct5104d_gpio: Initialiazing device ... \n");

	mutex_init(&ebbchar_mutex);       /// Initialize the mutex lock dynamically at runtime

	res = nct5104d_cdev_register();
	if (res)
		return res; 

	nct5104d_init_platform_data();
	printk(KERN_ALERT "nct5104d_gpio: Succesfuly registered platform device\n");

	platform_driver_probe(&ntc5104d_pldriver, ntc5104d_drv_probe);

    return 0;
}

/*--------  exit cleanup  --------*/
static void __exit nct5104d_driver_exit(void)
{
	printk(KERN_ALERT "nct5104d: unregistered platform device");

	device_destroy(cl, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(cl);                          // unregister the device class
	class_destroy(cl);                             // remove the device class
	unregister_chrdev(majorNumber, DRIVER_NAME);             // unregister the major number

	platform_driver_unregister(&ntc5104d_pldriver);
	platform_device_unregister(&device_pdevice_ntc5104d);

	mutex_destroy(&ebbchar_mutex);        /// destroy the dynamically-allocated mutex
    return;
}

module_init(nct5104d_driver_init);
module_exit(nct5104d_driver_exit);



MODULE_AUTHOR("RafPe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Define param gor GPIO access");
MODULE_VERSION("0.3");