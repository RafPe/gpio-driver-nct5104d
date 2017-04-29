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
#include <linux/mutex.h>	      // Required for the mutex functionality


#include "nct5104d_gpio.h"

static DEFINE_MUTEX(nct5104d_mutex);

static int 		gpio_access_addr = NCT5104D_DGA_GSR ;
static int    	majorNumber;                  

static dev_t dev;
static struct class *cl;

// module_param(gpio_access_addr, int, 0644);
// MODULE_PARM_DESC(gpio_access_addr, "GPIO direct access address");

#define NCT5104D_GPIO_BANK(_id, _ngpio, _regbase)					\
	{																\
		.set_dir  				= nct5104d_gpio_dir_set,			\
		.get_pin              	= nct5104d_gpio_pin_get,			\
		.set_pin              	= nct5104d_gpio_pin_set,			\	
		.num_gpio				= _ngpio,							\
		.regbase 				= _regbase,							\
		.id 					= _id,								\		
	}

/*--------  array for our GPIO banks  --------*/
static struct nct5104d_gpio_bank nct5104d_gpio_bank[] = {
	NCT5104D_GPIO_BANK(0, 8, 0xE0),
	NCT5104D_GPIO_BANK(1, 8, 0xE4)
};


static struct platform_driver nct5104d_pldriver = {
    .probe          = nct5104d_drv_probe,
    .remove         = nct5104d_drv_remove,
    .driver = {
            .name  = DRIVER_NAME,
			.owner = THIS_MODULE,
    },
};

static struct file_operations nct5104d_driver_fops = {
    .owner = THIS_MODULE,
    .open = nct5104d_cdev_open,
    .release = nct5104d_cdev_close,
    .unlocked_ioctl = nct5104d_ioctl,
};

static struct platform_data_nct5104d device_pdata_nct5104d = {
 .chip_addr = NCT5104D_DEVICE_ADDR ,
 .num_gpio  = 16 ,
};

static struct platform_device device_pdevice_nct5104d = {
        .name           = DRIVER_NAME,
        .id             = 0,
        // .num_resources  = ARRAY_SIZE(sample_resources),
        // .resource       = sample_resources,
		.dev            = {
			.platform_data	= &device_pdata_nct5104d,
			.release 		= nct5104d_device_release,	
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

static inline void nct5104d_writeb(int reg, unsigned val)
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


/*--------  GPIO management  --------*/

static void nct5104d_gpio_pin_get(gpio_arg_t * gpioctl){
	// u8 val;

	// nct5104d_select_logical_device(NCT5104D_LDEVICE_GPIO);

	// /*--------  GPIO1 register[8:15]  --------*/
	// if(pin > 7)
	// {
	// 	return nct5104d_readb(NCT5104D_REG_GPIO1_DATA);
	// }

	// /*--------  GPIO0 register[0:7]  --------*/
	// if(pin < 8)
	// {
	// 	return nct5104d_readb(NCT5104D_REG_GPIO0_DATA);
	// }
}

static void nct5104d_gpio_pin_set(gpio_arg_t * gpioctl){
	// u8 val;

	// nct5104d_select_logical_device(NCT5104D_LDEVICE_GPIO);

	// /*--------  GPIO1 register[8:15]  --------*/
	// if(pin > 7)
	// {
	// 	val = nct5104d_readb(NCT5104D_REG_GPIO1_DATA);
		
	// 	if (direction)
	// 	{
	// 		val |= (1 << pin);
	// 	}
	// 	else
	// 	{
	// 		val &= ~(1 << pin);
	// 	}

	// 	nct5104d_writeb(NCT5104D_REG_GPIO1_DATA,val); // ALL 0 == ALL OUT
	// }

	// /*--------  GPIO0 register[0:7]  --------*/
	// if(pin < 8)
	// {
	// 	val = nct5104d_readb(NCT5104D_REG_GPIO0_DATA);

	// 	if (direction)
	// 	{
	// 		val |= (1 << pin);
	// 	}
	// 	else
	// 	{
	// 		val &= ~(1 << pin);
	// 	}

	// 	nct5104d_writeb(NCT5104D_REG_GPIO0_DATA,val); // ALL 0 == ALL OUT
	// }
}

static void nct5104d_gpio_dir_set(gpio_arg_t * gpioctl){
	// u8 val;

	// nct5104d_select_logical_device(NCT5104D_LDEVICE_GPIO);

	// /*--------  GPIO1 register[8:15]  --------*/
	// if(pin > 7)
	// {
	// 	val = nct5104d_readb(NCT5104D_REG_GPIO1_IO);
		
	// 	if (direction)
	// 	{
	// 		val |= (1 << pin);
	// 	}
	// 	else
	// 	{
	// 		val &= ~(1 << pin);
	// 	}

	// 	nct5104d_writeb(NCT5104D_REG_GPIO1_IO,val); // ALL 0 == ALL OUT
	// }

	// /*--------  GPIO0 register[0:7]  --------*/
	// if(pin < 8)
	// {
	// 	val = nct5104d_readb(NCT5104D_REG_GPIO0_IO);

	// 	if (direction)
	// 	{
	// 		val |= (1 << pin);
	// 	}
	// 	else
	// 	{
	// 		val &= ~(1 << pin);
	// 	}

	// 	nct5104d_writeb(NCT5104D_REG_GPIO0_IO,val); // ALL 0 == ALL OUT
	// }

}



/*--------  Character device  --------*/
static int nct5104d_cdev_open(struct inode *i, struct file *f)
{
	if(!mutex_trylock(&nct5104d_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
											/// returns 1 if successful and 0 if there is contention
		printk(KERN_ALERT "nct5104d_gpio: Device in use by another process");
		return -EBUSY;
	}

    return 0;
}
static int nct5104d_cdev_close(struct inode *i, struct file *f)
{
	mutex_unlock(&nct5104d_mutex);          /// Releases the mutex (i.e., the lock goes up)
    return 0;
}

static long nct5104d_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int err;

    gpio_arg_t q_gpio;
    nct5104dctl_arg_t q_ctl;


	
	/*--------  enable EFM for all interactions  --------*/	
	err = nct5104d_efm_enable();
	if (err)
		return err;

    switch (cmd)
    {
        case IOCTL_GET_REG:
            if (copy_from_user(&q_ctl, (nct5104dctl_arg_t *)arg, sizeof(nct5104dctl_arg_t)))
            {
                return -EACCES;
            }			

			if( ( q_ctl.registry > 255) || (q_ctl.registry) < 0)  return -EINVAL;
			q_ctl.value = nct5104d_readb(q_ctl.registry);

			printk(KERN_INFO "nct5104d_gpio: [DEBUG] received cmd     => IOCTL_GET_REG ");
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] Query registry   => 0x%02x\n", q_ctl.registry);
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] Return value is  => %d\n", q_ctl.value);

            if (copy_to_user((nct5104dctl_arg_t *)arg, &q_ctl, sizeof(nct5104dctl_arg_t)))
            {
                return -EACCES;
            }
            break;
        case IOCTL_SET_REG:
			if (copy_from_user(&q_ctl, (nct5104dctl_arg_t *)arg, sizeof(nct5104dctl_arg_t)))
            {
                return -EACCES;
            }		

			if( ( q_ctl.registry > 255) || (q_ctl.registry) < 0)  return -EINVAL;

			nct5104d_writeb(q_ctl.registry,q_ctl.value );

			printk(KERN_INFO "nct5104d_gpio: [DEBUG] received cmd     => IOCTL_SET_REG ");
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] Query registry   => 0x%02x\n", q_ctl.registry);
			printk(KERN_INFO "nct5104d_gpio: [DEBUG] Return value is  => %d\n", q_ctl.value);

            break;
        case IOCTL_SET_PIN:
			if (copy_from_user(&q_gpio, (gpio_arg_t *)arg, sizeof(gpio_arg_t)))
            {
                return -EACCES;
            }

			printk(KERN_INFO "nct5104d_gpio: [DEBUG] received cmd     => IOCTL_SET_PIN ");

			&nct5104d_gpio_bank[NCT5104D_BANK(q_gpio.pin)]->set_dir(&q_gpio); 
			&nct5104d_gpio_bank[NCT5104D_BANK(q_gpio.pin)]->set_pin(&q_gpio); 


            break;			
        default:
            return -EINVAL;
    }

	nct5104d_efm_disable();
    return err;
}

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
static void nct5104d_device_release(struct device *dev)
{

}

static int nct5104d_drv_probe(struct platform_device *pdev)
{
	static int err;
	u16 devid;
	u8 gpio_en;

	struct platform_data_nct5104d *pdata = dev_get_platdata(&pdev->dev);

	err = nct5104d_efm_enable();
	if (err)
		return err;

	err = -ENODEV;

	devid = nct5104d_readw(NCT5104D_REG_CHIPID);

	switch (devid) 
	{
		case NCT5104D_ID:
			printk(KERN_ALERT "nct5104d_gpio: Found device with chip ID (Nuvo) : 0x%02x\n",devid);
			break;
		case NCT5104D_ID_APU:
			printk(KERN_ALERT "nct5104d_gpio: Found device with chip ID (APU)  : 0x%02x\n",devid);

			
			/*--------  enable GPIO ports  --------*/
			nct5104d_select_logical_device(NCT5104D_LDEVICE_GPIO);
			
			gpio_en = nct5104d_readb(NCT5104D_REG_GPIOEN);
			printk(KERN_ALERT "nct5104d_gpio: GPIO ports config                : 0x%02x\n",gpio_en);

			gpio_en |= ( NCT5104D_GPIO0_EN | NCT5104D_GPIO1_EN );

			nct5104d_writeb(NCT5104D_REG_GPIOEN, gpio_en);

			printk(KERN_ALERT "nct5104d_gpio: Enabled GPIO ports               : 0x%02x\n",gpio_en);
			break;
		default:
			printk(KERN_ALERT "nct5104d_gpio: Unsupported device 0x%04x\n", devid);
	}

	err = 0;

	printk(KERN_ALERT "nct5104d_gpio: platform data - chip addr        : 0x%02x\n",pdata->chip_addr);
	printk(KERN_ALERT "nct5104d_gpio: platform data - num GPIO         : %d\n",pdata->num_gpio);
	printk(KERN_ALERT "nct5104d_gpio: platform data - gpio access addr : 0x%02x\n",gpio_access_addr);
	
	printk(KERN_ALERT "nct5104d_gpio: Succesfuly registered platform device\n");

	nct5104d_efm_disable();
	return err;
	
}

static int nct5104d_drv_remove(struct platform_device *pdev)
{
	return 0;
}

/*--------  initialization  & release --------*/
void __init nct5104d_init_platform_data(void)
{
    /* Register "nct5104d platform device" with the OS. */
	platform_device_register(&device_pdevice_nct5104d);


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

	mutex_init(&nct5104d_mutex);       /// Initialize the mutex lock dynamically at runtime

	res = nct5104d_cdev_register();
	if (res)
		return res; 

	nct5104d_init_platform_data();
	platform_driver_probe(&nct5104d_pldriver, nct5104d_drv_probe);

    return 0;
}

/*--------  exit cleanup  --------*/
static void __exit nct5104d_driver_exit(void)
{
	device_destroy(cl, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(cl);                          // unregister the device class
	class_destroy(cl);                             // remove the device class
	unregister_chrdev(majorNumber, DRIVER_NAME);   // unregister the major number

	platform_driver_unregister(&nct5104d_pldriver);
	platform_device_unregister(&device_pdevice_nct5104d);

	mutex_destroy(&nct5104d_mutex);        			// destroy the dynamically-allocated mutex

	printk(KERN_ALERT "nct5104d_gpio: Unregistered platform device");

    return;
}

module_init(nct5104d_driver_init);
module_exit(nct5104d_driver_exit);



MODULE_AUTHOR("RafPe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Define param gor GPIO access");
MODULE_VERSION("0.3");