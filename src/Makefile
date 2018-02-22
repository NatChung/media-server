
all:
	gcc main.c socket_handler.c rtp_handler.c hls_handler.c g711.c circular_queue.c \
	../libhls/debug.linux/libhls.a \
	../librtp/debug.linux/librtp.a \
	../libmpeg/debug.linux/libmpeg.a \
	-I../libhls/include \
	-I../libmpeg/include \
	-I../librtp/include \
	-lz -lfaac \
	-o mediaserver

clean:
	rm -f mediaserver
 