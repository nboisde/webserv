#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 10

int main(void)
{
	int sockfd, new_fd;
	struct sockaddr_in serv_addr; // addr of the server.
	struct sockaddr_in cli_addr;
	int rd;
	int sin_size;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Error on socket\n");
		exit(1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr("10.11.9.6");
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("Error on binding socket\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("Error on listening\n");
		exit(1);
	}
// READ RFC7230
	while (1)
	{
		sin_size = sizeof(struct socklen_t *);
		//sin_size = sizeof(struct sockaddr);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&sin_size)) < 0)
		{
			perror("Error on accept\n");
			exit(1);
		}
		char *client_ip = inet_ntoa(cli_addr.sin_addr);
		printf("ip address of the client: %s\n", client_ip);
		printf("Cli addr: %i %u %u %s\n", cli_addr.sin_family, cli_addr.sin_port, cli_addr.sin_addr.s_addr, cli_addr.sin_zero);
		printf("Serv addr: %i %u %u %s\n", serv_addr.sin_family, serv_addr.sin_port, serv_addr.sin_addr.s_addr, serv_addr.sin_zero);
		printf("CLIENT SOCKET FILE DES: %i\n", new_fd);
		printf("SERVER SOCKET FILE DES: %i\n", sockfd);
		char buffer[30000] = {0};
		rd = read(new_fd, buffer, 30000);
		//rd = recv(new_fd, buffer, 30000, MSG_WAITALL);
		if (rd == -1)
			perror("Error on recv\n");
		write(1, buffer, 30000);
		//if (send(new_fd, hello, strlen(hello), MSG_OOB) == -1)
		//if (write(new_fd, hello, strlen(hello)) == -1)
		if (write(new_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 76\r\n\r\n<!DOCTYPE html><html><body><h1>First display on localhost</h1></body></html>", strlen("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 76\r\n\r\n<!DOCTYPE html><html><body><h1>First display on localhost</h1></body></html>")) == -1)
			perror("Error on send\n");
		close(new_fd);
	}
}
