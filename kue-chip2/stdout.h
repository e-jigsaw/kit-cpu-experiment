#pragma once
#include <data.h>
#include "stdout.h"
#include <stdio.h>

void output_register_dump(const data *d);
void output_memory_dump_line(const data *d, int a, int f);
void output_memory_dump(const data *d, int a, int f);
void output_memory_at_all_adress(const data *d);
void output_result(const data *d);

