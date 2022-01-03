#include <cstring>
#include <iostream>

int main(void)
{
	std::string l = "Content-Length";
	std::string str = "GET HTTPS/1.1\r\nContent-Length: 12342\r\nca va?\r\n\r\nbodyyyyyyyyyyyyy";
	int ret = str.find(l);
	std::cout << ret << std::endl;
	std::cout << str[ret + l.length() + 2] << std::endl;
	std::string cl;
	int i = 0;
	while (str[ret + l.length() + 2 + i] != '\r')
	{
		cl += str[ret + l.length() + 2 + i];
		i++;
	}
	std::cout << cl << std::endl;
	long ct_length = std::stol(cl, NULL, 10);
	std::cout << ct_length << std::endl;
	std::cout << ct_length - 1 << std::endl;
}