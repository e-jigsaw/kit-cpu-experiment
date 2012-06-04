#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "out_to_csv.h"
void out_to_csv(const char *fname,const data *d){
    const char *file=(fname==NULL)?"result.csv":fname;
    FILE *f=fopen(file,"a");
    if(d->code_size==2) fprintf(f,"%d;%02X %02X;%s;%02X;%02X;%02X;%d;%d;%d;%d;",
                                d->cpuid,d->obj_code[0],d->obj_code[1],d->mnemonic_code,
                                d->pc,d->acc,d->ix,
                                (d->flags&0x08)>>3,(d->flags&0x04)>>2,(d->flags&0x02)>>1,(d->flags&0x01));
    else if(d->code_size==1) fprintf(f,"%d;%02X;%s;%02X;%02X;%02X;%d;%d;%d;%d;",
                                d->cpuid,d->obj_code[0],d->mnemonic_code,d->pc,d->acc,d->ix,
                                (d->flags&0x08)>>3,(d->flags&0x04)>>2,(d->flags&0x02)>>1,(d->flags&0x01));
    
    char in[11],out[11];
    memset(in,'_',sizeof(in));
    memset(out,'_',sizeof(out));
     in[0]='['; in[9]=']';
    out[0]='[';out[9]=']';
    in[10]=out[10]='\0';
    for(size_t index=0;index<8;index++){
        if ((d->in->bits&(0x01<<(index)))>>index)  in[index+1] ='*';
        if ((d->out.bits&(0x01<<(index)))>>index) out[index+1]='*';
    }
    fprintf(f,"%s;%s;",in,out);
    
    switch(d->memory_changed){
        case PROGRAM_AREA:
            fprintf(f,"%02tX:%02X->%02X",d->modified_addr,d->prev,d->now);
            break;
        case DATA_AREA:
            fprintf(f,"%02tX:%02X->%02X",d->modified_addr|0x100,d->prev,d->now);
            break;
        default:break;
    }
    
    fputs(";\n",f);

    fclose(f);
}
