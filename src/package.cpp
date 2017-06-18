#include "main.h"
#include "package.h"
#include "environment.h"
#include "fileio.h"
#include "filefinder.h"
#include "artefact.h"
#include "rar.hpp"

//#ifndef _WIN32
//#include <unistd.h>
//#include <sys/time.h>
//#include <dirent.h>
//#include <sys/param.h>
//#include <stdlib.h>
//#endif
//
//#include <sys/types.h>
//#include <sys/stat.h>


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

	std::cout << "Scanning source path \"" << sourcepath << "\"" << std::endl;

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
			std::cout << "Error: Unable to create torrent temp folder for extracting \"" << destpath << "\"" << std::endl;
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

	std::cout << "Found artefact:" << std::endl;
	std::cout << "  Type: " << this_artefact.get_type_string() << std::endl;
	std::cout << "  Title: " << this_artefact.get_title() << std::endl;
	std::cout << "  Season: " << this_artefact.get_season() << std::endl;
	std::cout << "  Episode: " << this_artefact.get_episode() << std::endl;

	std::string destfilepath;
	if (!this_artefact.get_destination_path(destfilepath))
		return false;

	destfilepath += fileio(filepath).get_filename();

	std::cout << "Deploying artefact \"" << filepath << "\" to \"" << destfilepath << "\"" << std::endl;

	// make sure target file does not exist..
	if (fileio(destfilepath).exists())
	{
		// file exists..
		if (fileio(destfilepath).remove() == -1)
		{
			std::cout << "Error: Unable to delete existing target" << std::endl;
			return false;
		}
	}

	if (::rename(filepath.c_str(), destfilepath.c_str()) == -1)
	{
		std::cout << "Error: Deployment failed" << std::endl;

		fileio(destfilepath).remove();
		return false;
	}

	std::cout << "Deployment completed successfully" << std::endl;

	return true;
}

int package::extract_archive(const std::string& filepath, const std::string& destpath)
{
	std::cout << "Extracting \"" << filepath << "\" to \"" << destpath << "\"" << std::endl;
	
	char* argv[] = { "unrar", "e", "-o+", "-inul", (char*)filepath.c_str(), (char*)destpath.c_str(), NULL };
	int argc = sizeof(argv) / sizeof(char*) - 1;

	//"unrar e -o+ -inul " + filepath + " " + destpath;

#ifdef SFX_MODULE
	wchar ModuleName[NM];
#ifdef _WIN_ALL
	GetModuleFileName(NULL, ModuleName, ASIZE(ModuleName));
#else
	CharToWide(argv[0], ModuleName, ASIZE(ModuleName));
#endif
#endif

	CommandData *Cmd = new CommandData;

	try
	{
		Cmd->ParseCommandLine(true, argc, argv);
		if (!Cmd->ConfigDisabled)
		{
			Cmd->ReadConfig();
			Cmd->ParseEnvVar();
		}
		Cmd->ParseCommandLine(false, argc, argv);

		uiInit(false);
		InitConsoleOptions(Cmd->MsgStream, Cmd->RedirectCharset);
		InitLogOptions(Cmd->LogName, Cmd->ErrlogCharset);
		ErrHandler.SetSilent(Cmd->AllYes || Cmd->MsgStream == MSG_NULL);

		Cmd->OutTitle();
		Cmd->ProcessCommand();
	}
	catch (RAR_EXIT ErrCode)
	{
		ErrHandler.SetErrorCode(ErrCode);
	}
	catch (std::bad_alloc&)
	{
		ErrHandler.MemoryErrorMsg();
		ErrHandler.SetErrorCode(RARX_MEMORY);
	}
	catch (...)
	{
		ErrHandler.SetErrorCode(RARX_FATAL);
	}

	delete Cmd;

	return ErrHandler.GetErrorCode();
}
