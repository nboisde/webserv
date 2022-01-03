#include "PollClass.hpp"

struct pollfd &Poll::new_pollfd(int socketfd) //PROTEGER MEMOIRE
{
	struct pollfd *ptr = new struct pollfd;
	ptr->fd = socketfd;
	ptr->events = POLLIN;
	return (*ptr);
}