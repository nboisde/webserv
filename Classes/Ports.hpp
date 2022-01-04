#ifndef __PORTS_HPP__
# define __PORTS_HPP__

# include <iostream>
# include <string>

class Port :
{

	public:

		Port( void );
		Port( int nb );
		Port( Port const & src );
		virtual ~Port( void );

		Port &		operator=( Port const & rhs );

		int		getPort( void ) const;

	protected:
		int		_port_nb;
};

std::ostream &			operator<<( std::ostream & o, Port const & i );

#endif /* ****************************************************** PORTSCLASS_H */