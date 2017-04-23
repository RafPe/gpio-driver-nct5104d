#define DRIVER_NAME "nct5104d_gpio"

#define NCT5104D_DEVICE_ADDR            0x2E

#define NCT5104D_REG_SOFT_RESET     	0x02
#define NCT5104D_REG_LDEVICE     		0x07	
#define NCT5104D_REG_GPIO_BASEADDR_H    0x60
#define NCT5104D_REG_GPIO_BASEADDR_L    0x61

#define NCT5104D_LDEVICE_GPIO    		0x07
#define NCT5104D_LDEVICE_8      		0x08


/*--------  extended function mode  --------*/
#define NCT5104D_EFM_ENABLE	    	    0x87	
#define NCT5104D_EFM_DISABLE		    0xAA	



/*--------  Direct GPIO Access (DGA)  --------*/
#define NCT5104D_DGA_GSR               0x65                    /* Select GPIO */
#define NCT5104D_DGA_IO                0x66                    /* input or output */
#define NCT5104D_DGA_DATA              0x67                    /* set pin state */
#define NCT5104D_DGA_INVERSION         0x68                   
#define NCT5104D_DGA_STATUS            0x69                     /* active edge detection */


enum nct5104d_gpio_status {
    nct5104d_GPIO_HIGH,
    nct5104d_GPIO_LOW
};

enum nct5104d_gpio_dir {
    nct5104d_GPIO_IN,
    nct5104d_GPIO_OUT
};

struct platform_data_ntc5104d {
 int chip_addr;
 int num_gpio;
 void (*set_pin)(struct platform_data_ntc5104d* pdata,u8 pin,u8 state);
 void (*get_pin)(struct platform_data_ntc5104d* pdata,u8 pin);
};
