/*

Copyright <2017> <RafPe>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */
 
#include <linux/ioctl.h>

#define DRIVER_NAME "nct5104d_gpio"


/*--------  uncomment below to enable debugging  --------*/

//#define DRIVER_DEBUG

#define NCT5104D_DEVICE_ADDR            0x2E

#define NCT5104D_REG_SOFT_RESET     	0x02
#define NCT5104D_REG_LDEVICE     		0x07	
    #define NCT5104D_LDEVICE_GPIO    		0x07
    #define NCT5104D_LDEVICE_8      		0x08
#define NCT5104D_REG_CHIPID		        0x20	/* 2 bytes */
#define NCT5104D_REG_GPIOEN     		0x30
    #define NCT5104D_GPIO0_EN               0x01
    #define NCT5104D_GPIO1_EN               0x02
#define NCT5104D_REG_GPIO_BASEADDR_H    0x60
#define NCT5104D_REG_GPIO_BASEADDR_L    0x61


/*--------  GPIO OFFSETS  --------*/
#define NCT5104D_GPIO_OFFSET_IO                0                    /* input or output */
#define NCT5104D_GPIO_OFFSET_DATA              1                    /* set pin state */
#define NCT5104D_GPIO_OFFSET_INVERSION         2                   
#define NCT5104D_GPIO_OFFSET_STATUS            3                     /* active edge detection */

/*--------  GPIO0  --------*/
#define NCT5104D_REG_GPIO0_IO                0xE0                    /* input or output */
#define NCT5104D_REG_GPIO0_DATA              0xE1                    /* set pin state */
#define NCT5104D_REG_GPIO0_INVERSION         0xE2                   
#define NCT5104D_REG_GPIO0_STATUS            0xE3                     /* active edge detection */

/*--------  GPIO1  --------*/
#define NCT5104D_REG_GPIO1_IO                0xE4                    /* input or output */
#define NCT5104D_REG_GPIO1_DATA              0xE5                    /* set pin state */
#define NCT5104D_REG_GPIO1_INVERSION         0xE6                   
#define NCT5104D_REG_GPIO1_STATUS            0xE7                     /* active edge detection */

/*--------  chip IDs  --------*/
#define NCT5104D_ID					    0x1061	
#define NCT5104D_ID_APU         	    0xc452

/*--------  extended function mode  --------*/
#define NCT5104D_EFM_ENABLE	    	    0x87	
#define NCT5104D_EFM_DISABLE		    0xAA	

#define FIRST_MINOR 0
#define MINOR_CNT   1

#define NCT5104D_BANK(A) ({unsigned retval=0; if (A > 7 ) retval =1; retval;})
#define NCT5104D_PIN(A)  ({unsigned retval=A; if (A > 7 ) retval = A-8; retval;})

typedef struct {
    unsigned pin;
    unsigned direction;
    unsigned state;
} gpio_arg_t;
 
typedef struct
{
    int registry;
    int value;
} nct5104dctl_arg_t;
 
#define IOCTL_GET_PIN _IOR('q', 1, gpio_arg_t *)
#define IOCTL_SET_PIN _IOW('q', 2, gpio_arg_t *)


#define IOCTL_GET_REG _IOR('q', 3, nct5104dctl_arg_t *)
#define IOCTL_SET_REG _IOW('q', 4, nct5104dctl_arg_t *)

#define IOCTL_SET_DIR _IOW('q', 5, gpio_arg_t *)

typedef enum {
    GPIO_HIGH,
    GPIO_LOW
} gpio_status_e;

typedef enum {
    GPIO_IN,
    GPIO_OUT
} gpio_dir_e ;


struct platform_data_nct5104d {
 int chip_addr;
 int num_gpio;
};

typedef struct {
    unsigned int id;
	unsigned int regbase;
    unsigned int num_gpio;
} nct5104d_gpio_bank_t;

static unsigned int         nct5104d_readw(unsigned int  reg);
static inline unsigned int  nct5104d_readb(unsigned int  reg);
static inline void          nct5104d_writeb(unsigned int  reg, unsigned val);

static inline int       nct5104d_efm_enable(void);
static inline void      nct5104d_efm_disable(void);

static inline void      nct5104d_select_logical_device(unsigned int  ld);
static inline int       nct5104d_get_logical_device(void);

static inline void      nct5104d_soft_reset(void);

static unsigned int nct5104d_gpio_pin_get(gpio_arg_t * gpioctl, nct5104d_gpio_bank_t * gpiobank);
static void         nct5104d_gpio_pin_set(gpio_arg_t * gpioctl, nct5104d_gpio_bank_t * gpiobank);
static void         nct5104d_gpio_dir_set(gpio_arg_t * gpioctl, nct5104d_gpio_bank_t * gpiobank);

static int nct5104d_cdev_register(void);
static int nct5104d_cdev_open(struct inode *i, struct file *f);
static int nct5104d_cdev_close(struct inode *i, struct file *f);

static long nct5104d_ioctl(struct file *f, unsigned int cmd, unsigned long arg);
static void nct5104d_device_release(struct device *dev);

static int nct5104d_drv_probe(struct platform_device *pdev);
static int nct5104d_drv_remove(struct platform_device *pdev);

void __init nct5104d_init_platform_data(void);
static void __exit nct5104d_driver_exit(void);