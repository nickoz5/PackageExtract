#pragma once

#include <string>

class file
{
public:
	file(const std::string& filepath);
	~file();

public:
	// return just the filename and extension
	std::string file::get_filename();
	// return the path
	std::string file::get_path();
	// return the extension
	std::string file::get_extension();

private:
	std::string m_filepath;
};

