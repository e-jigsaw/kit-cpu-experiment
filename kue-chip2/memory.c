#include<memory.h>
#include<stdlib.h>
void destroy_mem(void *ptr){free(*(void **)ptr);}
