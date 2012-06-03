#include "string.h"
void tolower_str(char *str){for(char *ch=str;ch<str+strlen(str);ch++) (*ch)=tolower(*ch);}
void toupper_str(char *str){for(char *ch=str;ch<str+strlen(str);ch++) (*ch)=toupper(*ch);}
