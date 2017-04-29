/*

Copyright <2017> <RafPe>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */
 
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
#include "nct5104dreg.h"

#define NCT5104D_FILE_DEVICE "/dev/nct5104d_gpio" 

globalargs_t globalargs;

void get_registry(int fd, globalargs_t * globargs);
void set_registry(int fd, globalargs_t * globargs);

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
    globalargs.action   = e_reg_get;
    globalargs.reg      = -1;
    globalargs.value    = 0;
    
    getoptions( argc,  argv, &globalargs);
    
    //print_debug(&globalargs);

    fd = open_file_dev(file_name);

    /*--------  selected get / registry with valid id  --------*/    
    if(globalargs.action == e_reg_get && globalargs.reg >= 0 && globalargs.reg <= 255)
    {
        get_registry(fd, &globalargs);
    }

    /*--------  selected set / registry with valid id  --------*/  
    if(globalargs.action == e_reg_set && globalargs.reg >= 0 && globalargs.reg <= 255)
    {
        set_registry(fd, &globalargs);
    }


    close (fd);

    return 0;
}


void print_usage() {
 printf("\n Usage : nct5104dctl [ --get|--set ] [--reg|--pin ] --id <registry>|<pin> --val <value> --dir <direction_for_gpio> \n"); 
 printf("\n --id                : Takes hex value for registry and decimal value for pin \n"); 
 printf("\n --val               : Takes vlaue to be written for pin/registry\n");
 printf("\n --dir               : Direction of GPIO ( 1 => OUT ; 0 => IN) \n"); 
 printf("\n Examples:\n");
 printf("\n * Get a registry value: nct5104dctl --get --reg --id 0x07 \n");
 

}

void get_registry(int fd, globalargs_t * globargs){
    nct5104dctl_arg_t q;

    q.registry = globargs->reg;
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

void set_registry(int fd, globalargs_t * globargs){
    nct5104dctl_arg_t s;

    s.registry = globargs->reg;
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



void getoptions(int argc, char ** argv, globalargs_t * globargs)
{
    int option_index;
    int getopt_ret;
    static struct option long_options[] = {
        { "set",         no_argument,        NULL,    0},
        { "get",         no_argument,        NULL,    0},
        { "reg",         required_argument,  NULL,    'r'},
        { "val",         required_argument,  NULL,    'v'},   
        { NULL,          no_argument,        NULL,    0 },
    };

    while( -1 != ( getopt_ret = getopt_long(  argc
                                            , argv
                                            , "sgr:v:"
                                            , long_options
                                            , &option_index) ) ) {
        switch( getopt_ret ) {
        case 0: 
            if( strcmp( "get", long_options[option_index].name ) == 0 ) {
                globargs->action = e_reg_get;
            }
            if( strcmp( "set", long_options[option_index].name ) == 0 ) {
                globargs->action = e_reg_set;
            } 
            break;
        case 1:
            // handle non-option arguments here if you put a `-`
            // at the beginning of getopt_long's 3rd argument
            break;
        case 'v':
            globargs->value = atoi(optarg);
            break;
        case 'r':
            globargs->reg = convert_from_hex_string(optarg);
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
    printf("Type          : reg\n");
    printf("Pin           : 0x%02x\n", globargs->reg); 

    
    /*--------  action get  --------*/    
    if(globargs->action == e_reg_get) {
        printf("Action        : get\n");
    } 
    /*--------  action set  --------*/    
    if(globargs->action == e_reg_set) {
        printf("Action        : set\n");
    } 

    printf("Value         : %d\n",globargs->value);
}