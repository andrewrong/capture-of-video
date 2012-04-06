#include "video.h"
#include "m_mem.h"
#include "m_error.h"
#include "show.h"
#include "processimage.h"
#include "m_time.h"
#include <sys/time.h>

#include <stdio.h>

#define W 640 
#define H 480
long sum1 = 0;
void m_sum(int elem)
{
    if(elem < 0)
    {
	elem = 1000000 + (elem);
    }

    sum1 += elem;
}

int main()
{
    Show_T show;
    Video_T video;
    int i = 100;
    Time_T  yuvtime;
     
    mem_in_id = -1;
    is_check = 1;

    unsigned char *rgb888;
    unsigned char *rgb565;
    //unsigned int width = 0,heigth = 0;
    
    mem_check_init();
    show = Show_new();
    if(Show_open(show,"/dev/fb0") == -1)
    {
	return -1;
    }

    Show_getdeviceinfo(show);
    video = Video_new();

    if(Video_open(video,"/dev/video3") == -1)
    {
	return -1;
    }

    Video_getdeviceinfo(video);

    if(Video_set_fmt(video,W,H) == -1)
    {
	return -1;
    }

    if(Video_request_buf(video,4) == -1)
    {
	return -1;
    }

    if(Video_start_capture(video) == -1)
    {
	return -1;
    }

    //rgb888 = MALLOC(Video_getwidth(video) * Video_getheight(video) * 3);
    rgb565 = MALLOC(Video_getwidth(video) * Video_getheight(video) * 2);
    
    printf("width:%d,heigth:%d\n",Video_getwidth(video),Video_getheight(video));
    
    //process_init();

    yuvtime = Time_new();
    while(i--)
    {
	unsigned char* yuv = NULL;
	Video_capture(video);
	yuv = Video_getbuf(video);

	Time_start(yuvtime);
	v4l_yuv422p2rgb(rgb565,yuv,W,H,16);	
	Show_display(show,rgb565,W,H);
	Time_stop(yuvtime);

	m_sum(getdistance(yuvtime));
	Time_output(yuvtime);
    }
     
    printf("average is %ld\n",(long)(sum1 / 100));
    Time_free(&yuvtime);
    Video_stop_capture(video);
    Video_free(&video);
    Show_free(&show);
    FREE(rgb565);
    FREE(rgb888);
    
    mem_check_write();
    return 0;
}
