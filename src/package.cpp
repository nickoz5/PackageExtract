#include "main.h"
#include "package.h"
#include "environment.h"
#include "fileio.h"
#include "filefinder.h"
#include "artefact.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/param.h>
#include <stdlib.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>


const char * package::s_video_files_ext[] = {
	"mkv", "avi", "mp4", "iso"
};
const char * package::s_archive_files_ext[] = {
	"rar"
};

package::package()
{
}

package::~package()
{
}


bool package::process()
{
#ifndef _WIN32
	umask(000);
#endif //WIN32

	//// check destination folders exist..
	//if (access(m_szTVTargetFolder, 0x00) == -1 ||
	//	stat(m_szMovieTargetFolder, &st) == -1)
	//{
	//	std::cout << "Error: Please set EXTRACTOR_TV_DEST and EXTRACTOR_MOVIE_DEST environment variables.\n");
	//	return -1;
	//}

	std::string source = environment::get("PKGEXT_PATH");
	scan_directory(source);

	return true;
}

void package::scan_directory(const std::string& sourcepath)
{
	find_files finder;

	std::cout << "Scanning source path: " << sourcepath << std::endl;

	// find video files..
	if (!finder.find_first(sourcepath))
	{
		std::cout << "Error: Unable to open source path" << std::endl;
		return;
	}

	do
	{
		std::string filepath = finder.get_filepath();

		// check if folder;
		if (finder.is_directory() && !finder.is_dots())
			scan_directory(filepath);
		
		// check if extension is a known video file
		else if (is_payload(filepath))
			process_payload(filepath);

		// find compressed files..
		else if (is_archive_file(filepath))
			process_archive(filepath);
	} while (finder.find_next());
}

void package::process_archive(const std::string& filepath)
{
	// if file contains 'sample' - ignore.

	// extract archive file..
	std::string destpath = fileio(filepath).get_temp_path();

	if (!fileio(destpath).exists())
	{
		if (!fileio(destpath).mkdir())
		{
			std::cout << "Error: Unable to create torrent temp folder for extracting (" << destpath << ")" << std::endl;
			return;
		}
	}

	extract_archive(filepath, destpath);

	// find extracted file(s)..
	scan_directory(destpath);

	fileio(destpath).remove();
}

bool package::is_payload(const std::string& filename)
{
	// get filename extension
	std::string ext = fileio(filename).get_extension();

	for (size_t i = 0; i < sizeof(s_video_files_ext) / sizeof(char*); i++)
	{
		if (!stricmp(s_video_files_ext[i], ext.c_str()))
			return true;
	}
	return false;
}

bool package::is_archive_file(const std::string& filename)
{
	// get filename extension
	std::string ext = fileio(filename).get_extension();

	for (size_t i = 0; i < sizeof(s_archive_files_ext) / sizeof(char*); i++)
	{
		if (!stricmp(s_archive_files_ext[i], ext.c_str()))
			return true;
	}
	return false;
}


bool package::process_payload(const std::string& filepath)
{
	// if file contains 'sample' - ignore.
	if (to_lower(filepath).find("sample") != std::string::npos)
		return false;

	artefact this_artefact(filepath);
	if (!this_artefact.parse())
		return false;

	std::cout << "szTitle: " << this_artefact.m_title << std::endl;
	std::cout << "nType: " << this_artefact.m_type << std::endl;
	std::cout << "nSeasonNum: " << this_artefact.m_season_no << std::endl;
	std::cout << "nEpisodeNum: " << this_artefact.m_episode_no << std::endl;
/*
	// check again that the destination is accessible.
	if ((jobinfo.m_nType == package::TYPE_TV_SERIES && stat(m_szTVTargetFolder, &st) == -1) ||
		(jobinfo.m_nType == package::TYPE_MOVIE && stat(m_szMovieTargetFolder, &st) == -1))
	{
		return false;
	}

	if (jobinfo.m_nType == package::TYPE_TV_SERIES)
	{
		char szTargetTitleDir[FILENAME_MAX];
		char szTargetSeasonDir[FILENAME_MAX];

		sstd::cout << szTargetTitleDir, "%s/%s", m_szTVTargetFolder, jobinfo.m_szTitle);

		if (jobinfo.m_nSeasonNum > 0)
			sstd::cout << szTargetSeasonDir, "%s/Season %d", szTargetTitleDir, jobinfo.m_nSeasonNum);
		else
			strcpy(szTargetSeasonDir, szTargetTitleDir);

		if (stat(szTargetTitleDir, &st) == -1)
		{
			if (mkdir(szTargetTitleDir, 0777) == -1)
			{
				perror("Unable to create target directory");
				return false;
			}
		}
		if (stat(szTargetSeasonDir, &st) == -1) {
			if (mkdir(szTargetSeasonDir, 0777) == -1)
			{
				perror("Unable to create target directory");
				return false;
			}
		}

		strcpy(szTargetFolder, szTargetSeasonDir);
		sstd::cout << m_szTargetFilePath, "%s/%s", szTargetFolder, szSourceFileName);
	}

	if (jobinfo.m_nType == package::TYPE_MOVIE)
	{
		sstd::cout << szTargetFolder, "%s", m_szMovieTargetFolder);

		// dont want to copy to separate folders anymore..
		//if (jobinfo.m_nYear > 0)
		//	sstd::cout << szTargetFolder, "%s/%s (%d)", m_szMovieTargetFolder, jobinfo.m_szTitle, jobinfo.m_nYear);
		//else
		//	sstd::cout << szTargetFolder, "%s/%s", m_szMovieTargetFolder, jobinfo.m_szTitle);

		if (stat(szTargetFolder, &st) == -1)
		{
			if (mkdir(szTargetFolder, 0777) == -1)
			{
				perror("Unable to create target directory");
				return false;
			}
		}

		const char* p = strrchr(szSourceFileName, '.');
		if (p == NULL)
		{
			std::cout << "Unable to determine file extension: %s\n", jobinfo.m_szTitle);
			return false;
		}

		sstd::cout << m_szTargetFilePath, "%s/%s",
			szTargetFolder, szSourceFileName);
	}

	if (jobinfo.m_nType == package::TYPE_MOVIE_DVD)
	{
		strcpy(szTargetFolder, m_szMovieTargetFolder);

		if (jobinfo.m_nYear > 0)
			sstd::cout << m_szTargetFilePath, "%s/%s (%d)/%s",
				szTargetFolder,
				jobinfo.m_szTitle,
				jobinfo.m_nYear,
				szSourceFileName);
		else
			sstd::cout << m_szTargetFilePath, "%s/%s/%s",
				szTargetFolder,
				jobinfo.m_szTitle,
				szSourceFileName);
	}


	strcpy(m_szCopySourceFileName, szSourceFileName);
	strcpy(m_szCopyTargetFolder, szTargetFolder);
	strcpy(m_szCopySourceFolder, szSourceFolder);

	std::cout << "Moving file: %s to %s\n", lpszFilePath, m_szTargetFilePath);

	// make sure target file does not exist..
	if (jobinfo.m_nType != package::TYPE_MOVIE && access(m_szTargetFilePath, 0x00)) {
		// file exists..
		if (unlink(m_szTargetFilePath) == -1)
		{
			perror("Unable to delete target");
			return false;
		}
	}

	std::cout << "/bin/mv %s %s\n", lpszFilePath, m_szTargetFilePath);
	if (execl("/bin/mv", "/bin/mv", lpszFilePath, m_szTargetFilePath, NULL) == -1)
	{
		perror("File copy failed");
		unlink(m_szTargetFilePath);
		return false;
	}

	std::cout << "Copy completed: %s to %s\n", jobinfo.m_szTitle, m_szTargetFilePath);
	*/
	return true;
}


bool package::extract_archive(const std::string& filepath, const std::string& destpath)
{
	std::cout << "Extracting " << filepath << " to " << destpath << std::endl;
	
	std::string cmd;
	cmd = "unrar e -o+ -inul " + filepath + " " + destpath;

#ifdef _WIN32
	int status = 0;
#else
	int status = system(cmd);
#endif

	return status == 0;
}
