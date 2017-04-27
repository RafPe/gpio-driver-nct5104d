#include <linux/ioctl.h>

#define DRIVER_NAME "nct5104d_gpio"

#define NCT5104D_DEVICE_ADDR            0x2E

#define NCT5104D_REG_SOFT_RESET     	0x02
#define NCT5104D_REG_LDEVICE     		0x07	
#define NCT5104D_REG_CHIPID		        0x20	/* 2 bytes */
#define NCT5104D_REG_GPIOEN     		0x30
#define NCT5104D_REG_GPIO_BASEADDR_H    0x60
#define NCT5104D_REG_GPIO_BASEADDR_L    0x61

#define NCT5104D_LDEVICE_GPIO    		0x07
#define NCT5104D_LDEVICE_8      		0x08



#define NCT5104D_GPIO0_EN               0x01
#define NCT5104D_GPIO1_EN               0x02

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



/*--------  ioctl  --------*/
 typedef struct
{
    int pin;
    int state; 
    int direction;
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
 void (*set_pin)(struct platform_data_nct5104d* pdata,unsigned pin,unsigned state);
 void (*get_pin)(struct platform_data_nct5104d* pdata,unsigned pin);
};
