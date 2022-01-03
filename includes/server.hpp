#ifndef SERVER_HPP
# define SERVER_HPP

# define BUFFER 4096

# include "listenSocket.hpp"
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>

int	launch_server(void);


#endif