#ifndef VIDEO_INCLUDE 
#define VIDEO_INCLUDE

#define T Video_T
typedef struct T*  T;

extern T    Video_new(void);
extern int  Video_open(T video,const char *path);
extern int Video_free(T *video);
extern int  Video_set_fmt(T video,unsigned int width,unsigned int height);
extern int  Video_getdeviceinfo(T video);
extern int  Video_start_capture(T video);
extern int  Video_stop_capture(T video);
extern int  Video_capture(T video);
extern int  Video_request_buf(T video,unsigned int num_buf);
extern unsigned char* Video_getbuf(T video);
extern unsigned int Video_getwidth(T video);
extern unsigned int Video_getheight(T video);

#undef T
#endif
