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

#define NCT5104D_FILE_DEVICE "/dev/nct5104d_gpio" 

/**
 *
 * This structure defines our behaviour from options we receive.
 *
 * @type: 0-> registry ; 1->pin 
 * @action: 0 -> get (Default ) ; 1 -> set; 
 * @id: which registry/pin to target with our action
 * @value: what value to write to our target
 * @gpio_dir: 0 -> in ; 1 -> out;
 */
typedef struct
{
    int type;
    int action; 
    int id;
    unsigned value;
    unsigned gpio_dir;

} globalargs_t;

globalargs_t globalargs;

void get_registry(int fd, globalargs_t * globargs);
void set_registry(int fd, globalargs_t * globargs);
void set_pin(int fd, globalargs_t * globargs);

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
    globalargs.type = -1;
    globalargs.action = -1;
    globalargs.id = -1;
    globalargs.value =0;
    globalargs.gpio_dir = 0;

    
    getoptions( argc,  argv, &globalargs);
    
    //print_debug(&globalargs);

    fd = open_file_dev(file_name);

    /*--------  selected get / registry with valid id  --------*/    
    if(globalargs.action == 0 && globalargs.type == 0 && globalargs.id >= 0 && globalargs.id <= 255)
    {
        get_registry(fd, &globalargs);
    }

    /*--------  selected set / registry with valid id  --------*/  
    if(globalargs.action == 1 && globalargs.type == 0 && globalargs.id >= 0 && globalargs.id <= 255)
    {
        set_registry(fd, &globalargs);
    }

    /*--------  selected set / pin with valid id  --------*/  
    if(globalargs.action == 1 && globalargs.type == 1 && globalargs.id >= 0 && globalargs.id <= 255)
    {
        set_pin(fd, &globalargs);
    }

    close (fd);

    // if (q.registry < 1) {
    //     printf("sregistry not detected: %d \n",q.registry );     
    //     print_usage();
    //     exit(EXIT_FAILURE);
    // }

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

void get_registry(int fd, globalargs_t * globargs)
{
    nct5104dctl_arg_t q;

    q.registry = globargs->id;
    q.value    = 0;

    if (ioctl(fd, IOCTL_GET_REG, &q) == -1)
    {
        perror("nct5104dctl ioctl get");
    }
    else
    {
        printf("[{ \"registry\":\"0x%02x\",\"value\":%d}]\n", q.registry,q.value);
    }
}

void set_registry(int fd, globalargs_t * globargs)
{
    nct5104dctl_arg_t s;

    s.registry = globargs->id;
    s.value    = globargs->value;

    if (ioctl(fd, IOCTL_SET_REG, &s) == -1)
    {
        perror("nct5104dctl ioctl set");
    }
    else
    {
        printf("[{ \"registry\":\"0x%02x\",\"value\":%d}]\n", s.registry,s.value);
    }
}

void set_pin(int fd, globalargs_t * globargs)
{
    gpio_arg_t s;

    s.pin = 1;
    s.direction =1;
    s.state = 1;

    if (ioctl(fd, IOCTL_SET_PIN, &s) == -1)
    {
        perror("nct5104dctl ioctl set");
    }
    else
    {
        printf("[{ \"registry\":\"0x%02x\",\"value\":%d}]\n", 6,6);
    }
}

void getoptions(int argc, char ** argv, globalargs_t * globargs)
{
    int option_index;
    int getopt_ret;
    static struct option long_options[] = {
        { "set",         no_argument,        NULL,    0},
        { "get",         no_argument,        NULL,    0},
        { "pin",         no_argument,        NULL,    0},
        { "reg",         no_argument,        NULL,    0},
        { "id",          required_argument,  NULL,    'i'},
        { "val",         required_argument,  NULL,    'v'},
        { "dir",         required_argument,  NULL,    'd'},       
        { NULL,          no_argument,        NULL,    0 },
    };

    while( -1 != ( getopt_ret = getopt_long(  argc
                                            , argv
                                            , "sgpri:v:d:"
                                            , long_options
                                            , &option_index) ) ) {
        switch( getopt_ret ) {
        case 0: 
            if( strcmp( "get", long_options[option_index].name ) == 0 ) {
                globargs->action = 0;
            }
            if( strcmp( "set", long_options[option_index].name ) == 0 ) {
                globargs->action = 1;
            } 
            if( strcmp( "pin", long_options[option_index].name ) == 0 ) {
                globargs->type = 1;
            }
            if( strcmp( "reg", long_options[option_index].name ) == 0 ) {
                globargs->type = 0;
            } 
            break;
        case 1:
            // handle non-option arguments here if you put a `-`
            // at the beginning of getopt_long's 3rd argument
            break;
        case 'v':
            globargs->value = atoi(optarg);
            break;
        case 'i':
            /*--------  registry type selected  --------*/      
            if(globargs->type == 0) {
                globargs->id = convert_from_hex_string(optarg);
            } 
            /*--------  pin type selected  --------*/ 
            if(globargs->type == 1) {
                globargs->id = atoi(optarg);
            } 
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
    /*--------  registry type selected  --------*/      
    if(globargs->type == 0) {
        printf("Type          : reg\n");
        printf("ID            : 0x%02x\n", globargs->id); 
    } 
    /*--------  pin type selected  --------*/ 
    if(globargs->type == 1) {
        printf("Type          : pin\n");
        printf("ID            : %d\n", globargs->id); 
    } 

    
    /*--------  action get  --------*/    
    if(globargs->action == 0) {
        printf("Action        : get\n");
    } 
    /*--------  action set  --------*/    
    if(globargs->action == 1) {
        printf("Action        : set\n");
    } 

        printf("value         : %d\n",globargs->value);
}