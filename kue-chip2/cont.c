#include "kue-chip2-private.h"
int cont(data *d,const unsigned char stop_addr){
    int status=SUCCEEDED;
    while(d->pc<stop_addr){
        status=interpret(d);
        switch(status){
            case FAILURE: return FAILURE;
            case HLT: return HLT;
        }
    }
    return status;
}
