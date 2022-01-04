#include "webserv.hpp"

struct pollfd &new_pollfd(int socketfd) //PROTEGER MEMOIRE
{
	struct pollfd *ptr = new struct pollfd;
	ptr->fd = socketfd;
	ptr->events = POLLIN;
	return (*ptr);
}

int	launch_server()
{
	ws::listenSocket			listenSocket;
	char						*hello = (char *)("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8;\r\nContent-Length: 21\r\n\r\n<h1>Hello world!</h1>\n");
	std::vector<struct pollfd>	pollfd;
	int							ret;
	int							new_sd = -1;
	int							close_conn, compress_array = 0;
	char						buffer[BUFFER_SIZE];

	if (!listenSocket.bindSocket())
		return (0);
	int status = fcntl(listenSocket.server_fd, F_SETFL, fcntl(listenSocket.server_fd, F_GETFL, 0) | O_NONBLOCK);
	if (status < 0)
	{
		perror("In fcntl: ");
		exit(EXIT_FAILURE);
	}
	listenSocket.listenning();
	pollfd.push_back(new_pollfd(listenSocket.server_fd));
	do
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		ret = poll(&(*pollfd.begin()), pollfd.size(), DELAI);
		if (ret < 0)
		{
			perror("In poll: failes");
			break;
		}
		if (ret == 0)
		{
			perror("In poll: timed out");
			break;
		}
		for (int i = 0; i < (int)pollfd.size(); i++)
		{
			if (pollfd[i].revents == 0)
				continue;
			if (pollfd[i].revents != POLLIN)
			{
				printf("  Error! revents = %d\n", pollfd[i].revents);
				close(pollfd[i].fd);
				pollfd[i].fd = -1;
				compress_array = 1;
				break;
			}
			if (pollfd[i].fd == listenSocket.server_fd)
			{
				printf("  Listening socket is readable\n");
				do 
				{
					struct sockaddr_in cli_addr;
					socklen_t	addrlen = sizeof(cli_addr);

					new_sd = accept(listenSocket.server_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen);
					if (new_sd < 0)
						break;
					printf("  New incoming connection - %d\n", new_sd);
					pollfd.push_back(new_pollfd(new_sd));
				} while (new_sd != -1);
			}
			else
			{
				printf("  Descriptor %d is readable\n", pollfd[i].fd);
				close_conn = 0;
				do
				{
					ret = recv(pollfd[i].fd, buffer, sizeof(buffer), 0);
					if (ret < 0)
					{
						perror("\nIn recv");
						break;
					}
					if (ret == 0)
					{
						std::cout << "Connection closed\n";
						close_conn = 1;
						break;
					}
					int len = ret;
					printf("  %d bytes received\n", len);
					for (size_t i = 0; i < 80; i++)
						buffer[i] = 0;
					ret = send(pollfd[i].fd, hello, strlen(hello), 0);
					if (ret < 0)
					{
						perror("  send() failed");
						close_conn = 1;
						break;
					}

				} while (1);
				if (close_conn)
				{
					close(pollfd[i].fd);
					pollfd[i].fd = -1;
					compress_array = 1;
				}
			}
		}
		if (compress_array)
		{
			for (int i = 0; i < (int)pollfd.size(); i++)
			{
				if (pollfd[i].fd == -1)
					pollfd.erase(pollfd.begin() + i);
			}
		}
	} while (1);
	for (int i = 0; i <(int)pollfd.size(); i++)
	{
		if(pollfd[i].fd >= 0)
			close(pollfd[i].fd);
	}
	return (0);
}