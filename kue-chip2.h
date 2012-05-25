#pragma once
#include <data.h>
#include <stdio.h>
int non_interactive(const io_data *d,const size_t step,const char *out_to);
int interactive(const io_data *d,const size_t step,const char *out_to);
