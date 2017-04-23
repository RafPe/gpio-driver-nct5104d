#define DRIVER_NAME "nct5104d_gpio"

#define NCT5104D_DEVICE_ADDR            0x2E

#define NCT5104D_REG_LDEVICE     		0x07	
#define NCT5104D_REG_GPIO_BASEADDR_H    0x60
#define NCT5104D_REG_GPIO_BASEADDR_L    0x61

#define NCT5104D_LDEVICE_GPIO    		0x07


/*--------  extended function mode  --------*/
#define NCT5104D_EFM_ENABLE	    	    0x87	
#define NCT5104D_EFM_DISABLE		    0xAA	



/*--------  Direct GPIO Access (DGA)  --------*/
#define NCT5104D_DGA_GSR               0x65                    /* Select GPIO */
#define NCT5104D_DGA_IO                ( NCT5104D_DGA_GSR +1 ) /* input or output */
#define NCT5104D_DGA_DATA              ( NCT5104D_DGA_GSR +2 ) /* set pin state */
#define NCT5104D_DGA_INVERSION         ( NCT5104D_DGA_GSR +3 )
#define NCT5104D_DGA_STATUS            ( NCT5104D_DGA_GSR +4 ) /* active edge detection */

enum
{
LOW,
HIGH,
} e_pin_state;

struct platform_data_ntc5104d {
 int chip_addr;
 int num_gpio;
 void (*set_pin)(struct platform_data_ntc5104d* pdata,u8 pin,e_pin_state state);
 void (*get_pin)(struct platform_data_ntc5104d* pdata,u8 pin);
};
