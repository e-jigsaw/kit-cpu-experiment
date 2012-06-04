#include "kue-chip2-private.h"
#include "out_to_csv.h"
int non_interactive(io_data *d,const size_t step,const char *out_to){
    data result[2]={init(d),init(d)};
    result[0].in=&(result[1].out);
    result[1].in=&(result[0].out);
    result[0].cpuid=0;
    result[1].cpuid=1;
    int retcode=-1;
    
    for(size_t counter=0;retcode!=HLT&&retcode!=FAILURE&&counter<step;counter++){
        retcode=interpret(result);
        switch(retcode){
            case SUCCEEDED: case HLT:
                out_to_csv(out_to,result);
                (result)->memory_changed=NO_MODIFIED;
                free((result)->mnemonic_code);
                break;
            case FAILURE:
                fputs("The code is not executable:failed.\n",stderr);
                break;
        }
    }
    if(retcode==SUCCEEDED||retcode==HLT) puts("Ok.");
    return retcode;
}

int interactive(io_data *d, const size_t step, const char *out_to){
    data result[2]={init(d),init(d)};
    result[0].in=&(result[1].out);
    result[1].in=&(result[0].out);
    size_t cpu=0;
    int stop=0;
    while(!stop){
        printf("CPU%lu,PC=0x%02X>",cpu,result[cpu].pc);
        __attribute__((cleanup(destroy_mem))) char *line=NULL;
        char command=0x00;
        size_t n;
        getline(&line,&n,stdin);
        sscanf(line," %c",&command);
        if(command==0x00) continue;
        command=tolower(command);
        switch(command){
            case 't':cpu=(cpu+1)%2;break;
            case 'r':
            {
                __attribute__((cleanup(destroy_mem))) char *fname=malloc(strlen(line));
                
                if(sscanf(line," %*c %s",fname)<1){
                    fputs("!!Internal Error Load\n",stderr);
                    continue;
                }
                if(read_mem_file(result+cpu,fname)==SUCCEEDED) puts("Loaded.");
                else puts("Loading failed.");
            }
                break;
            case 'd':
                //output_register_dump(result+cpu);
                break;
            case 'm':
            {
                size_t addr=0;
                //if(sscanf(line," %*c %zx",&addr)<1) output_memory_at_all_adress(result+cpu);
                //else output_memory_dump(result+cpu,addr);
            }
                break;
            case 's':
            {
                char reg[3];
                bzero(reg,sizeof(reg));
                unsigned int value;
                int regid=0;
                if(sscanf(line," %*c %3s %x",reg,&value)<2){
                    fputs("!!Internal Error Set\n",stderr);
                    continue;
                }
                if(value>0xff){
                    fprintf(stderr,"Syntax Error: Invalid value:%02x\n",value);
                    continue;
                }
                tolower_str(reg);
                if(strcmp(reg,"acc")==0)        regid=ACC;
                else if(strcmp(reg,"ix")==0)    regid=IX;
                else if(strcmp(reg,"pc")==0)    regid=PC;
                else if(strcmp(reg,"in")==0)    regid=IN;
                else if(strcmp(reg,"out")==0)   regid=OUT;
                else{
                    fprintf(stderr,"Syntax Error: Unknown register:%s\n",reg);
                    continue;
                }
                setreg(regid,result+cpu,value);
            }
                break;
            case 'w':
            {
                size_t addr=0;
                unsigned int value=0;
                if(sscanf(line," %*c %zx %x",&addr,&value)<2){
                    fprintf(stderr,"Syntax Error: Invalid format\n");
                    continue;
                }
                if(value>0xff){
                    fprintf(stderr,"The value must be less than 0x100.\n");
                    continue;
                }
                set_mem(result+cpu,addr,value);
            }
                break;
            case 'i':
            {
                switch(interpret(result+cpu)){
                    case SUCCEEDED:
                        puts("The code is executed successfully.");
                        out_to_csv(out_to,result+cpu);
                        (result+cpu)->memory_changed=NO_MODIFIED;
                        free((result+cpu)->mnemonic_code);
                        break;
                    case HLT:
                        puts("The code is executed successfully.(Reached Exit code.)");
                        out_to_csv(out_to,result+cpu);
                        (result+cpu)->memory_changed=NO_MODIFIED;
                        free((result+cpu)->mnemonic_code);
                        break;
                    case FAILURE:
                        fputs("The code is not executable:failed.\n",stderr);
                        break;
                }
            }
                break;
            case 'c':
            {
                int retcode=-1;
                for(size_t counter=0;retcode!=HLT&&retcode!=FAILURE&&counter<step;counter++){
                    retcode=interpret(result+cpu);
                    switch(retcode){
                        case SUCCEEDED: case HLT:
                            out_to_csv(out_to,result+cpu);
                            //output_result(result+cpu):
                            (result+cpu)->memory_changed=NO_MODIFIED;
                            free((result+cpu)->mnemonic_code);
                            break;
                        case FAILURE:
                            fputs("The code is not executable:failed.\n",stderr);
                            break;
                    }
                }
            }
                break;
            case 'q':
                puts("Bye.");
                stop=1;
                break;
            default:
                fprintf(stderr,"Unknown command:%c\n",command);
                fflush(stderr);
                break;
        }
    }
    return EXIT_SUCCESS;
}

void help(){
    puts("t:Switch cpu.");
    puts("h(?):Show this help.");
    puts("q:Exit this software.");
    puts("r <script name>:Read memory file <script name>.");
    puts("d:Display registers.");
    puts("s <reg> <value>:Set the specified register <reg> to the specified value <value>.");
    puts("m [addr]: Show the value in the specified memory address [addr].");
    puts("w <addr> <value>: Set the specified memory address [addr] to the specified value <value>.");
    puts("i:Excecute a instruction");
    puts("c:Excecute instructions while HLT.");
}
