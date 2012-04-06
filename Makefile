SRC_FILES=main.c ./show/show.c ./collect/video.c  ./algorithm/processimage.c ./mem/m_mem.c ./time/m_time.c 
HEAD_FILES=-I./show/ -I./collect/ -I./algorithm/ -I./error/ -I./mem/ -I./time/
GCC=gcc
AGCC=arm-none-linux-gnueabi-gcc
OBJ_FILES=${patsubst %.c,%.o,${SRC_FILES}}

main:${OBJ_FILES}
	${AGCC} -O2  ${OBJ_FILES}  ${HEAD_FILES} -o main
%.o:%.c
	${AGCC}  -O2  -c $< -o $@ ${HEAD_FILES}



clean:
	rm main.o ./algorithm/processimage.o ./show/show.o ./collect/video.o ./mem/m_mem.o  ./time/m_time.o main
