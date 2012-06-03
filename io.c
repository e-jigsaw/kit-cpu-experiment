#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <property.h>

typedef struct stat stat_t;
void read_program_data(const char *fname,io_data *data){
    if(data==NULL){fputs("data is null.\n",stderr);return;}
    data->program=malloc(program_memory_size);
    if(data->program==NULL){
        fputs("Program memory allocation failed.\n",stderr);
        exit(EXIT_FAILURE);
    }
    memset(data->program,0x1f,program_memory_size);

    
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Program:open()"); return;}
    stat_t file_status;
    fstat(fd,&file_status);
    size_t size_to_read=(file_status.st_size<program_memory_size)?file_status.st_size:program_memory_size;
    read(fd,data->program,size_to_read);
    close(fd);
}

void read_initial_state(const char *fname,io_data *data){
    data->initial_state=malloc(4);
    if(data->initial_state==NULL){
        fputs("Initial State allocation failed.\n",stderr);
        exit(EXIT_FAILURE);
    }
    bzero(data->initial_state,4);
    
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Initial:open()"); return;}
    read(fd,data->initial_state,4);
    close(fd);
}

void read_data_area(const char *fname,io_data *data){
    if(data==NULL){fputs("data is null.\n",stderr);return;}
    data->data_area=malloc(data_memory_size);
    if(data->data_area==NULL){
        fputs("Data memory allocation failed.\n",stderr);
        exit(EXIT_FAILURE);
    }
    memset(data->data_area,0x1f,data_memory_size);
    
    if(fname==NULL) return;
    int fd=open(fname,O_RDONLY);
    if(fd<0){perror("Data:open()"); return;}
    stat_t file_status;
    fstat(fd,&file_status);
    size_t size_to_read=(file_status.st_size<data_memory_size)?file_status.st_size:data_memory_size;
    read(fd,data->data_area,size_to_read);
    close(fd);
}
