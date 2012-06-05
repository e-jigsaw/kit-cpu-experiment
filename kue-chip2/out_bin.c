#include "out_bin.h"
#include "memory.h"
#include "kue-chip2-private.h"
#include <property.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void auto_close(int *desc){close(*desc);}

int out_bin(const data *d,const char *name){
    const char *pgr_ext=".bin",
               *dat_ext=".dat",
               *init_ext=".init";
    __attribute__((cleanup(destroy_mem))) 
            char *program_name=calloc(sizeof(char),strlen(name)+strlen(pgr_ext)+1),
                 *data_name   =calloc(sizeof(char),strlen(name)+strlen(dat_ext)+1),
                 *init_name   =calloc(sizeof(char),strlen(name)+strlen(init_ext)+1);
    
    strcpy(program_name,name);
    strcat(program_name,pgr_ext);
    strcpy(data_name,name);
    strcat(data_name,dat_ext);
    strcpy(init_name,name);
    strcat(init_name,init_ext);
    
    mode_t mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int open_flag=O_EXCL|O_CREAT|O_WRONLY;
    __attribute__((cleanup(auto_close)))
            int pgr_d=open(program_name,open_flag,mode),dat_d=open(data_name,open_flag,mode),
        init_d=open(init_name,open_flag,mode);
    if(pgr_d<0||dat_d<0||init_d<0){
        fputs("Open error: The specified file group exists.\n",stderr);
        return FAILURE;
    }
    write(pgr_d,d->program_memory,program_memory_size);
    write(dat_d,d->data_memory,data_memory_size);
    unsigned char init_data[4]={d->acc,d->ix,d->out.bits,d->flags<<4};
    write(init_d,init_data,sizeof(init_data));
    return SUCCEEDED;
}
