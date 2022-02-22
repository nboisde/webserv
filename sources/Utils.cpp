#include "webserv.hpp"
#include "Utils.hpp"

std::string strToLower(std::string s1)
{
	std::string s2;
	for (std::string::iterator it = s1.begin(); it != s1.end(); it++)
	{
		s2 += std::tolower(*it);
	}
	return s2;
}

int strIsPrintable(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
	{
		if (!std::isprint(*it))
			return 0;
	}
	return 1;
}

bool	isfile(std::string const & path)
{
	struct stat buf;
	if ((stat(path.c_str(), &buf)) < 0)
	{
		perror("stat");
		return false;
	}
	else
	{
		if (!S_ISREG(buf.st_mode))
			return false;
		else
			return true;
	}
}