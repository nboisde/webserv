#include "Port.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Port::Port( void )
{
	return;
}

Port::Port( int nb ) : _port_nb(nb)
{
	return;
}

Port::Port( const Port & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Port::~Port( void )
{
	return;
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Port &				Port::operator=( Port const & rhs )
{
	if ( this != &rhs )
	{
		this->_port_nb = rhs.getPort();
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Port const & i )
{
	o << "Port_nb = " << i.getPort();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Port::getPort( void ) const{
	return this->_port_nb;
}

/* ************************************************************************** */