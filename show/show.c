#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "show.h"
#include "m_mem.h"
#include "m_error.h"


#define T Show_T

struct T
{
    unsigned char *addr;
    size_t width;
    size_t height;
    size_t bpp;
    size_t scrsize;
    int fd;
    unsigned char** lineptr;
};

T Show_new(void)
{
    T show = (T)MALLOC(sizeof(struct T));

    assert(show);

    show->addr = NULL;
    show->width = 0;
    show->height = 0;
    show->bpp = 0;
    show->scrsize = 0;
    show->fd = -1;
    show->lineptr = NULL;

    return show;
}

static int _Show_getdeviceinfo(T show)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    assert(show);    

    if(ioctl(show->fd,FBIOGET_FSCREENINFO,&finfo) == -1)
    {
	print_error("get finfo is error!");
    }

    if(ioctl(show->fd,FBIOGET_VSCREENINFO,&vinfo) == -1)
    {
	print_error("get vinfo is error!");
    }

    show->width = vinfo.xres;
    show->height = vinfo.yres;
    show->bpp = vinfo.bits_per_pixel;
    show->scrsize = (show->width * show->height * (show->bpp >> 3));
    show->lineptr = MALLOC(sizeof(unsigned char*) * show->height); 

    assert(show->lineptr);

    return 0;
}


static void _Show_computelineaddr(T show)
{
    unsigned int i = 0;
    
    for(i = 0; i < show->height ; i++)
    {
	show->lineptr[i] = show->addr + (i * show->width * (show->bpp >> 3));
    }
}

void Show_getdeviceinfo(T show)
{
    assert(show && (show->fd != -1));
    
    printf("this device is width:%d,height:%d,bpp:%d,scrsize:%d\n",show->width,show->height,show->bpp,show->scrsize);
}

static int  _Show_mmap(T show)
{
    assert(show);

    if((show->addr = (unsigned char*)mmap(0,show->scrsize,PROT_READ | PROT_WRITE,MAP_SHARED,show->fd,0)) == NULL)
    {
	print_error("mmap is failure!");
    }
}

int Show_free(T *show)
{
    assert(show && *show);

    munmap((*show)->addr,(*show)->scrsize);
    close((*show)->fd);
    FREE((*show)->lineptr);
    FREE(*show);

    return 0;
}

void Show_display(T show,unsigned char *pdata,unsigned int width,unsigned int height)
{
    unsigned int realw = 0;
    unsigned int realh = 0;
    unsigned int i = 0;
    
    realw = (width > show->width) ? show->width : width; 
    realh = (height > show->height) ? show->height: height; 
    
    for(i = 0 ; i < realh; i++) 
    {
	memcpy(show->lineptr[i],pdata+(i*width*(show->bpp>>3)),realw*(show->bpp >> 3));
    }
}

int Show_open(T show,const char *path)
{
    assert(show && path);
    
    if((show->fd = open(path,O_RDWR)) == -1)    
    {
	print_error("open file is failure!")
    }

    _Show_getdeviceinfo(show);
    _Show_mmap(show);
    _Show_computelineaddr(show);
}
