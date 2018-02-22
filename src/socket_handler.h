#ifndef _SOCKET_HANDLER_H
#define _SOCKET_HANDLER_H

int open_udp_server_socket(int *local_port);
int connect_tcp_server_socket(unsigned short port);

#endif