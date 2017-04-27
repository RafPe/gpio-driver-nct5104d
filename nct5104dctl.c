#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
 

/*--------  Include for types and ioctl  --------*/
#include "nct5104d_gpio.h"

enum gpio_status_e {
    GPIO_HIGH,
    GPIO_LOW
};

enum gpio_dir_e {
    GPIO_IN,
    GPIO_OUT
};

/**
 *
 * This structure defines our behaviour from options we receive.
 *
 * @type: 0-> registry ; 1->gpio 
 * @action: 0 -> get (Default ) ; 1 -> set; 
 * @id: which registry/pin to target with our action
 * @value: what value to write to our target
 * @gpio_dir: 0 -> in ; 1 -> out;
 */
typedef struct
{
    unsigned type;
    unsigned action; 
    unsigned id;
    unsigned value;
    gpio_dir_e gpio_dir;

} globalargs_t;

nct5104dctl_arg_t q;

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
          { "set-pin", required_argument, NULL, 0},
          { "get-pin", required_argument, NULL, 0},        
          { "set-reg", required_argument, NULL, 0},
          { "get-reg", required_argument, NULL, 0},
          {0, 0, 0, 0}
    };

    while( -1 != ( getopt_ret = getopt_long(  argc
                                            , argv
                                            , "cgr:a::"
                                            , long_options
                                            , &option_index) ) ) {
        const char *tmp_optarg = optarg;
        switch( getopt_ret ) {
        case 0: if( strcmp( "config", long_options[option_index].name ) == 0 ) {
                    printf( "You config\n" );
                }
                if( strcmp( "gpio", long_options[option_index].name ) == 0 ) {
                    printf( "You gpio\n" );
                } 
                break;
        case 1:
            // handle non-option arguments here if you put a `-`
            // at the beginning of getopt_long's 3rd argument
            break;
        case 'r':
            printf("Kudos to %s\n", optarg); break;
        case 'a':
            if(   !optarg
            && NULL != argv[option_index]
            && '-' != argv[option_index][0] ) 
            {
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
            if (tmp_optarg) 
            {
                printf (", %s!\n", tmp_optarg);
            } 
            else 
            {
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


// //http://stackoverflow.com/questions/11867636/c-getopt-wrong-argument-corresponding-to-its-option
// void getoptions (int argc, char **argv, globalargs_t* globalargs) 
// {

//     static const char *optstring = "vDqnd:c:f:o:h?:";

//     static const struct option longopts[] = {
//         { "config",      no_argument,        NULL,    0},
//         { "gpio",        no_argument,        NULL,    0},
//         { "verbose",    no_argument,        NULL,    'v'},
//         { "quiet",        no_argument,        NULL,    'q'},
//         { "noheader",    no_argument,        NULL,    0},
//         { "delimiter",    required_argument,     NULL,    0},
//         { "columns",    required_argument,    NULL,    0},
//         { "filter",        required_argument,    NULL,    0},
//         { "order",        required_argument,    NULL,    0},
//         { "getid",        no_argument,        NULL,    'i'},
//         { NULL,            no_argument,        NULL,    0 }
//     };

//     int opt = 0;
//     int longindex = 0;
//     //opterr = 0;

//     /* Process the arguments with getopt_long(), then populate globalargs-> */
//     opt = getopt_long( argc, argv, optstring, longopts, &longindex );
//     while( opt != -1 ) {
//         switch( opt ) {
//             case '?':
//                 break;
//             case 'D':
//                 globalargs->debug = 1;    /* true */
//                 break;
//             case 'v':
//                 globalargs->verbose++;
//                 break;
//             case 'q':
//                 globalargs->quiet = 1;
//                 break;
//             case 'i':
//                 globalargs->id = 1;
//                 break;
//             case 'h':
//                 globalargs->help = 1;
//                 break;
//             case 0:        /* long option without a short arg */
//                 if( strcmp( "Debug", longopts[longindex].name ) == 0 ) {
//                     globalargs->debug = 1;
//                 }
//                 if( strcmp( "verbose", longopts[longindex].name ) == 0 ) {
//                     globalargs->verbose = 1;
//                 }
//                 if( strcmp( "quiet", longopts[longindex].name ) == 0 ) {
//                     globalargs->quiet = 1;
//                 }
//                 if( strcmp( "noheader", longopts[longindex].name ) == 0 ) {
//                     globalargs->noheader = 1;
//                 }
//                 if( strcmp( "delimiter", longopts[longindex].name ) == 0 ) {
//                     globalargs->delimiter = *optarg;
//                 }
//                 if( strcmp( "filter", longopts[longindex].name ) == 0 ) {
//                     globalargs->filter = optarg;
//                 }
//                 if( strcmp( "order", longopts[longindex].name ) == 0 ) {
//                     globalargs->order = optarg;
//                 }
//                 if( strcmp( "columns", longopts[longindex].name ) == 0 ) {
//                     globalargs->columns = optarg;
//                 }
//                 break;
//             default:
//                 /* You won't actually get here. */
//                 break;
//         }
//         opt = getopt_long( argc, argv, optstring, longopts, &longindex );
//     }
//     if (optind < argc) {
//         while (optind < argc) {
//             globalargs->actions[globalargs->actionsindex] = argv[optind++];
//             globalargs->actionsindex++;
//         }
//     }
// }