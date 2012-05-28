#include <kue-chip2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum instruction_mask{
    NP =0x00,IO =0x10,CF =0x20,LD =0x60,ST =0x70,ADD=0xB0,ADC=0x90,SUB=0xA0,
    SBC=0x80,CMP=0xF0,AND=0xE0,OR =0xD0,EOR=0xC0,SR =0x40,B  =0x30
};
enum operandB{
    ACC=0,IX=1,VA1=2,VA2=3,
    AAP=4,AAD=5,IDXP=6,IDXD=7,
    //This is used for REGID.
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
    result[0].in=&result[1].out;
    result[1].in=&result[0].out;
    
    free(result[0].mnemonic_code);
    free(result[1].mnemonic_code);
    return 0;
}

int interactive(io_data *d, const size_t step, const char *out_to){
    data result[2]={init(d),init(d)};
    result[0].in=&result[1].out;
    result[1].in=&result[0].out;
    
    free(result[0].mnemonic_code);
    free(result[1].mnemonic_code);
    return 0;
}
data init(io_data *d){
    if(d==NULL||d->program==NULL||d->data_area==NULL||d->initial_state==NULL){
        fputs("io_data must not be null.\n",stderr);
        exit(EXIT_FAILURE);
    }
    data result;
    result.pc=0;
    result.program_memory=d->program;
    result.data_memory=d->data_area;
    result.acc=d->initial_state[0];
    result.ix=d->initial_state[1];
    result.out.bits=d->initial_state[2];
    result.flags=d->initial_state[3];
    result.mnemonic_code=NULL;
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
    
    d->obj_code[0]=*instruction_byte;
    d->obj_code[1]=0x0;
    
    switch(instruction){
        case IO:
            if((*instruction_byte)&0x8){
                d->mnemonic_code=malloc(strlen("IN"));
                strcpy(d->mnemonic_code,"IN");
                d->acc=d->in->bits;
            }
            else{
                d->mnemonic_code=malloc(strlen("OUT"));
                strcpy(d->mnemonic_code,"OUT");
                d->out.bits=d->acc;
                d->out.flag=1;
            }
            break;
        case CF:
            d->mnemonic_code=malloc(3);
            if((*instruction_byte)&0x8){
                strcpy(d->mnemonic_code,"SCF");
                d->flags|=0x8;
            }else{
                strcpy(d->mnemonic_code,"RCF");
                d->flags&=0x7;
            }
            break;
        case LD: case ST: case ADD:case SUB:case CMP:case ADC:case SBC:case AND:case OR:case EOR:
        {
            const char *mnemonic_head;
            
            unsigned char opBtype=(*instruction_byte)&0x7;
            unsigned char *opA;
            char *opAstr;
            
            if(isOpA_acc(*instruction_byte)){opA=&(d->acc);opAstr="ACC";}
            else{opA=&(d->ix);opAstr="IX";}
            
            unsigned char *opB=getOpBAddr(opBtype,d);
            char *opBstr=operand2str(opBtype,d);
            d->obj_code[1]=((opBtype&0x6)>>1)?*(instruction_byte+1):0x0;
            
            switch(instruction){
                case LD:
                    mnemonic_head="LD";
                    (*opA)=(*opB);
                    break;
                case ST:
                    if(!((opBtype&0x4)>>2)){
                        fputs("Stntax error: you can't specify a register to operandB.\n",stderr);
                        return;
                    }
                    mnemonic_head="ST";
                    setModifiedPoint(opBtype,opB,d);
                    d->prev=*opB;
                    (*opB)=isOpA_acc(*instruction_byte)?d->acc:d->ix;
                    d->now=*opB;
                    break;
                case ADD:case SUB:case CMP:case ADC:case SBC:
                {
                    unsigned short result;
                    switch(instruction){
                        case ADD:
                            mnemonic_head="ADD";
                            result=*opA+*opB;
                            *opA=(result&0x00ff);
                            break;
                        case SUB:
                            mnemonic_head="SUB";
                            result=*opA-*opB;
                            *opA=(result&0x00ff);
                            break;
                        case CMP:
                            mnemonic_head="CMP";
                            result=*opA-*opB;
                            break;
                        case ADC:case SBC:
                        {
                            switch(instruction){
                                case ADC:
                                    mnemonic_head="ADC";
                                    result=*opA+*opB+(d->flags&0x08>>7);
                                    *opA=(result&0x00ff);
                                    break;
                                case SBC:
                                    mnemonic_head="SBC";
                                    result=*opA-*opB-(d->flags&0x08>>7);
                                    *opA=(result&0x00ff);
                                    break;
                            }
                            d->flags&=(((result&0x0100)>>5)|0x07);
                        }
                    }
                    d->flags&=(0x08|((result>0x007f)<<2)|((result&0x0080)>>6)|(result==0));
                }
                    break;
                case AND: case OR: case EOR:
                {
                    unsigned char result;
                    switch(instruction){
                        case AND:
                            mnemonic_head="AND";
                            result=(*opA)&(*opB);
                            break;
                        case OR:
                            mnemonic_head="OR";
                            result=(*opA)|(*opB);
                            break;
                        case EOR:
                            mnemonic_head="EOR";
                            result=(*opA)^(*opB);
                            break;
                    }
                    (*opA)=result;
                    d->flags&=(0x08|((result&0x0080)>>6)|(result==0));
                }
                    break;
            }
            d->mnemonic_code=malloc(strlen(mnemonic_head)+strlen(" ")+
                                    strlen(opAstr)+strlen(",")+strlen(opBstr));
            strcpy(d->mnemonic_code,mnemonic_head);
            strcat(d->mnemonic_code," ");
            strcat(d->mnemonic_code,opAstr);
            strcat(d->mnemonic_code,",");
            strcat(d->mnemonic_code,opBstr);
            free(opBstr);
        }
            break;
    }
}
void setModifiedPoint(unsigned char opBtype,unsigned char *ptr, data *d){
    if(!((opBtype&0x4)>>2)){
        fputs("Stntax error: you can't specify a register to operandB.\n",stderr);
        return;
    }
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
        default: fputs("Operator B is invalid.\n",stderr); return NULL;
    }
    return NULL;
}
