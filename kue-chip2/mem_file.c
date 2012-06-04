#include"kue-chip2-private.h"

int read_mem_file(data *d,const char *fname){
    FILE *f=fopen(fname,"r");
    if(f==NULL) return FAILURE;
    size_t line=0;
    unsigned char program[program_memory_size],data[data_memory_size];
    unsigned char program_addr,data_addr;
    unsigned int addr;
    int now_editing=PROGRAM_AREA;
    program_addr=data_addr=addr=0;
    memcpy(program,d->program_memory,sizeof(program));
    memcpy(data,d->program_memory,sizeof(data));
    
    while(!feof(f)){
        line++;
        __attribute__((cleanup(destroy_mem))) char *fline=NULL;
        size_t n;
        char id[5];
        
        getline(&fline,&n,f);
        
        int n_apply=sscanf(fline," %5s %02x*",id,&addr);
        tolower_str(id);
        if(addr>255){
            fprintf(stderr,"Invalid argument %x for %s.\n %x must be less than/equal to 0xff.\n"
                    "Ignore this line.\n", addr, id,addr);
            fflush(stderr);
            continue;
        }
        if(strcmp(id,".text")==0){
            now_editing=PROGRAM_AREA;
            program_addr=(n_apply<2)?0:(unsigned char)addr;
        }else if(strcmp(id,".data")==0){
            now_editing=DATA_AREA;
            data_addr=(n_apply<2)?0:(unsigned char)addr;
        }else{
            unsigned int buf=0;
            unsigned int counter=0;
            for(counter=0;counter<strlen(fline)&&sscanf(fline+counter," %02x",&buf)==1;counter+=3){
                if(buf>0xff){
                    fprintf(stderr,"Syntax Error(Line:%lu):%02x must be less than 0xff.",line,buf);
                    return FAILURE;
                }
                switch(now_editing){
                    case PROGRAM_AREA:
                        program[program_addr++]=(unsigned char)buf;
                        break;
                    case DATA_AREA:
                        data[data_addr++]=(unsigned char)buf;
                        break;
                    default:
                        fprintf(stderr,"!! Internal Error:File:%s,line:%d",__FILE__,__LINE__);
                        return FAILURE;
                }
            }
            if(counter<strlen(fline)){
                fprintf(stderr,"Syntax Error(Line:%lu):Conversion failed.",line);
                return FAILURE;
            }
        }
    }
    memcpy(d->program_memory,program,program_memory_size);
    memcpy(d->data_memory,data,data_memory_size);
    fclose(f);
    return SUCCEEDED;
}
