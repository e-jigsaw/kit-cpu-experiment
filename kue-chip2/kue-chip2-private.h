#pragma once
#include <data.h>
#include <stdlib.h>
#include <property.h>
#include "string.h"
#include "memory.h"
enum instruction_mask{
    NP =0x00,IO =0x10,CF =0x20,LD =0x60,ST =0x70,ADD=0xB0,ADC=0x90,SUB=0xA0,
    SBC=0x80,CMP=0xF0,AND=0xE0,OR =0xD0,EOR=0xC0,SR =0x40,B  =0x30
};
enum operandB{
    ACC=0,IX=1,VA1= 2,VA2=3,
    AAP=4,AAD=5,IDXP=6,IDXD=7,PC=8
};
enum SRTYPE{
    SRA=0x0,SLA=0x1,SRL=0x2,SLL=0x3,
    RRA=0x4,RLA=0x5,RRL=0x6,RLL=0x7
};
enum BRANCH_CONDITION_TYPE{
        A=0x0,VF=0x8,NZ=0x1,Z=0x9,ZP=0x2,N=0x0a,P=0x3,ZN=0xb,NI=0x4,
        NO=0xC,NC=0x5,C=0xD,GE=0x6,LT=0xE,GT=0x7,LE=0xf
};
enum EXIT_STATUS{HLT,SUCCEEDED,FAILURE};

int read_mem_file(data *,const char*);
void setreg(const int regid,data *d,const unsigned char value);
void set_mem(data *,const unsigned short offset,const unsigned char value);
int cont(data *,const unsigned char stop_addr);

unsigned char *getOpBAddr(const unsigned char opB,data *d);
unsigned char isOpA_acc(const unsigned char inst);
char *operand2str(const unsigned char operand, const data *d);

void setModifiedPoint(unsigned char opBtype, unsigned char *ptr, data *d);
data init(io_data *d);
int interpret(data *);

void help();
