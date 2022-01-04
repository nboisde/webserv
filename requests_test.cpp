#include <cstring>
#include <iostream>
#include "includes/webserv.hpp"
#include <vector>

#define BUFFER_SIZE 4096
#define RC goo

int main(void)
{
	//std::string buf1 = "POST / HTTP/1.1\r\nContent-Type: application/json\r\ncache-control: no-cache\r\nPostman-Token: 47124fa0-d97e-4b6f-bfbe-a45c89fb6298\r\nUser-Agent: PostmanRuntime/7.1.1\r\nAccept: */*\r\nHost: localhost:8080\r\naccept-encoding: gzip, deflate\r\ncontent-length: 56\r\nConnection: keep-alive\r\n\r\n{\r\n\t\"email\": \"test@example.com\",\r\n\t\"password\":\"azery\"\r\n}";
	std::string goo = "GET /favicon.ico HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: keep-alive\r\nsec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"96\", \"Google Chrome\";v=\"96\"\r\nDNT: 1\r\nsec-ch-ua-mobile: ?0\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\nsec-ch-ua-platform: \"macOS\"\r\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\nSec-Fetch-Site: same-origin\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Dest: image\r\nReferer: http://127.0.0.1:8080/\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n";
	
	std::cout << RC << std::endl;

	std::vector<std::string> v;
	size_t i = 0;
	while (i < RC.length())
	{
		int j = 0;
		std::string s;
		while (j < BUFFER_SIZE && RC[i + j])
		{
			s += RC[i + j];
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
	std::cout << "RAW_INFORMATIONS: " << std::endl << "----------------------------------------" << std::endl;
	std::cout << " - state: " << r.requestReceptionState() << std::endl;
	std::cout << " - raw content: " << std::endl << r.getRawContent() << std::endl;
	std::cout << " - Method (1-get, 2-post, 3-del)"<< r.getMethodType() << std::endl;
	std::cout << " - content length: " << r.getContentLength() << std::endl;
	std::cout << "////////////////////////////////////////////////////////" << std::endl;

	std::cout << "HEADER AND BODY: " << std::endl << "----------------------------------------" << std::endl;
	r.fillHeaderAndBody();
	std::cout << " - header : " << std::endl << r.getHeader() << std::endl;
	std::cout << "--------------------" << std::endl;
	std::cout << " - body : " << std::endl << r.getBody() << std::endl;
	std::cout << goo.length() << std::endl;
}
