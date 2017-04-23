#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <stdlib.h>
 
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

#define IOCTL_CMD_GET_PIN 1
#define IOCTL_CMD_SET_PIN 2

#define IOCTL_CMD_GET_REG 3
#define IOCTL_CMD_SET_REG 4


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
    printf("Usage: rectangle [ap] -l num -b num\n");
}


void get_vars(int fd)
{
        printf("Request - cmd : %zu\n", IOCTL_CMD_GET_REG);
        printf("Request registry: %d\n", q.registry);

    if (ioctl(fd, IOCTL_CMD_GET_REG, &q) == -1)
    {
        perror("query_apps ioctl get");
    }
    else
    {
        printf("Registry : %d\n", q.registry);
        printf("Value: %d\n", q.value);
    }
}
 
int main(int argc, char *argv[])
{
    char *file_name = "/dev/nct5104d_gpio";
    int fd;

    q.value=0;

    q.registry = 0x07

    // while ((ch = getopt_long(argc, argv, "t:a:", long_options, NULL)) != -1)
    // {
    //     // check to see if a single character or long option came through
    //     switch (ch)
    //     {
    //         case 'a':
    //             if (strcmp(optarg, "get") == 0)
    //             {
    //                 option = e_get;
    //             }
    //             else
    //             {
    //                 option = e_set;
    //             }
    //             break;
    //         case 'r':
    //             q.registry = atoi(optarg); 
    //             break;
    //         case 'v':
    //             q.value = atoi(optarg);
    //             break;
    //          default: print_usage();  
    //                   return 0;                             
    //     }
    // }


    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("query_apps open");
        return 2;
    }

    if(option == e_get)
    {
        get_vars(fd);
    }
    else
    {

    }

    close (fd);
 
    return 0;
}