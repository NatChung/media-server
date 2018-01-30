all:
	gcc main.c udp_socket.c encode.c decode.c audio.c ../librtp/debug.linux/librtp.a ../libmpeg/debug.linux/libmpeg.a -lz -I../libmpeg/include -I../librtp/include

clean:
	rm a.out
 