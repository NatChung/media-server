#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>

int open_udp_server_socket(int local_port)
{
	struct sockaddr_in my_addr ;
	int udp_fd = -1, tmp;
	
	udp_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (udp_fd < 0)
	{
		printf("open socket fail!\n");
		goto fail;
	}
	
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(local_port);
    //printf("\n\n open udp port is %d\n\n",htons(local_port));
	if (bind(udp_fd,(struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) 
	{
		printf("bind %d fail\n", local_port);
		goto fail;
	}

// 	int buff_size;
// 	int size = sizeof(buff_size);

// 	getsockopt(udp_fd,SOL_SOCKET,SO_SNDBUF,(char*)&size,sizeof(size));
	
// 	/* limit the tx buf size to limit latency */
// #define UDP_TX_BUF_SIZE 85600
// 	tmp = UDP_TX_BUF_SIZE;
// 	if (setsockopt(udp_fd, SOL_SOCKET, SO_SNDBUF, &tmp, sizeof(tmp)) < 0) {
// 		goto fail;
// 	}
	
	return udp_fd ;
	
fail:
	if (udp_fd >= 0)
		close(udp_fd);
	return -1 ;
}