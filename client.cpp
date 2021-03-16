#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <string>

int main() {
	int socket_fd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(12345);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	clock_t start_time = clock();
	if (connect(socket_fd,(sockaddr*)&server_addr,sizeof(struct sockaddr_in))) {
		printf("ERROR\n");
	}
	else {
		clock_t time = clock() - start_time;
		printf("%lu\n",time);
		fflush(stdout);
		sleep(10);
	}
	return 0;
}