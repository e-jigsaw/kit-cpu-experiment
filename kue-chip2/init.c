#include "kue-chip2-private.h"
data init(io_data *d){
    if(d==NULL||d->program==NULL||d->data_area==NULL||d->initial_state==NULL){
        fputs("io_data must not be null.\n",stderr);
        exit(EXIT_FAILURE);
    }
    data result;
    result.pc=0x0;
    result.program_memory=d->program;
    result.data_memory=d->data_area;
    result.acc=d->initial_state[0];
    result.ix=d->initial_state[1];
    result.out.bits=d->initial_state[2];
    result.flags=d->initial_state[3];
    result.mnemonic_code=NULL;
    return result;
}