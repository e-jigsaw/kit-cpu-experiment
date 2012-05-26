#include <kue-chip2.h>
#include <string.h>

enum regs{ACC,IX,PC};
void interpret(data *);
data init(const io_data *d);

void step(data *);
void read_mem_file(data *);
void setreg(const int regid,const unsigned char value);
void set_mem(data *,const unsigned short offset,const unsigned char value);
void cont(data *);

void step(data *d){interpret(d);}
int non_interactive(const io_data *d,const size_t step,const char *out_to){
    data result[2]={init(d),init(d)};
    return 0;
}

int interactive(const io_data *d, const size_t step, const char *out_to){
    data result[2]={init(d),init(d)};
    return 0;
}
data init(const io_data *d){
    data result;
    if(d->program!=NULL) strcpy(result.program_memory,d->program);
    if(d->data_area!=NULL) strcpy(result.program_memory,d->program);
    if(d->initial_state!=NULL){
        result.acc=d->initial_state[0];
        result.ix=d->initial_state[1];
        result.in.bits=d->initial_state[2];
        result.flags=d->initial_state[3];
    }
    return result;
}
