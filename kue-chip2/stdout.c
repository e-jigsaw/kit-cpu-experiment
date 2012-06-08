#include "stdout.h"
#include <stdio.h>

int calc1(unsigned char a) {
	if((a>>7)) return (-1) * (int)((~a)+1);
	else return (int)a;
}

void output_register_dump(const data *d) {
	printf("ACC: %.2x (%d, %d) IX: %.2x (%d, %d)\nCF: %.2x VF: %.2x NF: %.2x ZF: %.2x\nIN: %d:%.2x (%d, %d) OUT: %d:%.2x (%d, %d)\n", 
	d->acc, calc1(d->acc), d->acc, 
	d->ix, calc1(d->ix), d->ix, 
	((d->flags&0x8)>>3), ((d->flags&0x4)>>2), ((d->flags&0x2)>>1), (d->flags&0x1), 
	(d->in)->flag, (d->in)->bits, calc1((d->in)->bits), (d->in)->bits,
	(d->out).flag, (d->out).bits, calc1((d->out).bits), (d->out).bits);
}

void output_memory_dump_line(const data *d, int a) {
	int i;
	printf("|%3d:", a);
	if(a < 256) {
		for(i=a; i<a+8; i++) {
			printf(" %.2x", d->program_memory[i]);
		}
		printf("\n");
	} else {
		for(i=a-256; i<a-248; i++) {
			printf(" %.2x", d->data_memory[i]);
		}
		printf("\n");
	}

}

void output_memory_dump(const data *d, int a) {
	output_memory_dump_line(d, a&0xFF0);
	output_memory_dump_line(d, (a&0xFF0)+8);
}

void output_memory_at_all_adress(const data *d) {
	int i;
	for(i=0; i<512; i+=16) {
		output_memory_dump(d, i);
	}
}

void output_result(const data *d) {
	printf("%.2x ", d->obj_code[0]);
	if(d->code_size == 2) {
		printf("%.2x ", d->obj_code[1]);
	}
	printf("%s [CPU: %.2x PC: %.2x ACC: %.2x (%d, %d) IX: %.2x (%d, %d)\nCF: %.2x VF: %.2x NF: %.2x ZF: %.2x\nIN: %d:%.2x (%d, %d) OUT: %d:%.2x (%d, %d)]\n ", d->mnemonic_code, d->cpuid, d->pc, d->acc, calc1(d->acc), d->acc, d->ix, calc1(d->ix), d->ix, ((d->flags&0x8)>>3), ((d->flags&0x4)>>2), ((d->flags&0x2)>>1), (d->flags&0x1), (d->in)->flag, (d->in)->bits, calc1((d->in)->bits), (d->in)->bits, (d->out).flag, (d->out).bits, calc1((d->out).bits), (d->out).bits);
	if(d->memory_changed == 0) {
		printf("]\n");
	} else if(d->memory_changed == 1) {
		printf("%.2x: %.2x -> %.2x]\n", d->program_memory[(int)d->modified_addr], d->prev, d->now);
	} else {
		printf("%.2x: %.2x -> %.2x]\n", d->data_memory[(int)d->modified_addr], d->prev, d->now);
	}
}
