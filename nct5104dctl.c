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
    printf("Request registry: %d\n", q.registry);

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
    int option = 0;
    char *file_name = "/dev/nct5104d_gpio";
    int fd;

    if(argc < 4)
    {
        print_usage();
        return 1;
    }

    q.registry = -1;
    q.value = 0;


    while ((option = getopt(argc, argv,"a:r:v:")) != -1) {
        switch (option) {
             case 'a' :         printf("I got [%s] as action param\n",optarg);
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
             case 'r' : q.registry = atoi(optarg); 
                 break;
             case 'v' : q.value = atoi(optarg); 
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }

    if (option == 0) {
        printf("I would get something for you\n");
    }
    else
    {
                printf("I would get something for you\n");
    }


    if (q.registry == -1) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    printf("Registry : %d\n", q.registry);
    printf("Value: %d\n", q.value);


    // fd = open(file_name, O_RDWR);
    // if (fd == -1)
    // {
    //     perror("query_apps open");
    //     return 2;
    // }

    // if(option == e_get)
    // {
    //     get_vars(fd);
    // }
    // else
    // {

    // }

    // close (fd);
 
    return 0;
}










