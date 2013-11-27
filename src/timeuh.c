#include "timeuh.h"

struct timespec time_diff(const time_counter *tc){
    struct timespec tp;
    tp.tv_sec = tc->end.tv_sec - tc->begin.tv_sec;
    tp.tv_nsec = tc->end.tv_nsec - tc->begin.tv_nsec;
    if(tp.tv_nsec<0){
        tp.tv_sec -= 1;
        tp.tv_nsec += 1000000000;
    }
    return tp;
}
