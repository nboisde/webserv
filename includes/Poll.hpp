
#ifndef __POLLCLASS_HPP__
# define __POLLCLASS_HPP__

#include "webserv.hpp"

namespace ws{
class Poll
{
	public:
		Poll(void);
		virtual ~Poll(void);
		Poll(Poll const & src);

		Poll & operator=(Poll const & rhs);

		struct pollfd & new_pollfd(int socketfd);

	private:
		std::vector<struct pollfd>	_pollfds;
};
};

#endif