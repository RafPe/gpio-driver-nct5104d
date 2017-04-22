#define DRIVER_NAME "nct5104d_gpio"

#define NCT5104D_DEVICE_ADDR            0x2E
#define NCT5104D_LDEV_SELECT     		0x07	
#define NCT5104D_SUPERIO_ENABLE	    	0x87	
#define NCT5104D_SUPERIO_DISABLE		0xAA	

#define NCT5104D_REG_BASE               0x67
#define NCT5104D_REG_GPIO_IO            ( NCT5104D_REG_BASE_ADDR +1 )
#define NCT5104D_REG_GPIO_DATA          ( NCT5104D_REG_BASE_ADDR +2 )
#define NCT5104D_REG_GPIO_INVERSION     ( NCT5104D_REG_BASE_ADDR +3 )
#define NCT5104D_REG_GPIO_STATUS        ( NCT5104D_REG_BASE_ADDR +4 )

struct platform_data_ntc5104d {
 int chip_addr;
 int num_gpio;
 int gpio_access_addr;
};
