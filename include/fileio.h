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
	std::string get_filename() const;
	
	// return the path, including trailing directory seperator
	std::string get_path() const;

	// return the extension
	std::string get_extension() const;

	bool is_directory() const;

	// return true if the file/path exists
	bool exists() const;

	bool mkdir() const;

	int remove() const;

	// gets a temporary path for use
	std::string get_temp_path() const;

private:
	std::string m_filepath;
};

