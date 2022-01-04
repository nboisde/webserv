#ifndef __PORTS_HPP__
# define __PORTS_HPP__

# include "webserv.hpp"
# include <iostream>
# include <string>

namespace ws{
class Port
{

	public:

		Port( int nb );
		Port( Port const & src );
		virtual ~Port( void );

		Port &		operator=( Port const & rhs );

		int		getPort( void ) const;

	protected:
		Port( void );
		int		_port_nb;
};
}

std::ostream &			operator<<( std::ostream & o, Port const & i );

#endif /* ****************************************************** PORTSCLASS_H */