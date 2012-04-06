#ifndef ERROR_INCLUDE
#define ERROR_INCLUDE

#define print_error(err) \
    fprintf(stderr,"%s,errno is %d [info:%s]\n",err,errno,strerror(errno));\
    fprintf(stderr,"File:%s,line:%d,function:%s\n",__FILE__,__LINE__,__func__);\
    return -1;

#endif
