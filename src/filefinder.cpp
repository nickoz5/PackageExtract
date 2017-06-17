#include "main.h"
#include "filefinder.h"
#include "fileio.h"



find_files::find_files()
{
	m_handle = NULL;
}

find_files::~find_files()
{
	close();
}

bool find_files::find_first(const std::string& path)
{
	close();

	m_path = path;

#ifdef _WIN32
	std::string findpath(path);
	if (findpath.at(findpath.length() - 1) != fileio::s_dir_sep)
		findpath += fileio::s_dir_sep;
	findpath += "*";

	m_handle = FindFirstFileA(findpath.c_str(), &m_file_data);
	if (m_handle == NULL)
		return false;
#else
	if ((m_handle = opendir(filepath)) == NULL)
		return false;
	if (!find_next())
		return false;
#endif // _WIN32

	return true;
}

void find_files::close()
{
#ifdef _WIN32
	if (m_handle)
		FindClose(m_handle);
#else

#endif // _WIN32
}

bool find_files::find_next()
{
#ifdef _WIN32
	if (FindNextFileA(m_handle, &m_file_data) == 0)
		return false;
#else
	if (m_file_data = readdir(m_handle) == NULL)
		return false;
#endif // _WIN32

	return true;
}

std::string find_files::get_filename()
{
#ifdef _WIN32
	return m_file_data.cFileName;
#else
	return m_file_data.d_name;
#endif // _WIN32
}

std::string find_files::get_filepath()
{
	std::string filepath = get_path() + fileio::s_dir_sep + get_filename();
	return filepath;
}

bool find_files::is_dots()
{
	if (!strcmp(get_filename().c_str(), ".") || !strcmp(get_filename().c_str(), ".."))
		return true;
	return false;
}

bool find_files::is_directory()
{
#ifdef _WIN32
	if (m_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;
#else
	struct stat fbuf;
	if (stat(get_filepath(), &fbuf) >= 0)
	{
		if (S_ISDIR(fbuf.st_mode))
			return true;
	}
#endif // _WIN32

	return false;
}
