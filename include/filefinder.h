#pragma once


class find_files
{
public:
	find_files();
	~find_files();

public:
	bool find_first(const std::string& path);

	// return false if no more files exist
	bool find_next();

	// returns the current filename (or directory) only.
	std::string get_filename();

	// returns the current filename (or directory) including the source path
	std::string get_filepath();

	std::string get_path()
	{
		return m_path;
	}

	bool is_dots();

	// returns true if this is a directory
	bool is_directory();

	// Close the open handle. This is called automatically from the destructor,
	// but you may wish to close it if you wish to reuse this instance for
	// multiple search operations.
	void close();

private:
#ifdef _WIN32
	typedef HANDLE FINDHANDLE;
	typedef WIN32_FIND_DATAA FILEDATA;
#else
	typedef struct __dirstream* FINDHANDLE;
	typedef struct dirent* FILEDATA;
#endif

	std::string m_path;
	FILEDATA m_file_data;
	FINDHANDLE m_handle;
};

