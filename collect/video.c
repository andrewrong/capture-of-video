#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <linux/videodev.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include "video.h"
#include "m_mem.h"
#include "m_error.h"

#define T Video_T
#define CLEAR(s)  memset(&(s),0,sizeof(s))

struct bufarray
{
    void *start;
    int length;
};

typedef struct bufarray bufarray;

struct Video_T
{
    int fd;
    bufarray *b_arr;
    size_t buf_num;
    unsigned int index;
    unsigned int width;
    unsigned int height;
    unsigned int scrsize;
    unsigned char *tmpbuf;
    struct v4l2_capability m_cap;
    struct v4l2_format m_fmt;
    struct v4l2_requestbuffers m_rb;
    struct v4l2_buffer m_buf;
    enum v4l2_buf_type m_type;
};

T Video_new(void)
{
    T video = (T)MALLOC(sizeof(struct Video_T));
    assert(video);
    
    video->fd = -1;
    video->b_arr = NULL;
    video->buf_num = 0;
    video->index = 0;
    video->width = 0;
    video->height = 0;
    video->tmpbuf = NULL;
    video->scrsize = 0;
    
    printf("create video is success!\n");
    return video;
}

int Video_open(T video,const char *path)
{
    assert(video && path);

    if((video->fd = open(path,O_RDONLY)) == -1)
    {
	print_error("open video file is failure!");
    }
    
    printf("open video is success!\n");
    return 0;
}

int Video_set_fmt(T video,unsigned int width,unsigned int height)
{
    assert(video && width >= 0 && height >= 0);  

    CLEAR(video->m_fmt);
    
    video->width = width;
    video->height = height;

    video->m_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video->m_fmt.fmt.pix.width = width;
    video->m_fmt.fmt.pix.height = height;
    video->m_fmt.fmt.pix.field = V4L2_FIELD_ANY;
    video->m_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    if(-1 == ioctl(video->fd,VIDIOC_S_FMT,&(video->m_fmt)))
    {
	print_error("fmt is not set");
    }
    
    //printf("width:%d,height:%d\n",video->m_fmt.fmt.pix.width,video->m_fmt.fmt.pix.height);

    video->scrsize = (video->width * video->height) << 1;
    printf("set fmt is success!\n");
    return 0;
}

int Video_getdeviceinfo(T video)
{

   assert(video);

   CLEAR(video->m_cap); 

   if(-1 == ioctl(video->fd,VIDIOC_QUERYCAP,&video->m_cap))
   {
       print_error("VIDIOC_QUERYCAP");
   }
    
   printf("driver:%s\n",video->m_cap.driver);
   printf("card:%s\n",video->m_cap.card);
   printf("bus_info:%s\n",video->m_cap.bus_info);
   printf("version:%d\n",video->m_cap.version);
   printf("capability:%x\n",video->m_cap.capabilities);

   printf("query capability success\n");
   return 0;
}
int Video_request_buf(T video,unsigned int num_buf)
{
    assert(video && num_buf > 0);

    CLEAR(video->m_rb);

    video->m_rb.count = num_buf;
    video->m_rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video->m_rb.memory = V4L2_MEMORY_MMAP;
    
    if(-1 == ioctl(video->fd,VIDIOC_REQBUFS,&video->m_rb))
    {
	print_error("request buf is failure!");
    }

    video->buf_num = video->m_rb.count;
    video->b_arr = (bufarray*)MALLOC(video->buf_num * sizeof(struct bufarray)); 
    assert(video->b_arr);

    printf("request buf is success\n");
    return 0;
}


static int  Video_mmap(T video)
{
    int i = 0;

    assert(video);
    
    for(i = 0; i < video->buf_num; i++)
    {
	CLEAR(video->m_buf);

	video->m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	video->m_buf.index = i;
	video->m_buf.memory = V4L2_MEMORY_MMAP;

	if(-1 == ioctl(video->fd,VIDIOC_QUERYBUF,&video->m_buf))
	{
	    print_error("query buf is failure!");
	}
	
	video->b_arr[i].length = video->m_buf.length;
	video->b_arr[i].start = mmap(0,video->m_buf.length,PROT_READ,MAP_SHARED,video->fd,video->m_buf.m.offset);
	
	//printf("length:%d\n",video->m_buf.length);
	if(MAP_FAILED == video->b_arr[i].start)
	{
	    print_error("mmap is failure!");
	}
    }

    video->tmpbuf = (unsigned char*)MALLOC((video->width * video->height) << 1);

    assert(video->tmpbuf);

    return 0;
}

static int Video_q_buf(T video)
{
    unsigned int i = 0;

    assert(video);

    for(i = 0; i < video->buf_num; i++)
    {
	CLEAR(video->m_buf);

	video->m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	video->m_buf.index = i;
	video->m_buf.memory = V4L2_MEMORY_MMAP;

	if(-1 == ioctl(video->fd,VIDIOC_QBUF,&video->m_buf))
	{
	    print_error("qbuf is failure!");
	}
    }

    printf("v4l2 init(qbuf) is success!\n");

    return 0;
}

int Video_start_capture(T video)
{
    assert(video);
    Video_mmap(video);
    Video_q_buf(video);

    CLEAR(video->m_type);
    video->m_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(video->fd,VIDIOC_STREAMON,&video->m_type))
    {
	print_error("start capture is failure!");
    }
    printf("streamon is success!\n");
    return 0;
}

int Video_capture(T video)
{
	assert(video);
	CLEAR(video->m_buf);

	video->m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	video->m_buf.memory = V4L2_MEMORY_MMAP;

	if(-1 == ioctl(video->fd,VIDIOC_DQBUF,&video->m_buf))
	{
	    print_error("DQBUF is failure!");
	}
	
	memcpy(video->tmpbuf,video->b_arr[video->m_buf.index].start,video->scrsize);
	
	/*注意这个count可能导致溢出的现象*/
	//msg_out("frame:%ld  ",count++);
	
	if(-1 == ioctl(video->fd,VIDIOC_QBUF,&video->m_buf))
	{
	    print_error("qbuf2 is failure!");
	}
	
	//printf("一帧\n");
	return 0;
}

int Video_stop_capture(T video)
{
    assert(video);
    CLEAR(video->m_type);
    video->m_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(video->fd,VIDIOC_STREAMOFF,&video->m_type))
    {
	print_error("stop capture is failure!");
    }
    
    printf("streamoff is success!\n");
    return 0;
}

unsigned char *Video_getbuf(T video)
{
    assert(video);
    return video->tmpbuf;
}

unsigned int Video_getwidth(T video)
{
    assert(video);
    return video->width;
}

unsigned int Video_getheight(T video)
{
    assert(video);
    return video->height;
}

int Video_free(T *video)
{
    int i = 0;

    for(i = 0; i < (*video)->buf_num; i++)
    {
	if(munmap((*video)->b_arr[i].start,(*video)->b_arr[i].length) == -1)
	{
	    print_error("munmap is failure!");
	}
    }
    
    close((*video)->fd);
    FREE((*video)->tmpbuf);
    FREE((*video)->b_arr);
    FREE(*video);
}
