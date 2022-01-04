#ifndef __PORTS_HPP__
# define __PORTS_HPP__

# include <iostream>
# include <string>

class Ports
{

	public:

		Ports();
		Ports( Ports const & src );
		~Ports();

		Ports &		operator=( Ports const & rhs );

	private:
		int		port_nbr;
		Poll	poll_fct;
};

std::ostream &			operator<<( std::ostream & o, Ports const & i );

#endif /* ****************************************************** PORTSCLASS_H */