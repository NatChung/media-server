#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>

int open_udp_server_socket(int *local_port)
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
	my_addr.sin_port = htons(*local_port);
	if (bind(udp_fd,(struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) 
	{
		printf("bind %d fail\n", *local_port);
		goto fail;
	}

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(udp_fd, (struct sockaddr *)&sin, &len) != -1)
    	*local_port = ntohs(sin.sin_port);

	
	return udp_fd ;
	
fail:
	if (udp_fd >= 0)
		close(udp_fd);
	return -1 ;
}