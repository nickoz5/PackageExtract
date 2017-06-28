#include "main.h"
#include "package.h"
#include "environment.h"
#include "fileio.h"
#include "filefinder.h"
#include "artefact.h"
#include "rar.hpp"



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

	std::string source = environment::get("PKGEXT_PATH");
	scan_directory(source);

	return true;
}

void package::scan_directory(const std::string& sourcepath)
{
	find_files finder;

	LOG("Scanning source path \"{}\"", sourcepath);

	// find video files..
	if (!finder.find_first(sourcepath))
	{
		LOG_ERROR("Unable to open source path \"{}\"", sourcepath);
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
	// extract archive file..
	std::string destpath = fileio(filepath).get_temp_path();

	if (!fileio(destpath).exists())
	{
		if (!fileio(destpath).mkdir())
		{
			LOG_ERROR("Unable to create torrent temp folder for extracting \"{}\"", destpath);
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

	LOG("Found artefact:");
	LOG("  Type:    {}", this_artefact.get_type_string());
	LOG("  Title:   {}", this_artefact.get_title());
	LOG("  Season:  {}", this_artefact.get_season());
	LOG("  Episode: {}", this_artefact.get_episode());

	std::string destfilepath;
	if (!this_artefact.get_destination_path(destfilepath))
		return false;

	destfilepath += fileio(filepath).get_filename();

	LOG("Deploying artefact \"{}\" to \"{}\"", filepath, destfilepath);

	// make sure target file does not exist..
	if (fileio(destfilepath).exists())
	{
		// file exists..
		if (fileio(destfilepath).remove() == -1)
		{
			LOG_ERROR("Unable to delete existing target");
			return false;
		}
	}

	if (::rename(filepath.c_str(), destfilepath.c_str()) == -1)
	{
		LOG_ERROR("Deployment failed \"{}\"", filepath);

		fileio(destfilepath).remove();
		return false;
	}

	LOG("Deployment completed successfully");

	return true;
}

int package::extract_archive(const std::string& filepath, const std::string& destpath)
{
	int nErrorCode = 0;

	LOG("Extracting \"{}\" to \"{}\"", filepath, destpath);
	
	char* argv[] = { "unrar", "e", "-o+", "-inul", (char*)filepath.c_str(), (char*)destpath.c_str(), NULL };
	int argc = sizeof(argv) / sizeof(char*) - 1;

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
		//InitConsoleOptions(Cmd->MsgStream, Cmd->RedirectCharset);

		ErrHandler.SetSilent(true);

		Cmd->OutTitle();
		Cmd->ProcessCommand();
	}
	catch (RAR_EXIT ErrCode)
	{
		nErrorCode = ErrCode;
		LOG("Extraction of \"{}\" failed - error code {}", filepath, ErrCode);
	}
	catch (std::bad_alloc&)
	{
		LOG("Extraction of \"{}\" failed - out of memory", filepath);
		nErrorCode = RARX_MEMORY;
	}
	catch (...)
	{
		LOG("Extraction of \"{}\" failed", filepath);
		nErrorCode = RARX_FATAL;
	}

	delete Cmd;

	LOG("Extraction of \"{}\" completed", filepath);
	return nErrorCode;
}
