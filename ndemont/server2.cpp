#include "socket.hpp"
#include "listenSocket.hpp"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

# define BUFFER 4096

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
	int							delai = -1;
	std::vector<struct pollfd>	pollfd;
	int							ret;
	int							end_server = 0;
	int							new_sd = -1;
	int							close_conn, compress_array = 0;
	char						buffer[80];

	int status = fcntl(listenSocket.server_fd, F_SETFL, fcntl(listenSocket.server_fd, F_GETFL, 0) | O_NONBLOCK);
	if (status < 0)
	{
		perror("In fcntl: ");
		exit(EXIT_FAILURE);
	}
	listenSocket.listenning();
	pollfd.push_back(new_pollfd(listenSocket.server_fd));

	while (true)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		struct sockaddr_in cli_addr;
		socklen_t	addrlen = sizeof(cli_addr);
		new_sd = accept(listenSocket.server_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&addrlen);
		if (new_sd <= 0)
		{
			if (new_sd < 0)// && errno != EWOULDBLOCK)
			{
				perror("Accept error");
				//continue ;
			}
			std::cout << "No accept in the loop." << std::endl;
		}
		pollfd.push_back(new_pollfd(new_sd));
		for (std::vector<struct pollfd>::iterator it = pollfd.begin(); it != pollfd.end(); it++)
		{
			(*it).events = POLLIN;
		}

		/*
		if(inWritePending){
   fds[1].events = POLLOUT;
   fds[0].events = 0;
  }

  if(outWritePending){
   fds[0].events = POLLOUT;
   if(!inWritePending){
    fds[1].events = 0;
   }
  }*/

		ret = poll(&(*pollfd.begin()), pollfd.size(), delai);
		if (ret > 0)
		{

		}
	}

}