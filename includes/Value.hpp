#ifndef VALUE_HPP
# define VALUE_HPP

# include <iostream>
# include <string>

class Value
{

	public:

		Value();
		Value( Value const & src );
		~Value();

		Value &		operator=( Value const & rhs );

	private:

};

std::ostream &			operator<<( std::ostream & o, Value const & i );

#endif /* *********************************************************** VALUE_H */