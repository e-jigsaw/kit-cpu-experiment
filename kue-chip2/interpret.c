#include "kue-chip2-private.h"
int interpret(data *d){
    if(d==NULL){
        fputs("Null Pointer Exception\n",stderr);
        return FAILURE;
    }
    int exit_flag=SUCCEEDED;
    unsigned char *instruction_byte=d->program_memory+((d->pc)++);
    unsigned char instruction=(*instruction_byte)&0xf0;
    
    d->obj_code[0]=*instruction_byte;
    d->code_size=1;
    d->obj_code[1]=0x0;
    
    switch(instruction){
        case IO:
            if((*instruction_byte)&0x8){
                d->mnemonic_code=calloc(sizeof(char),strlen("IN")+1);
                strcpy(d->mnemonic_code,"IN");
                d->acc=d->in->bits;
            }
            else{
                d->mnemonic_code=calloc(sizeof(char),strlen("OUT")+1);
                strcpy(d->mnemonic_code,"OUT");
                d->out.bits=d->acc;
                d->out.flag=1;
            }
            break;
        case CF:
            d->mnemonic_code=calloc(sizeof(char),4);
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
            __attribute__((cleanup(destroy_mem))) char *opBstr=operand2str(opBtype,d);
            d->obj_code[1]=((opBtype&0x6)>>1)?*(instruction_byte+1):0x0;
            
            switch(instruction){
                case LD:
                    mnemonic_head="LD";
                    (*opA)=(*opB);
                    break;
                case ST:
                    if(!((opBtype&0x4)>>2)){
                        fputs("Interpret Error: you can't specify a register to operandB.\n",stderr);
                        return FAILURE;
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
                    unsigned char cflag=d->flags&0x08;
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
                            result=(*opA)-(*opB);
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
                            cflag=((result&0x0100)>>5);
                        }
                    }
                    d->flags=(cflag|((result>0x007f)<<2)|((result&0x0080)>>6)|(result==0));
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
                    d->flags=(d->flags|((result&0x0080)>>6)|(result==0));
                }
                    break;
            }
            d->mnemonic_code=calloc(sizeof(char),strlen(mnemonic_head)+strlen(" ")+
                                    strlen(opAstr)+strlen(",")+strlen(opBstr)+1);
            strcpy(d->mnemonic_code,mnemonic_head);
            strcat(d->mnemonic_code," ");
            strcat(d->mnemonic_code,opAstr);
            strcat(d->mnemonic_code,",");
            strcat(d->mnemonic_code,opBstr);
        }
            break;
        case SR:
        {
            unsigned char *opA;
            char *opAstr;
            const char *mnemonic_head;
            if(isOpA_acc(instruction)){opA=&(d->acc);opAstr="ACC";}
            else{opA=&(d->ix);opAstr="IX";}
            
            unsigned char type=(*instruction_byte)&0x07;
            unsigned char cflag=0,vflag=0;
            switch(type){
                case SRA: case SRL: case RRA: case RRL:
                {
                    cflag=((*opA)&0x1)<<3;
                    switch(type){
                        case SRA:
                            mnemonic_head="SRA";
                            (*opA)=((*opA)>>1)|((*opA)&0x80);
                            break;
                        case SRL:
                            mnemonic_head="SRL";
                            (*opA)>>=1;
                            break;
                        case RRA:
                            mnemonic_head="RRA";
                            (*opA)=((*opA)>>1)|((d->flags&0x08)>>3);
                            break;
                        case RRL:
                            mnemonic_head="RRL";
                            (*opA)=((*opA)>>1)|(cflag<<4);
                            break;
                    }
                }
                    break;
                case SLA: case SLL: case RLA: case RLL:
                {
                    cflag=((*opA)&0x80)>>4;
                    switch(type){
                        case SLA: case RLA:
                            if(*opA<128&&*opA>0x40) vflag=4;
                            if(*opA>127) vflag=4;
                            switch(type){
                                case SLA:
                                    mnemonic_head="SLA";
                                    (*opA)<<=1;
                                    break;
                                case RLA:
                                    mnemonic_head="SLA";
                                    (*opA)=(*opA)<<1|((d->flags&0x8)>>3);
                                    break;
                            }
                            break;
                        case SLL: case RLL:
                            switch(type){
                                case SLL:
                                    mnemonic_head="SLL";
                                    (*opA)<<=1;
                                    break;
                                case RLL:
                                    mnemonic_head="RLL";
                                    (*opA)=(*opA)<<1|(cflag>>3);
                                    break;
                            }
                            break;
                    }
                }
                    break;
            }
            d->flags=cflag|vflag|(((*opA)&0x80)>>6)|((*opA)==0);
            d->mnemonic_code=calloc(sizeof(char),strlen(mnemonic_head)+strlen(" ")+strlen(opAstr)+1);
            strcpy(d->mnemonic_code,mnemonic_head);
            strcat(d->mnemonic_code," ");
            strcat(d->mnemonic_code,opAstr);
        }
            break;
        case B:
        {
            d->code_size=2;
            d->obj_code[1]=d->program_memory[(d->pc)++];
            unsigned char condition_type=d->obj_code[0]&0xf;
            const char *mnemonic_head;
            int jmp=0;
            switch(condition_type){
                case A: mnemonic_head="BA";                                                                         jmp=1; break;
                case VF:mnemonic_head="BVF"; if((d->flags&0x04)>>2)                                                 jmp=1; break;
                case NZ:mnemonic_head="BNZ"; if(!(d->flags&0x01))                                                   jmp=1; break;
                case Z: mnemonic_head="BZ";  if(d->flags&0x01)                                                      jmp=1; break;
                case ZP:mnemonic_head="ZP";  if(!((d->flags&0x2)>>1))                                               jmp=1; break;
                case N: mnemonic_head="N";   if((d->flags&0x2)>>1)                                                  jmp=1; break;
                case P: mnemonic_head="P";   if(!(d->flags&0x01)&&!((d->flags&0x02)>>1))                            jmp=1; break;
                case ZN:mnemonic_head="ZN";  if((d->flags&0x01)||((d->flags&0x02)>>1))                              jmp=1; break;
                case NI:mnemonic_head="NI";  if(!d->in->flag)                                                       jmp=1; break;
                case NO:mnemonic_head="NO";  if(!d->out.flag)                                                       jmp=1; break;
                case NC:mnemonic_head="NC";  if(!((d->flags&0x08)>>3))                                              jmp=1; break;
                case C: mnemonic_head="C";   if((d->flags&0x08)>>3)                                                 jmp=1; break;
                case GE:mnemonic_head="GE";  if(!(((d->flags&0x04)>>2)^((d->flags&0x02)>>1)))                       jmp=1; break;
                case LT:mnemonic_head="LT";  if(((d->flags&0x04)>>2)^((d->flags&0x02)>>1))                          jmp=1; break;
                case GT:mnemonic_head="GT";  if(!((((d->flags&0x04)>>2)^((d->flags&0x02)>>1))|(d->flags&0x01)))     jmp=1; break;
                case LE:mnemonic_head="LE";  if((((d->flags&0x04)>>2)^((d->flags&0x02)>>1))|(d->flags&0x01))        jmp=1; break;
            }
            if(jmp) d->pc=d->obj_code[1];
            
            d->mnemonic_code=calloc(sizeof(char),strlen(mnemonic_head)+strlen(" ")+3);
            strcpy(d->mnemonic_code,mnemonic_head);
            strcat(d->mnemonic_code," ");
            sprintf(d->mnemonic_code+strlen(mnemonic_head)+strlen(" "),"%02X",d->obj_code[1]);
        }
            break;
        case NP:
        {
            unsigned char op=(*instruction_byte)&0xf;
            const char *mnemonic_head;
            d->code_size=1;
            if(!((op&0x8)>>3)){ mnemonic_head="NOP";}
            else if((op&0xc>>2)==3){ mnemonic_head="HLT"; exit_flag=HLT;}
            else{
                switch(op){
                    case 0xa: //JAL
                        d->code_size=2;
                        d->obj_code[1]=d->program_memory[(d->pc)++];
                        d->acc=d->pc+2;
                        d->pc=d->obj_code[1];
                        mnemonic_head="JAL";
                        break;
                    case 0xb://JR
                        mnemonic_head="JR";
                        d->pc=d->acc;
                        break;
                }
            }
            if(d->code_size==1){
                d->mnemonic_code=calloc(sizeof(char),strlen(mnemonic_head)+1);
                strcpy(d->mnemonic_code,mnemonic_head);
            }else{
                d->mnemonic_code=calloc(sizeof(char),strlen(mnemonic_head)+strlen(" ")+3);
                strcpy(d->mnemonic_code,mnemonic_head);
                sprintf(d->mnemonic_code+strlen(mnemonic_head)+strlen(" "),"%02X",d->obj_code[1]);
            }
        }
            break;
        default:
            fputs("Interpret Error:Unknown instruction.\n",stderr);
            return FAILURE;
    }
    return exit_flag;
}
