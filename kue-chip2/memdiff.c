#include"kue-chip2-private.h"
void setModifiedPoint(unsigned char opBtype,unsigned char *ptr, data *d){
    if(!((opBtype&0x4)>>2)){
        fputs("Interpret Error: you can't specify a register to operandB.\n",stderr);
        return;
    }
    switch((opBtype&0x3)){
        case 0: case 2: d->memory_changed=PROGRAM_AREA; d->modified_addr=ptr-d->program_memory; break;
        case 1:case 3:  d->memory_changed=DATA_AREA;    d->modified_addr=ptr-d->data_memory;    break;
    }
}