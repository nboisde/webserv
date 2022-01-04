#include "webserv.hpp"

namespace ws{

struct pollfd &Poll::new_pollfd(int socketfd){
	struct pollfd *elem = new struct pollfd;
	elem->fd = socketfd;
	elem->events = POLLIN;
	return (*elem);
}
}