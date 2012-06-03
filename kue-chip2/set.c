#include "kue-chip2-private.h"
void set_mem(data *d,const unsigned short offset,const unsigned char value){
    if(offset<0x100) d->program_memory[offset]=value;
    else d->data_memory[offset^0x100]=value;
}

void setreg(const int regid, data *d, const unsigned char value){
    switch(regid){
        case ACC:d->acc=value;break;
        case IX: d->ix=value; break;
        case PC: d->pc=value; break;
    }
}
