#include <stdio.h>
#include <sys/time.h>
#include "m_mem.h"
#include "m_time.h"
#include <assert.h>

#define Time_T T

struct Time_T
{
    struct timeval begin;
    struct timeval end;
};

T Time_new()
{
    T time = MALLOC(sizeof(struct Time_T));
    assert(time);
    
    return time;
}

void Time_start(T time)
{
    assert(time);
    gettimeofday(&(time->begin),NULL);
}

void Time_stop(T time)
{
    assert(time);
    gettimeofday(&(time->end),NULL);
}

void Time_output(T time)
{
    assert(time);
    printf("second is %ld,msecond is %ld\n",time->end.tv_sec - time->begin.tv_sec,time->end.tv_usec - time->begin.tv_usec);
}

int getdistance(T time)
{
    return time->end.tv_usec - time->begin.tv_usec;
}
void Time_free(T *time)
{
    assert(time && *time);
    FREE(*time);
}
