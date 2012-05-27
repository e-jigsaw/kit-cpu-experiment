#include <kue-chip2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum instruction_mask{
    NP =0x00,
    IO =0x10,
    CF =0x20,
    LD =0x60,
    ST =0x70,
    ADD=0xB0,
    ADC=0x90,
    SUB=0xA0,
    SBC=0x80,
    CMP=0xF0,
    AND=0xE0,
    OR =0xD0,
    EOR=0xC0,
    SR =0x40,
    B  =0x30
};
enum operandB{
    ACC=0,
    IX=1,
    VA1=2,
    VA2=3,
    AAP=4,
    AAD=5,
    IDXP=6,
    IDXD=7,
    //This is used for CPUID.
    PC=8
};
void interpret(data *);
data init(io_data *d);

void (*step)(data *)=interpret;
void read_mem_file(data *);
void setreg(const int regid,const unsigned char value);
void set_mem(data *,const unsigned short offset,const unsigned char value);
void cont(data *);

unsigned char *getOpBAddr(const unsigned char opB,data *d);
unsigned char isOpA_acc(const unsigned char inst);
char *operand2str(const unsigned char operand, const data *d);

void setModifiedPoint(unsigned char opBtype, unsigned char *ptr, data *d);


int non_interactive(io_data *d,const size_t step,const char *out_to){
    data result[2]={init(d),init(d)};
    return 0;
}

int interactive(io_data *d, const size_t step, const char *out_to){
    data result[2]={init(d),init(d)};
    result[0].in=&result[1].out;
    result[1].in=&result[0].out;
    return 0;
}
data init(io_data *d){
    data result;
    result.pc=0;
    result.program_memory=d->program;
    result.data_memory=d->data_area;
    
    if(d->initial_state==NULL){
        result.acc=0;
        result.ix=0;
        result.out.bits=0;
        result.flags=0;
    }else{
        result.acc=d->initial_state[0];
        result.ix=d->initial_state[1];
        result.out.bits=d->initial_state[2];
        result.flags=d->initial_state[3];
    }
    return result;
}
void interpret(data *d){
    if(d==NULL){
        fputs("Null Pointer Exception\n",stderr);
        return;
    }
    d->obj_code[1]=0x0;
    unsigned char *instruction_byte=d->program_memory+(++(d->pc));
    unsigned char instruction=(*instruction_byte)&0xf0;
    unsigned char opBtype=(*instruction_byte)&0x7;
    unsigned char obj1_copy_needed=((opBtype&0x6)>>1);
    
    switch(instruction){
        case IO:
            d->obj_code[0]=(*instruction_byte);
            if((*instruction_byte)&0x8){
                d->mnemonic_code=malloc(2);
                strcpy(d->mnemonic_code,"IN");
                d->acc=d->in->bits;
            }
            else{
                d->mnemonic_code=malloc(3);
                strcpy(d->mnemonic_code,"OUT");
                d->out.bits=d->acc;
                d->out.flag=1;
            }
            break;
        case CF:
            d->obj_code[0]=(*instruction_byte);
            d->mnemonic_code=malloc(3);
            if((*instruction_byte)&0x8){
                strcpy(d->mnemonic_code,"RCF");
                d->flags&=0x7;
            }else{
                strcpy(d->mnemonic_code,"RCF");
                d->flags|=0x8;
            }
            break;
        case LD:
        {
            d->obj_code[0]=(*instruction_byte);
            d->obj_code[1]=(obj1_copy_needed)?*(instruction_byte+1):0x0;
            unsigned char *src=getOpBAddr(opBtype,d);
            char *opBstr=operand2str(opBtype,d);
            
            if(isOpA_acc(*instruction_byte)){
                d->acc=(*src);
                d->mnemonic_code=malloc(7+strlen(opBstr));
                strcpy(d->mnemonic_code,"LD ACC,");
                strcat(d->mnemonic_code,opBstr);
            }else{
                d->ix=(*src);
                d->mnemonic_code=malloc(6+strlen(opBstr));
                strcpy(d->mnemonic_code,"LD IX,");
                strcat(d->mnemonic_code,opBstr);
            }
            free(opBstr);
        }
            break;
        case ST:
        {
            if(!((opBtype&0x4)>>2)){
                fputs("Stntax error: you can't specify a register to operandB.",stderr);
                return;
            }
            memcpy(d->obj_code,instruction_byte,2);

            unsigned char *dest=getOpBAddr(opBtype,d);
            setModifiedPoint(opBtype,dest,d);
            d->prev=*dest;
            char *opBStr=operand2str(opBtype,d);
            if(isOpA_acc(*instruction_byte)){
                (*dest)=d->acc;
                d->mnemonic_code=malloc(7+strlen(opBStr));
                strcpy(d->mnemonic_code,"ST ACC,");
                strcat(d->mnemonic_code,opBStr);
            }else{
                (*dest)=d->ix;
                strcpy(d->mnemonic_code,"ST IX,");
                strcat(d->mnemonic_code,opBStr);
            }
            free(opBStr);
            d->now=*dest;
        }
            break;
    }
}
void setModifiedPoint(unsigned char opBtype,unsigned char *ptr, data *d){
    switch((opBtype&0x3)){
        case 0: case 2:
            d->memory_changed=PROGRAM_AREA;
            d->modified_addr=ptr-d->program_memory;
            break;
        case 1:case 3:
            d->memory_changed=DATA_AREA;
            d->modified_addr=ptr-d->data_memory;
            break;
    }
}

unsigned char isOpA_acc(const unsigned char inst){return (inst&0x8>>3);}

char *operand2str(const unsigned char operand,const data *d){
    char *opstr;
    switch(operand){
        case ACC:
            opstr=malloc(3);
            opstr=strcpy(opstr,"ACC");
            break;
        case IX:
            opstr=malloc(2);
            opstr=strcpy(opstr,"IX");
            break;
        case VA1: case VA2:
            opstr=malloc(3);
            sprintf(opstr,"%2Xd",d->obj_code[1]);
            break;
        case AAP:
            opstr=malloc(5);
            sprintf(opstr,"[%2Xd]",d->obj_code[1]);
            break;
        case AAD:
            opstr=malloc(5);
            sprintf(opstr,"(%2Xd)",d->obj_code[1]);
            break;
        case IDXP:
            opstr=malloc(8);
            sprintf(opstr,"[IX+%2Xd]",d->obj_code[1]);
            break;
        case IDXD:
            opstr=malloc(8);
            sprintf(opstr,"(IX+%2Xd)",d->obj_code[1]);
            break;
    }
    return opstr;
}

unsigned char *getOpBAddr(const unsigned char opB,data *d){
    switch(opB){
        case ACC:           return &d->acc;
        case IX:            return &d->ix;
        case VA1: case VA2: return d->program_memory+(++(d->pc));
        case AAP:           return d->program_memory+d->program_memory[++(d->pc)];
        case AAD:           return d->data_memory+d->program_memory[++(d->pc)];
        case IDXP:          return d->program_memory+(d->program_memory[++(d->pc)]+d->ix);
        case IDXD:          return d->data_memory+(d->program_memory[++(d->pc)]+d->ix);
    }
}
