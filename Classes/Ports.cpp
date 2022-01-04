#include "PortsClass.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

PortsClass::PortsClass()
{
}

PortsClass::PortsClass( const PortsClass & src )
{
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

PortsClass::~PortsClass()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

PortsClass &				PortsClass::operator=( PortsClass const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, PortsClass const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */