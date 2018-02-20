
all:
	gcc main.c udp_socket.c rtp_handler.c hls_handler.c g711.c \
	../librtp/debug.linux/librtp.a \
	../libmpeg/debug.linux/libmpeg.a \
	-lz -lfaac \
	-I../libmpeg/include \
	-I../librtp/include

clean:
	rm -f a.out
 