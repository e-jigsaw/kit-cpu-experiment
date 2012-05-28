#pragma once
enum changed_flag{NO_MODIFIED,PROGRAM_AREA,DATA_AREA};

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
    unsigned char obj_code[2];
    char *mnemonic_code;
    unsigned char pc,acc,ix;
    unsigned char flags;
    buf *in,out;
    unsigned char *program_memory;
    unsigned char *data_memory;
    //the value is tri-state. look at changed_flag.
    int memory_changed;
    unsigned char modified_addr;
    unsigned char prev;
    unsigned char now;
} typedef data;
