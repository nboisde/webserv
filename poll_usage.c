#include <stdio.h>
#include <stdlib.h>
//#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 8080

#define TRUE 1
#define FALSE 0

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

int main(int ac, char **av)
{
	int len, rc, on = 1;
	int listen_sd = -1, new_sd = -1;
	int desc_ready, end_server = FALSE, compress_array = FALSE;
	int close_conn;
	char buffer[50000];
	struct sockaddr_in6 addr;
	int timeout;
	struct pollfd fds[200];
	int nfds = 1, current_size = 0, i, j;

	listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket failed: ");
		exit(-1);
	}

	rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt failed: ");
		close(listen_sd);
		exit(-1);
	}

	int flags = fcntl(listen_sd, F_GETFD);
	if (flags < 0)
	{
		perror("get flags: ");
		close(listen_sd);
		exit(-1);
	}
	rc = fcntl(listen_sd, F_SETFL, flags | O_NONBLOCK);
	if (rc < 0)
	{
		perror("Non blocking: ");
		close(listen_sd);
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind listen: ");
		close(listen_sd);
		exit(-1);
	}

	rc = listen(listen_sd, 32);
	if (rc < 0)
	{
		perror("listen failed: ");
		close(listen_sd);
		exit(-1);
	}

	memset(fds, 0, sizeof(fds));

	fds[0].fd = listen_sd;
	fds[0].events = POLLIN;

	timeout = (3 * 60 * 1000);

	do
	{
		printf("waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);

		if (rc < 0)
		{
			perror("poll failed: ");
			break ;
		}

		if (rc == 0)
		{
			printf("poll timed out. End Program.\n");
			break ;
		}

		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			if (fds[i].revents == 0)
				continue;
			if (fds[i].revents != POLLIN)
			{
				printf("Error ! revents %d\n", fds[i].revents);
				end_server = TRUE;
				break ;
			}
			if (fds[i].fd == listen_sd)
			{
				printf("Listening socket is readdable\n");
				do {
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("accept failed: ");
							end_server = TRUE;
						}
						break ;
					}
				} while (new_sd != -1);
			}
			else
			{
				printf("Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;
				do {
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("accept failed: ");
							end_server = TRUE;
						}
						break ;
					}
					if (rc == 0)
					{
						printf("Connection closed\n");
						close_conn = TRUE;
						break ;
					}
					len = rc;
					printf("%d bytes recieved\n", len);

					rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("send failed: ");
						close_conn = TRUE;
						break ;
					}
				} while (TRUE);
				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}
		}
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for (j = i; j < nfds - 1; j++)
					{
						fds[j].fd = fds[j + 1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	} while (end_server == FALSE);


	for (i = 0; i < nfds; i++)
	{
		if (fds[i].fd >= 0)
			close(fds[i].fd);
	}
}