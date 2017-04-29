#include <linux/ioctl.h>

#define DRIVER_NAME "nct5104d_gpio"

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



/*--------  Direct GPIO Access (DGA)  --------*/
#define NCT5104D_DGA_GSR               0x65                    /* Select GPIO */
#define NCT5104D_DGA_IO                0x66                    /* input or output */
#define NCT5104D_DGA_DATA              0x67                    /* set pin state */
#define NCT5104D_DGA_INVERSION         0x68                   
#define NCT5104D_DGA_STATUS            0x69                     /* active edge detection */


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