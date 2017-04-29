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
#include "nct5104dpin.h"

#define NCT5104D_FILE_DEVICE "/dev/nct5104d_gpio" 

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

globalargs_t globalargs;

void set_pin(int fd, globalargs_t * globargs);
void get_pin(int fd, globalargs_t * globargs);

int convert_from_hex_string(char * strhex);
void print_debug(globalargs_t * globargs);

void print_usage();
void getoptions(int argc, char ** argv, globalargs_t * globargs);
int open_file_dev(char * file_dev_name);

 
int main(int argc, char *argv[])
{
    int fd;
    char *file_name = NCT5104D_FILE_DEVICE;


    
    /*--------  default init  --------*/    
    globalargs.action   = e_pin_get;            // Default behaviour
    globalargs.dir      = e_pin_out;            // Default behaviour
    globalargs.pin      = -1;
    globalargs.value    = 0;
    
    getoptions( argc,  argv, &globalargs);
    
    //print_debug(&globalargs);

    fd = open_file_dev(file_name);


    if(globalargs.action == e_pin_set   \
    && (globalargs.pin >= 0)    \
    && (globalargs.pin < 16)   \
    && (globalargs.value == 0 || globalargs.value == 1) )
    {
        set_pin(fd, &globalargs);
    }

    close (fd);

    return 0;
}


void print_usage() 
{
 printf("\n Usage : nct5104dctl [ --get|--set ] [--reg|--pin ] --id <registry>|<pin> --val <value> --dir <direction_for_gpio> \n"); 
 printf("\n --id                : Takes hex value for registry and decimal value for pin \n"); 
 printf("\n --val               : Takes vlaue to be written for pin/registry\n");
 printf("\n --dir               : Direction of GPIO ( 1 => OUT ; 0 => IN) \n"); 
 printf("\n Examples:\n");
 printf("\n * Get a registry value: nct5104dctl --get --reg --id 0x07 \n");
 

}

void set_pin(int fd, globalargs_t * globargs)
{
    gpio_arg_t s;

    s.pin       = globargs->pin;
    s.direction = globargs->dir;
    s.state     = globargs->value;

    if (ioctl(fd, IOCTL_SET_PIN, &s) == -1)
    {
        perror("nct5104dctl ioctl set pin");
    }
    else
    {
        printf("[{ \"pin\":%d,\"value\":%d}]\n", globargs->pin,globargs->value);
    }
}

void getoptions(int argc, char ** argv, globalargs_t * globargs)
{
    int option_index;
    int getopt_ret;
    static struct option long_options[] = {
        { "set",         no_argument,        NULL,    0},
        { "get",         no_argument,        NULL,    0},
        { "pin",         required_argument,  NULL,    'p'},
        { "val",         required_argument,  NULL,    'v'},
        { "dir",         required_argument,  NULL,    'd'},       
        { NULL,          no_argument,        NULL,    0 },
    };

    while( -1 != ( getopt_ret = getopt_long(  argc
                                            , argv
                                            , "sgp:v:d:"
                                            , long_options
                                            , &option_index) ) ) {
        switch( getopt_ret ) {
        case 0: 
            if( strcmp( "get", long_options[option_index].name ) == 0 ) {
                globargs->action = e_pin_get;
            }
            if( strcmp( "set", long_options[option_index].name ) == 0 ) {
                globargs->action = e_pin_set;
            } 
            break;
        case 1:
            // handle non-option arguments here if you put a `-`
            // at the beginning of getopt_long's 3rd argument
            break;
        case 'v':
            globargs->value = atoi(optarg);
            break;
        case 'p':
            globargs->pin = atoi(optarg);
            break;
        case '?':
            print_usage();
            break;
        default:
            printf( "Unknown: getopt_ret == %d\n", getopt_ret );
            break;
        }
    }

}

int open_file_dev(char * file_dev_name)
{
    int fdevice;
    fdevice = open(file_dev_name, O_RDWR);
    if (fdevice == -1)
    {
        perror("nct5104dctl open");
        return 2;
    }

    return fdevice;
}

int convert_from_hex_string(char * strhex)
{
    long val;
    char *endptr;
  
    /*--------  convert from hex string  --------*/    
    val = strtol(strhex, &endptr, 16);

    if (endptr == strhex) 
    {
        return atoi(strhex); 
    }
    else
    {
        return (int) val; 
    }
}

void print_debug(globalargs_t * globargs)
{

    printf("Type          : pin\n");
    printf("Pin            : %d\n", globargs->pin); 

    
    /*--------  action get  --------*/    
    if(globargs->action == e_pin_get) {
        printf("Action        : get\n");
    } 
    /*--------  action set  --------*/    
    if(globargs->action == e_pin_set) {
        printf("Action        : set\n");
    } 

    printf("Value         : %d\n",globargs->value);
}
