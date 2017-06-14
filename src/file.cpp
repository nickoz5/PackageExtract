#include "..\include\file.h"



file::file(const std::string& filepath) :
	m_filepath(filepath)
{
}


file::~file()
{
}


std::string file::get_filename()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

std::string file::get_path()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(0, found);
	return ret;
}

std::string file::get_extension()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of(".");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

