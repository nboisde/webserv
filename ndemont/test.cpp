#include "test.hpp"

void	init_sockaddr_in(struct sockaddr_in *address)
{
	const int			PORT = 8080;

	address->sin_family = AF_INET; 
	address->sin_addr.s_addr = htonl(INADDR_ANY); 
	address->sin_port = htons(PORT); 
	//address->sin_len = 0;
	//address->sin_zero;
}

int	main()
{
	int					server_fd;
	struct sockaddr_in	address;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Cannot create socket\n");
		return (1);
	}
	std::cout << "Server fd = " << server_fd << std::endl;
	memset((char *)&address, 0, sizeof(address));
	init_sockaddr_in(&address);
	if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0) 
	{
		perror("Bind failed\n"); 
		return (0); 
	}
	return (0);
}