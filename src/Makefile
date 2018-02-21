
all:
	gcc main.c udp_socket.c rtp_handler.c hls_handler.c g711.c \
	../libhls/debug.linux/libhls.a \
	../librtp/debug.linux/librtp.a \
	../libmpeg/debug.linux/libmpeg.a \
	-I../libhls/include \
	-I../libmpeg/include \
	-I../librtp/include \
	-lz -lfaac 

clean:
	rm -f a.out
 