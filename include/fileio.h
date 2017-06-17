#pragma once


class fileio
{
public:
	fileio(const std::string& filepath);

public:
#ifdef _WIN32
	static const char s_dir_sep = '\\';
#else
	static const char s_dir_sep = '/';
#endif // _WIN32

	// return just the filename and extension
	std::string get_filename();
	
	// return the path, including trailing directory seperator
	std::string get_path();

	// return the extension
	std::string get_extension();

	// return true if the file/path exists
	bool exists();

	bool mkdir();

	int remove();

	// gets a temporary path for use
	std::string get_temp_path();

private:
	std::string m_filepath;
};

