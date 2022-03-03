# include "Listdir.hpp"

listdir::listdir(){}
listdir::~listdir(){}

std::string		listdir::generateAutoindex(std::string path, std::string loc)
{
	listFiles(path, loc);
	std::string autoindex = generateHTML();
	_dirs.clear();
	return (autoindex);
}

void		listdir::listFiles(std::string path, std::string loc){
	if (DIR *dir = opendir(path.c_str()))
	{
		while (struct dirent *f = readdir(dir))
		{
			if (f->d_name[0] == '.')
				continue ;
			if (f->d_type == DT_DIR)
			{
				_dirs.push_back(loc + "/" + f->d_name);
				listFiles(path + "/" + f->d_name, loc);
			}
			if (f->d_type == DT_REG)
			{
				std::string p = path + "/" + f->d_name;
				_dirs.push_back(p.substr(p.find(loc)));
			}
		}
		closedir(dir);
	}
}


std::string		listdir::resolvePath( std::string path )
{
	std::string new_path = path;
	return new_path;
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
