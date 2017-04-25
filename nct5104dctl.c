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

nct5104dctl_arg_t q;

#define IOCTL_GET_PIN _IOR('q', 1, gpio_arg_t *)
#define IOCTL_SET_PIN _IOW('q', 2, gpio_arg_t *)


#define IOCTL_GET_REG _IOR('q', 3, nct5104dctl_arg_t *)
#define IOCTL_SET_REG _IOW('q', 4, nct5104dctl_arg_t *)

void get_vars(int fd);
void print_usage();

 
int main(int argc, char *argv[])
{
    int ch;
    int fd ,option = 0 ,indexptr = 0;
    char c, *endptr, *str, *file_name = "/dev/nct5104d_gpio";
    long val;
    int getopt_ret;
    int option_index;
    static struct option long_options[] = {
          {"registry",  required_argument, 0, 'r'},
          {"action",  optional_argument, 0, 'a'},
          {0, 0, 0, 0}
    };

    while( -1 != ( getopt_ret = getopt_long(  argc
                                            , argv
                                            , "r:a::"
                                            , long_options
                                            , &option_index) ) ) {
        const char *tmp_optarg = optarg;
        switch( getopt_ret ) {
        case 0: break;
        case 1:
            // handle non-option arguments here if you put a `-`
            // at the beginning of getopt_long's 3rd argument
            break;
        case 'r':
            printf("Kudos to %s\n", optarg); break;
        case 'a':
            if(   !optarg
            && NULL != argv[option_index]
            && '-' != argv[option_index][0] ) {
            // This is what makes it work; if `optarg` isn't set
            // and argv[optindex] doesn't look like another option,
            // then assume it's our parameter and overtly modify optindex
            // to compensate.
            //
            // I'm not terribly fond of how this is done in the getopt
            // API, but if you look at the man page it documents the
            // existence of `optarg`, `optindex`, etc, and they're
            // not marked const -- implying they expect and intend you
            // to modify them if needed.
            tmp_optarg = argv[option_index++];
            }
            printf( "You suck" );
            if (tmp_optarg) {
            printf (", %s!\n", tmp_optarg);
            } else {
            printf ("!\n");
            }
            break;
        case '?':
            printf("Unknown option\n");
            break;
        default:
            printf( "Unknown: getopt_ret == %d\n", getopt_ret );
            break;
        }
    }

    // q.registry = -1;
    // q.value = 0;


    // while ((c = getopt_long(argc, argv, "gr:v:",long_options, &indexptr)) != -1){
    // //while ((c = getopt(argc, argv,"a:r:v:")) != -1) {
    //     switch (c) {
    //          case 'g' :   
    //                                                     option = 0;
    //                             printf("setup option to be get\n");      
    //                     // if (strcmp(optarg, "get") == 0)
    //                     //     {
    //                     //         option = 0;
    //                     //         printf("setup option to be get\n");
    //                     //     }
    //                     //     else
    //                     //     {
    //                     //         option = 1;
    //                     //         printf("setup option to be set\n");
    //                     //     }
    //                         break;
    //          case 'r' : val = strtol(optarg, &endptr, 16); // convert from hex string  i.e. 0x01
    //                     if (endptr == optarg) {
    //                         q.registry = atoi(optarg); 
    //                     }
    //                     else
    //                     {
    //                         q.registry = (int) val; // TODO Check for conversion error!
    //                     }
                         
    //              break;
    //          case 'v' : val = strtol(optarg, &endptr, 16); // convert from hex string  i.e. 0x01
    //                     if (endptr == optarg) {
    //                         q.value = atoi(optarg); 
    //                     }
    //                     else
    //                     {
    //                         q.value = (int) val; // TODO Check for conversion error!
    //                     }
    //              break;
    //          default: print_usage(); 
    //              exit(EXIT_FAILURE);
    //     }
    // }


    // if (q.registry < 1) {
    //     printf("sregistry not detected: %d \n",q.registry );     
    //     print_usage();
    //     exit(EXIT_FAILURE);
    // }

    // fd = open(file_name, O_RDWR);
    // if (fd == -1)
    // {
    //     perror("nct5104dctl open");
    //     return 2;
    // }

    // if (option == 0) 
    // {
    //      get_vars(fd);
    // }
    // else if (option == 1) 
    // {
    //     get_vars(fd);
    // }

    // close (fd);
 
    return 0;
}


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