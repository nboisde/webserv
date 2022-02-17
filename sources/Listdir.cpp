# include "Listdir.hpp"

listdir::listdir(){}
listdir::~listdir(){}

std::string		listdir::genetateAutoindex(std::string root, std::string path)
{
	listFiles(root, path);
	std::string autoindex = generateHTML();	
	_dirs.clear();
	return (autoindex);
}

void		listdir::listFiles(std::string root, std::string path){
		if (DIR *dir = opendir(path.c_str()))
	{
		while (struct dirent *f = readdir(dir))
		{
			if (f->d_name[0] == '.')
				continue ;
			if (f->d_type == DT_DIR)
				listFiles(path + f->d_name + "/", root);
			if (f->d_type == DT_REG)
			{
				std::string p = path + f->d_name;
				if (static_cast<int>(p.find(root)) != -1)
					_dirs.push_back(p.substr(p.find_first_of("/")));
			}
		}
		closedir(dir);
	}
}

std::string		listdir::generateHTML( void )
{
	std::string html = "<!DOCTYPE html><html><head><title>autoindex</title></head><body><ul>";
	for (std::vector<std::string>::iterator it = _dirs.begin(); it != _dirs.end(); it++)
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
	return html;
}
