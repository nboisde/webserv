#include <string>
#include <iostream>
#include <functional>
#include <dirent.h>
#include <vector>

std::vector<std::string> v;

void listFiles(std::string path) {
    if (DIR *dir = opendir(path.c_str())) {
        while (struct dirent *f = readdir(dir)) {
            if (/*!(f->d_name) || */f->d_name[0] == '.') continue;
            if (f->d_type == DT_DIR)
                listFiles(path + f->d_name + "/");

            if (f->d_type == DT_REG)
			{
				std::string p = path + f->d_name;
				//if (static_cast<int>(p.find(root)) != -1)
				//{
					//v.push_back(p.substr(p.find_last_of("/")));
					v.push_back(p.substr(p.find_first_of("/")));
				//}
			}
        }
        closedir(dir);
    }
}

std::string generateAutoindexHTML(void)
{
	std::string html = "<!DOCTYPE html><html><head><title>autoindex</title></head><body><ul>";
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
	{
		std::string p = "";
		p += "<li><a href=\"";
		p += (*it);
		p += "\">";
		p += (*it);
		p += "</a></li>";
		html += p;
	}
	html += "</ul></body></html>";
	std::cout << html << std::endl;	
	return html;
}

// int main(void)
// {
// 	listFiles("/mnt/nfs/homes/nboisde/sgoinfre/webserv/www/php/");
// 	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
// 		std::cout << (*it) << std::endl;
// 	generateAutoindexHTML();
// }
