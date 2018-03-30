#include "main.h"
#include "fileio.h"
#include "environment.h"
#include "filefinder.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <fcntl.h>
#endif // _WIN32


#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif


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

bool fileio::copy(const std::string& dest) const
{
#ifndef _WIN32
	int inputFd, outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];

	inputFd = open(m_filepath.c_str(), O_RDONLY);
	if (inputFd == -1)
	{
		LOG_ERROR("error opening source file");
		return false;
	}

	openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
		S_IROTH | S_IWOTH;      /* rw-rw-rw- */

	outputFd = open(dest.c_str(), openFlags, filePerms);
	if (outputFd == -1)
	{
		LOG_ERROR("error opening destination file");
		return false;
	}

	/* Transfer data until we encounter end of input or an error */

	while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
	{
		if (write(outputFd, buf, numRead) != numRead)
		{
			LOG_ERROR("error writing buffer to destination - disk full?");
			return false;
		}
	}

	if (numRead == -1)
	{
		LOG_ERROR("error reading input file");
		return false;
	}
	if (close(inputFd) == -1)
	{
		LOG_ERROR("error closing input handle");
		return false;
	}
	if (close(outputFd) == -1)
	{
		LOG_ERROR("error closing output handle");
		return false;
	}
#else
	::CopyFile(m_filepath.c_str(), dest.c_str(), FALSE);
#endif //_WIN32

	return true;
}

bool fileio::move(const std::string& dest) const
{
	return ::rename(m_filepath.c_str(), dest.c_str());
}
