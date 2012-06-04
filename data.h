#pragma once
#include <stdio.h>
#include <stddef.h>
enum state_flag{NO_MODIFIED,PROGRAM_AREA,DATA_AREA};

struct io_data{
    unsigned char *program;
    unsigned char *data_area;
    unsigned char *initial_state;
}typedef io_data;

struct buf{
    unsigned char bits;
    int flag;
}typedef buf;

struct data{
    unsigned short cpuid;
    unsigned char obj_code[2];
    size_t code_size;
    char *mnemonic_code;
    unsigned char pc,acc,ix;
    unsigned char flags;
    buf *in,out;
    unsigned char *program_memory;
    unsigned char *data_memory;
    //the value is tri-state. look at changed_flag.
    int memory_changed;
    ptrdiff_t modified_addr;
    unsigned char prev;
    unsigned char now;
} typedef data;
