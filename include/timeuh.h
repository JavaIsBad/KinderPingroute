#ifndef __TIMEUH_H
#define __TIMEUH_H
#include <time.h>

typedef struct {
    struct timespec begin, end;
} time_counter;

static inline int get_begin_time(time_counter *tc, clockid_t clk_id){
    return clock_gettime(clk_id, &tc->begin);
}

static inline int get_end_time(time_counter *tc, clockid_t clk_id){
    return clock_gettime(clk_id, &tc->end);
}

struct timespec time_diff(struct timespec* begin, struct timespec* end);

#endif // __TIMEUH_H
