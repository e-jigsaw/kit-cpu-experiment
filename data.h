#pragma once
struct io_data{
    unsigned char *program;
    unsigned char *data_area;
    unsigned char *initial_state;
}typedef io_data;

struct data{
    unsigned short obj_code;
    unsigned char *mnemonic_code;
    unsigned char pc,acc,ix;
    unsigned char flags;
    unsigned char in,out;
    unsigned char memory[256];
    int memory_changed;
    unsigned char modified_addr;
    unsigned char prev;
    unsigned char now;
} typedef data;
