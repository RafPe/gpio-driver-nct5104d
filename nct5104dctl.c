#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
 
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




enum
{
    e_get,
    e_set,
} option; 

nct5104dctl_arg_t q;
int ch;

static struct option long_options[] =
{
    {"action", required_argument, NULL, 'a'},
    {"registry", required_argument, NULL, 'r'},
    {"value", optional_argument, NULL, 'v'},
};

void print_usage() 
{
 printf("\n Usage : nct5104dctl <operation> -reg <registry> -val <value>\n"); 
 printf("\n <operation> can be 'get', 'set'\n"); 
 printf("\n For example: nct5104dctl get 0x07\n");

}


void get_vars(int fd)
{
    if (ioctl(fd, IOCTL_GET_REG, &q) == -1)
    {
        perror("nct5104dctl ioctl get");
    }
    else
    {
        printf("Registry : %d\n", q.registry);
        printf("Value: %d\n", q.value);
    }
}
 
int main(int argc, char *argv[])
{
    int fd ,option = 0 ,indexptr = 0;
    char *endptr, *str, *file_name = "/dev/nct5104d_gpio";
    long val;

    if(argc < 4)
    {
        print_usage();
        return 1;
    }

    q.registry = -1;
    q.value = 0;


    //while ((option = getopt_long(argc, argv, "a:r:v:",long_options, &indexptr)) != -1){
    while ((option = getopt(argc, argv,"a:r:v:")) != -1) {
        switch (option) {
             case 'a' :         
                        if (strcmp(optarg, "get") == 0)
                            {
                                option = 0;
                                printf("setup option to be get\n");
                            }
                            else
                            {
                                option = 1;
                                printf("setup option to be set\n");
                            }
                            break;
             case 'r' : val = strtol(optarg, &endptr, 16); // convert from hex string  i.e. 0x01
                        if (endptr == optarg) {
                            q.registry = atoi(optarg); 
                        }
                        else
                        {
                            q.registry = (int) val; // TODO Check for conversion error!
                        }
                         
                 break;
             case 'v' : val = strtol(optarg, &endptr, 16); // convert from hex string  i.e. 0x01
                        if (endptr == optarg) {
                            q.value = atoi(optarg); 
                        }
                        else
                        {
                            q.value = (int) val; // TODO Check for conversion error!
                        }
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }


    if (q.registry < 1) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("nct5104dctl open");
        return 2;
    }

    if (option == 0) {
         get_vars(fd);
    }
    else
    {
        get_vars(fd);
    }

    close (fd);
 
    return 0;
}