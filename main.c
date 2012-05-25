#include <data.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#define free_mem \
    free(initial_filepath);\
    free(program_filepath);\
    free(data_filepath);\
    free(out_to)

#define free_data \
    free(d.program); \
    free(d.data_area); \
    free(d.initial_state); 

void showDesc(char *apppath);
int main(int argc,char *argv[]){
    int invalid=0,loopid=0,per_inst=0;
    size_t step=0;
    char *initial_filepath,*program_filepath,*data_filepath,*out_to;
    initial_filepath=program_filepath=data_filepath=NULL;
    
    struct option long_options[]={
        {"initial-state",required_argument,NULL,'z'},
        {"data-area",required_argument,NULL,'d'},
        {"step",required_argument,NULL,'s'},
        {"output",required_argument,NULL,'o'},
        {"input",required_argument,NULL,'i'},
        {"per-inst",no_argument,NULL,'p'},
        {0,0,0,0}
    };
    
    while((loopid=1)||1){
        int c=getopt_long(argc,argv,"z:d:s:o:i:p",long_options,NULL);
        if(c<0&&loopid==0){invalid=1;break;}
        else if(c<0&&loopid>0) break;
        
        switch(c){
            case 'z':
                initial_filepath=malloc(strlen(optarg));
                strcpy(initial_filepath,optarg);
                break;
            case 'd':
                data_filepath=malloc(strlen(optarg));
                strcpy(data_filepath,optarg);
                break;
            case 's':
                step=atol(optarg);
                break;
            case 'o':
                out_to=malloc(strlen(optarg));
                strcpy(out_to,optarg);
                break;
            case 'p':
                per_inst=1;
                break;
            case 'i':
            default:
                program_filepath=malloc(strlen(optarg));
                break;
        }
    }
    
    if(invalid) showDesc(argv[0]);
    if(program_filepath==NULL){
        fputs("You have to specify program filepath.\n",stderr);
        free_mem;
        return EXIT_FAILURE;
    }
    
    /*Read files*/
    io_data d;
    
    read_initial_state(initial_filepath,&d);
    read_program_data(program_filepath,&d);
    read_data_area(data_filepath,&d);
    
    puts("File read successfully.");
    
    /*Transform*/
    if(per_inst){
        
    }else{
        
    }
    
    /*Clean data*/
    free_data;
    free_mem;
    return 0;
}
void showDesc(char *apppath){
    char *title="KUE-Chip2 Simulator";
    puts(title);
    for(int i=0;i<strlen(title);i++) putchar('=');
    puts("");
    printf("Usage: %s [option] [--input|-i] [program file] \n",apppath);
    puts("\tOptions:");
    puts("\t--initial-state (-i) <filename>:Sets an initial registers/flags state.");
    puts("\t--data-area (-d) <filename>\t:Sets data into data area.");
    puts("\t--step (-s)\t:Sets step-limitation");
    puts("\t--output (-o)\t:Sets an output file.");
    puts("\t--input (-i)\t:Sets a machine language file.");
    puts("\t--per-inst (-p)\t:Interactive Mode. Runs instructions per one instruction.");
}
