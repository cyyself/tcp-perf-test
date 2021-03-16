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

char* newstr(const char *src) {
	int len = strlen(src);
	char *res = (char*)malloc(len+1);
	memcpy(res,src,len+1);
	return res;
}

int socket_fd;
int online_client = 0;

int init_fd(const int listen_port,const int backlog) {
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if (socket_fd < 0) return socket_fd;
	int error;
	const int one = 1;
	error = setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(int));
	if (error) return error;
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(listen_port);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	error = bind(socket_fd,(struct sockaddr* )&server_sockaddr,sizeof(server_sockaddr));
	if (error == -1) return error;
	error = listen(socket_fd,backlog);
	if (error == -1) return error;
	return 0;
}

void client_handler(const int client_fd) {
	const int buf_sz = 2048;
	char buf[buf_sz];
	int sz;
	while ((sz = recv(client_fd,buf,buf_sz,0)) > 0) {
		if (send(client_fd,buf,sz,0) < 0) break;
	}
	close(client_fd);
	online_client --;
	printf("[INFO] Client leave, count = %d\n",online_client);
}

void accept_client() {
	struct sockaddr_in clientaddr,myaddr,destaddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int client_fd = accept(socket_fd,(struct sockaddr*)&clientaddr,&addrlen);
	if (client_fd == -1) {
		printf("[ERROR] accept return %d:",client_fd);
		perror("");
		printf("\n");
		close(socket_fd);
		exit(1);
		return;
	}
	char *clientip = newstr(inet_ntoa(clientaddr.sin_addr));
	online_client ++;
	printf("[INFO] Client from %s:%d, count = %d\n",clientip,ntohs(clientaddr.sin_port),online_client);
	std::thread t(client_handler,client_fd);
	t.detach();
}

int main() {
	int listen_port = 12345;
	if (init_fd(listen_port,1024)) {
		perror("[ERROR] listen failed:");
		return -1;
	}
	printf("[INFO] Server listening on %d\n",listen_port);
	while (1) accept_client();
	return 0;
}
