#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct stat stat_t;
void read_program_data(const char *fname,io_data *data){
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Program:open()"); return;}
    stat_t file_status;
    fstat(fd,&file_status);
    data->program=malloc(file_status.st_size);
    if(data->program==NULL){
        fputs("Program memory allocation failed.\n",stderr);
        exit(EXIT_FAILURE);
    }
    if(data==NULL) 
    read(fd,data->program,file_status.st_size);
    close(fd);
}

void read_initial_state(const char *fname,io_data *data){
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Initial:open()"); return;}
    stat_t file_status;
    fstat(fd,&file_status);
    data->data_area=malloc(file_status.st_size);
    if(data->data_area==NULL){
        fputs("Data memory allocation failed.\n",stderr);
        return;
    }
    read(fd,data->data_area,file_status.st_size);
    close(fd);
}

void read_data_area(const char *fname,io_data *data){
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Data:open()"); return;}
    stat_t file_status;
    fstat(fd,&file_status);
    data->initial_state=malloc(file_status.st_size);
    if(data->initial_state==NULL){
        fputs("Data memory allocation failed.\n",stderr);
        return;
    }
    read(fd,data->initial_state,file_status.st_size);
    close(fd);
}
