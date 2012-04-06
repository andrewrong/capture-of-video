#ifndef TIME_INCLUDE
#define TIME_INCLUDE

#define Time_T T
typedef struct T* T;

extern int getdistance(T time);
extern void Time_start(T time);
extern void Time_stop(T time);
extern void Time_output(T time);
extern T Time_new();
extern void Time_free(T *time);

#undef T

#endif
