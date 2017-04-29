typedef enum {
    e_pin_get,
    e_pin_set,
    e_dir_set,
} e_pin_action_t;

typedef enum {
    e_pin_out,
    e_pin_in,
} e_pin_dir_t;

typedef struct {
    e_pin_action_t      action; 
    int                 pin;
    unsigned            value;
    e_pin_dir_t         dir;

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

#define IOCTL_SET_DIR _IOW('q', 5, gpio_arg_t *)