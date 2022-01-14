#include <fstream>
#include <iostream>
#include <map>



int main(void)
{
	std::map<std::string, std::string> _dict;
	std::ifstream file("dictConf.default");
	std::string line;
	while (std::getline(file, line))
	{
		//std::cout << line << std::endl;
		int i = 0;
		while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\v')
			i++;
		if (line[i] == '#')
			continue ;
		else if (!line[i] || line[i] == '\n')
			continue ;
		else
		{
			int dbl = line.find(":");
			if (dbl == -1)
				return (1);
			std::string key = line.substr(i, dbl - i);
			std::string value = line.substr(dbl + 1, line.length());
			while (key.find(" ") != -1 || key.find("\t") != -1)
				key = key.substr(0, key.length() - 1);
			int k = 0;
			while (value[k] == ' ' || value[k] == '\t' || value[k] == '\r' || value[k] == '\v')
				k++;
			value = value.substr(k, value.length() - k);
			while (value.find(" ") != -1 || value.find("\t") != -1)
				value = value.substr(0, value.length() - 1);
			_dict[key] = value;
		}
	}
	for (std::map<std::string, std::string>::iterator it = _dict.begin(); it != _dict.end(); it++)
			std::cout << (*it).first << ", " << (*it).second << std::endl;
}