int	Parser::checkLocationKeys(Value & loc_config, keys_type & new_config)
{
	while (_pos < _size && isspace(_content[_pos]))
		_pos++;
	std::pair<std::string, route> new_path(path, route());
	loc_config._locations.insert(new_path);

	std::vector<std::string>::iterator it = loc_config._locations[path].keys.begin();
	std::vector<std::string>::iterator ite = loc_config._locations[path].keys.end();
	
	int	found = 0;
	for (; it != ite; it++)
	{
		std::cout << "KEY " << *it << std::endl;
		int i = _content.find(*it, _pos);
		std::cout << "POS " << _pos << std::endl;
		std::cout << "FIND " << i << std::endl;
		if (i == _pos)
		{
			found = 1;
			_pos += it->size();
			break;
		}
	}
	std::cout << "FOUND KEY " << *it << std::endl;
	return (0);
	if (!found)
		return (0);
	if (!setValues(*it, new_config, &(loc_config._locations[path]), 1))
		return (0);
	return (SUCCESS);
}