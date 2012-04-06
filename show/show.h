#ifndef SHOW_INCLUDE 
#define SHOW_INCLUDE	

#define T   Show_T
typedef struct T*	T;

extern T    Show_new(void);
extern int  Show_open(T show,const char *path);
extern void Show_getdeviceinfo(T show);
extern void Show_display(T show,unsigned char *pdata,unsigned int width,unsigned int height);
extern int  Show_free(T *show);

#undef T
#endif
