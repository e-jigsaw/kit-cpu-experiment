#include "kue-chip2-private.h"
unsigned char isOpA_acc(const unsigned char inst){return ((inst&0x8)>>3)==0;}

char *operand2str(const unsigned char operand,const data *d){
    char *opstr;
    switch(operand){
        case ACC:                   opstr=calloc(sizeof(char),4);    strcpy(opstr,"ACC");                        break;
        case IX:                    opstr=calloc(sizeof(char),3);    strcpy(opstr,"IX");                         break;
        case VA1: case VA2:         opstr=calloc(sizeof(char),4);    sprintf(opstr,"%02Xd",d->obj_code[1]);       break;
        case AAP:                   opstr=calloc(sizeof(char),6);    sprintf(opstr,"[%02Xd]",d->obj_code[1]);     break;
        case AAD:                   opstr=calloc(sizeof(char),6);    sprintf(opstr,"(%02Xd)",d->obj_code[1]);     break;
        case IDXP:                  opstr=calloc(sizeof(char),9);    sprintf(opstr,"[IX+%02Xd]",d->obj_code[1]);  break;
        case IDXD:                  opstr=calloc(sizeof(char),9);    sprintf(opstr,"(IX+%02Xd)",d->obj_code[1]);  break;
    }
    return opstr;
}

unsigned char *getOpBAddr(const unsigned char opB,data *d){
    switch(opB){
        case ACC:           return &d->acc;
        case IX:            return &d->ix;
        case VA1: case VA2: d->code_size++; return d->program_memory+((d->pc)++);
        case AAP:           d->code_size++; return d->program_memory+d->program_memory[(d->pc)++];
        case AAD:           d->code_size++; return d->data_memory+d->program_memory[(d->pc)++];
        case IDXP:          d->code_size++; return d->program_memory+(d->program_memory[(d->pc)++]+d->ix);
        case IDXD:          d->code_size++; return d->data_memory+(d->program_memory[(d->pc)++]+d->ix);
        default: fputs("Operator B is invalid.\n",stderr); return NULL;
    }
    return NULL;
}
