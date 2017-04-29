typedef enum {
    e_reg_get,
    e_reg_set,
} e_reg_action_t;

typedef struct {
    e_reg_action_t      action; 
    int                 reg;
    unsigned            value;
} globalargs_t;

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