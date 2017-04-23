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
    int pin, state, direction;
} gpio_arg_t;
 
#define IOCTL_GET_PIN _IOR('q', 1, gpio_arg_t *)
#define IOCTL_SET_PIN _IOW('q', 2, gpio_arg_t *)

typedef struct
{
    int registry, value;
} nct5104dctl_arg_t;

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
    printf("Usage: rectangle [ap] -l num -b num\n");
}


void get_vars(int fd)
{
        printf("Request - cmd : %zu\n", IOCTL_GET_REG);
        printf("Request registry: %d\n", q.registry);

    if (ioctl(fd, IOCTL_GET_REG, &q) == -1)
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

    while ((ch = getopt_long(argc, argv, "t:a:", long_options, NULL)) != -1)
    {
        // check to see if a single character or long option came through
        switch (ch)
        {
            case 'a':
                if (strcmp(optarg, "get") == 0)
                {
                    option = e_get;
                }
                else
                {
                    option = e_set;
                }
                break;
            case 'r':
                q.registry = atoi(optarg); 
                break;
            case 'v':
                q.value = atoi(optarg);
                break;
             default: print_usage();  
                      return 0;                             
        }
    }


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