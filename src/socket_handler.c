#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <assert.h>

int connect_tcp_server_socket(unsigned short port){

	int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
	assert(sockfd > 0);

    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(port);

    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
	assert(err != -1);

    return sockfd;
}

int open_udp_server_socket(int *local_port)
{
	struct sockaddr_in my_addr ;
	int udp_fd = -1, tmp;
	
	udp_fd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(udp_fd > 0);

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(*local_port);
	assert(bind(udp_fd,(struct sockaddr *)&my_addr, sizeof(my_addr)) != -1) ;

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(udp_fd, (struct sockaddr *)&sin, &len) != -1)
    	*local_port = ntohs(sin.sin_port);

	return udp_fd ;
}