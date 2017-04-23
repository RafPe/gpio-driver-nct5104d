#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H

#include <linux/ioctl.h>

typedef struct
{
    int pin, state, direction;
} gpio_arg_t;
 
#define IOCTL_GET_PIN _IOR('q', 1, gpio_arg_t *)
#define IOCTL_SET_VARIABLES _IOW('q', 2, gpio_arg_t *)

typedef struct
{
    int registry, value;
} nct5104dctl_arg_t;

#define IOCTL_GET_REG _IOR('q', 3, nct5104dctl_arg_t *)
#define IOCTL_SET_REG _IOW('q', 4, nct5104dctl_arg_t *)

#endif