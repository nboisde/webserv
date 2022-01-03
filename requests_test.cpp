#include <cstring>
#include <iostream>
#include "requests.hpp"
#include <vector>

#define BUFFER_SIZE 1

int main(void)
{
	std::string buf1 = "POST / HTTP/1.1\r\nContent-Type: application/json\r\ncache-control: no-cache\r\nPostman-Token: 47124fa0-d97e-4b6f-bfbe-a45c89fb6298\r\nUser-Agent: PostmanRuntime/7.1.1\r\nAccept: */*\r\nHost: localhost:8080\r\naccept-encoding: gzip, deflate\r\ncontent-length: 56\r\nConnection: keep-alive\r\n\r\n{\r\n\t\"email\": \"test@example.com\",\r\n\t\"password\":\"azery\"\r\n}";
	std::cout << buf1 << std::endl;

	std::vector<std::string> v;
	size_t i = 0;
	while (i < buf1.length())
	{
		int j = 0;
		std::string s;
		while (j < BUFFER_SIZE && buf1[i + j])
		{
			s += buf1[i + j];
			j++;
		}
		v.push_back(s);
		i+= BUFFER_SIZE;
	}
/* 	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
	{
		std::cout << *it << std::endl << std::endl;
	} */
	ws::Requests r;
	std::vector<std::string>::iterator it = v.begin();
	while (it != v.end() && r.concatenateRequest(*it) == 0)
	{
		//if (!r.concatenateRequest(*it))
		//	std::cout << "concatenating..." << std::endl;
		//if (r.concatenateRequest(*it) == 1)
		//	break;
		it++;
	}
	std::cout << "state: " << r.requestReceptionState() << std::endl;
	std::cout << "raw: " << std::endl << r.getRawContent() << std::endl;
	std::cout << "Method (1-get, 2-post, 3-del)"<< r.getMethodType() << std::endl;
	std::cout << "content length: " << r.getContentLength() << std::endl;

	//r.fillHeaderAndBody();
	std::cout << r.getHeader() << std::endl;
	std::cout << "--------------------" << std::endl;
	std::cout << r.getBody() << std::endl;
}