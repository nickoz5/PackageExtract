#include "main.h"
#include "fileio.h"
#include "environment.h"
#include "filefinder.h"


fileio::fileio(const std::string& filepath) :
	m_filepath(filepath)
{
}

std::string fileio::get_filename()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

std::string fileio::get_path()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of("/\\");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(0, found+1);
	return ret;
}

std::string fileio::get_extension()
{
	std::string ret(m_filepath);
	size_t found = m_filepath.find_last_of(".");
	if (found == std::string::npos)
		return ret;
	ret = m_filepath.substr(found+1);
	return ret;
}

bool fileio::exists()
{
	if (!access(m_filepath.c_str(), 0x00))
		return true;
	return false;
}

bool fileio::mkdir()
{
#ifdef _WIN32
	if (::_mkdir(m_filepath.c_str()) == -1)
		return false;
#else
	if (::mkdir(m_filepath, 0777) == -1)
		return false;
#endif // _WIN32
	return true;
}

std::string fileio::get_temp_path()
{
	std::string temppath(environment::get("PKGEXT_TEMP"));
	if (temppath.empty())
		temppath = get_path();

	std::string destpath = temppath;
	if (destpath.at(destpath.length() - 1) != fileio::s_dir_sep)
		destpath += fileio::s_dir_sep;

	destpath += "_extracted";

	return destpath;
}


int fileio::remove()
{
	find_files finder;

	if (!finder.find_first(m_filepath))
		return 0;

	do
	{
		std::string filepath = finder.get_filepath();
		if (finder.is_dots())
			continue;

		if (finder.is_directory())
		{
			fileio(filepath).remove();
		}
		else
		{
			if (::remove(filepath.c_str()) == -1)
			{
				std::cout << "Error: unable to remove " << filepath << std::endl;
				return -1;
			}
		}
	} while (finder.find_next());

	return ::remove(m_filepath.c_str());
}
