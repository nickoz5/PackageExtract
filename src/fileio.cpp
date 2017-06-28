#include "main.h"
#include "fileio.h"
#include "environment.h"
#include "filefinder.h"



fileio::fileio(const std::string& filepath) :
	m_filepath(filepath)
{
}

std::string fileio::get_filename() const
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

std::string fileio::get_path() const
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(0, found+1);
	return ret;
}

std::string fileio::get_extension() const
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of(".");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

bool fileio::is_directory() const
{
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributesA(m_filepath.c_str());
	return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
#else
	struct stat fbuf;
	if (stat(m_filepath.c_str(), &fbuf) >= 0)
	{
		if (S_ISDIR(fbuf.st_mode))
			return true;
	}
	return false;
#endif
}

bool fileio::exists() const
{
	if (!access(m_filepath.c_str(), 0x00))
		return true;
	return false;
}

bool fileio::mkdir() const
{
#ifdef _WIN32
	if (::_mkdir(m_filepath.c_str()) == -1)
		return false;
#else
	if (::mkdir(m_filepath.c_str(), 0777) == -1)
		return false;
#endif // _WIN32
	return true;
}

std::string fileio::get_temp_path() const
{
	std::string destpath;

	destpath = get_path();
	destpath += "_extracted";

	return destpath;
}

int fileio::remove() const
{
	find_files finder;

	if (is_directory())
	{
		if (!finder.find_first(m_filepath))
			return 0;

		do
		{
			std::string filepath = finder.get_filepath();
			if (finder.is_dots())
				continue;

			// recurse, and delete whatever this is.
			fileio(filepath).remove();

		} while (finder.find_next());
	}

	if (::remove(m_filepath.c_str()) == -1)
	{
		LOG_ERROR("Unable to remove \"{}\"", m_filepath.c_str());
		return -1;
	}

	return true;
}
