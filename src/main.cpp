
#include "jobinfo.h"
#include "tr-getopt.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/param.h>
#include <stdlib.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#define stat _stat
#endif

const char * m_szTVTargetFolder = "/volume1/TV Shows";
const char * m_szMovieTargetFolder = "/volume1/Movies";

char m_szTorrentDir[FILENAME_MAX];

char m_szCopySourceFileName[FILENAME_MAX];
char m_szCopyTargetFolder[FILENAME_MAX];
char m_szTargetFilePath[FILENAME_MAX];
char m_szCopySourceFolder[FILENAME_MAX];

static const char * s_lpszVideoFiles[] = {
	"mkv", "avi", "mp4"
};
static const char * s_lpszArchiveFiles[] = {
	"rar", "iso"
};

#define LONG_VERSION_STRING "1.0.0"
#define MY_NAME "Extractor"
#define TR_DEFAULT_RPC_WHITELIST "127.0.0.1"

/*
 *  Config File
 */
static const char *
getUsage( void )
{
    return "Extractor " LONG_VERSION_STRING
           "\n"
           "Usage: " MY_NAME " [options]";
}
static const struct tr_option options[] =
{
    { 'p', "path", "Torrent path", "p", 1, "<path>" },
    { 0, NULL, NULL, NULL, 0, NULL }
};



bool CopyVideoFile(const char* lpszFilePath);
int RecursiveDelete(char* dirname);
void FindVideoFile(const char * lpszFolder);
bool ExtractRAR(const char* lpszArchiveFile, const char* lpszExtractFolder);


void GetFileOnly(const char* lpszFilePath, char* buffer, size_t size)
{
	buffer[0] = '\0';
	const char* p = strrchr(lpszFilePath, '/');
	if (p == NULL)
		return;
	strcpy(buffer, ++p);
}

void GetFolderOnly(const char* lpszFilePath, char* buffer, size_t size)
{
	buffer[0] = '\0';
	const char* p = strrchr(lpszFilePath, '/');
	if (p == NULL)
		return;
	strncpy(buffer, lpszFilePath, (int)(p - lpszFilePath));
}

const char* GetFileExt(const char* lpszFilePath)
{
	const char* p = strrchr(lpszFilePath, '.');
	if (p == NULL)
		return NULL;
	return ++p;
}

int main(int argc, char ** argv)
{
	struct stat st;
	const char * optarg;
	int c;

#ifndef WIN32
	umask(000);
#endif //WIN32

	// check destination folders exist..
	if (stat(m_szTVTargetFolder, &st) == -1 ||
		stat(m_szMovieTargetFolder, &st) == -1)
	{
		printf("Error: Please set EXTRACTOR_TV_DEST and EXTRACTOR_MOVIE_DEST environment variables.\n");
		return -1;
	}

	// parse command line..
	while(( c = tr_getopt( getUsage(), argc, (const char**)argv, options, &optarg )))
	{
		switch( c )
		{
			case 'p':
				strcpy(m_szTorrentDir, optarg);
				break;
		}
	}

	FindVideoFile(m_szTorrentDir);
	return 0;
}


void FindVideoFile(const char * lpszFolder)
{
	char             szFilename[0x100];
	_DIR             *dirp;
	struct dirent   *dp;
	struct stat      fbuf;


	printf("Finding video files in: %s\n", lpszFolder);

	// find video files..
	if((dirp = opendir(lpszFolder)) == NULL)
	{
		perror("Unable to open source folder");
		return;
	}

	while((dp = readdir(dirp)) != NULL)
	{
		sprintf(szFilename,"%s%c%s", lpszFolder,'/',dp->d_name);
		if(stat(szFilename, &fbuf) < 0)
		{
			perror("stat() failed");
			return;
		}

		// get filename extension
		const char* ext = GetFileExt(szFilename);

		// check if folder;
		if(S_ISDIR(fbuf.st_mode))
		{
			if(strcmp(dp->d_name, ".") &&
			   strcmp(dp->d_name, ".."))
			{
				FindVideoFile(szFilename);
			}
		}

		// check if extension is known
		for (size_t i=0; i<sizeof(s_lpszVideoFiles)/sizeof(char*); i++)
		{
			if (!strcasecmp(s_lpszVideoFiles[i], ext))
			{
				// copy video file to <target dir>\series name\Season <season number>
				CopyVideoFile(szFilename);
			}
		}

		// find compressed files..
		for (size_t i=0; i<sizeof(s_lpszArchiveFiles) / sizeof(const char *); i++)
		{
			if (!strcasecmp(s_lpszArchiveFiles[i], ext))
			{
				// if file contains 'sample' - ignore.

				// extract archive file..
				char szDestFolder[FILENAME_MAX];
				sprintf(szDestFolder, "%s/_extracted", lpszFolder);

				if (stat(szDestFolder, &fbuf) == -1)
				{
					if (mkdir(szDestFolder, 0777) == -1)
					{
						perror("Unable to create torrent temp folder for extracting");
						continue;
					}
				}

				if (!strcasecmp(s_lpszArchiveFiles[i], "iso"))
				{
					// TODO
				} else {
					ExtractRAR(szFilename, szDestFolder);
				}

				// find extracted file..
				FindVideoFile(szDestFolder);
				RecursiveDelete(szDestFolder);
			}
		}
	}

	closedir(dirp);
}


int RecursiveDelete(char* dirname)
{
  DIR *dp;
  struct dirent *ep;
  char abs_filename[FILENAME_MAX];

  dp = opendir (dirname);
  if (dp != NULL)
  {
		while ((ep = readdir (dp)))
		{
			struct stat stFileInfo;
			snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirname, ep->d_name);

			if (lstat(abs_filename, &stFileInfo) < 0)
				perror ( abs_filename );

			if(S_ISDIR(stFileInfo.st_mode))
			{
				if(strcmp(ep->d_name, ".") && 
				   strcmp(ep->d_name, "..")) {
				  printf("removing directory: %s\n",abs_filename);
				  RecursiveDelete(abs_filename);
				}
			} else {
				printf("removing file: %s\n",abs_filename);
	      remove(abs_filename);
      }
    }
    (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");

  remove(dirname);
  return 0;
}

bool CopyVideoFile(const char* lpszFilePath)
{
	CJobInfo jobinfo;
	char szSourceFileName[FILENAME_MAX];
	char szSourceFolder[FILENAME_MAX];
	char szTargetFolder[FILENAME_MAX];
	struct stat st;

	printf("Searching for video files...\n");

	GetFileOnly(lpszFilePath, szSourceFileName, FILENAME_MAX);
	GetFolderOnly(lpszFilePath, szSourceFolder, FILENAME_MAX);
	if (szSourceFileName[0] == '\0' || szSourceFolder[0] == '\0')
	{
		fprintf(stderr, "Unable to find source filename and folder: %s", lpszFilePath);
		return false;
	}

	strcpy(jobinfo.m_szTorrentDir, m_szTorrentDir);
	strcpy(jobinfo.m_szTorrentTitle, szSourceFileName);
	if (!jobinfo.ParseTorrentName())
		return -1;

	printf("szTitle: %s\n", jobinfo.m_szTitle);
	printf("nType: %d\n", jobinfo.m_nType);
	printf("nSeasonNum: %d\n", jobinfo.m_nSeasonNum);
	printf("nEpisodeNum: %d\n", jobinfo.m_nEpisodeNum);

	if (jobinfo.m_nType != CJobInfo::TYPE_TV_SERIES &&
			jobinfo.m_nType != CJobInfo::TYPE_MOVIE &&
		 	jobinfo.m_nType != CJobInfo::TYPE_MOVIE_DVD)
	{
		printf("Unknown torrent type: %s\n", jobinfo.m_szTorrentTitle);
		return -1;
	}

	// if file contains 'sample' - ignore.
	if (strcasestr(szSourceFileName, "sample") != NULL)
		return false;
	
	// check again that the destination is accessible.
	if ((jobinfo.m_nType == CJobInfo::TYPE_TV_SERIES && stat(m_szTVTargetFolder, &st) == -1) ||
			(jobinfo.m_nType == CJobInfo::TYPE_MOVIE && stat(m_szMovieTargetFolder, &st) == -1))
	{
		return false;
	}

	if (jobinfo.m_nType == CJobInfo::TYPE_TV_SERIES)
	{
		char szTargetTitleDir[FILENAME_MAX];
		char szTargetSeasonDir[FILENAME_MAX];

		sprintf(szTargetTitleDir, "%s/%s", m_szTVTargetFolder, jobinfo.m_szTitle);

		if (jobinfo.m_nSeasonNum > 0)
			sprintf(szTargetSeasonDir, "%s/Season %d", szTargetTitleDir, jobinfo.m_nSeasonNum);
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
		sprintf(m_szTargetFilePath, "%s/%s", szTargetFolder, szSourceFileName);
	}

	if (jobinfo.m_nType == CJobInfo::TYPE_MOVIE)
	{
		sprintf(szTargetFolder, "%s", m_szMovieTargetFolder);

		// dont want to copy to separate folders anymore..
		//if (jobinfo.m_nYear > 0)
		//	sprintf(szTargetFolder, "%s/%s (%d)", m_szMovieTargetFolder, jobinfo.m_szTitle, jobinfo.m_nYear);
		//else
		//	sprintf(szTargetFolder, "%s/%s", m_szMovieTargetFolder, jobinfo.m_szTitle);

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
			printf("Unable to determine file extension: %s\n", jobinfo.m_szTitle);
			return false;
		}
		
		sprintf(m_szTargetFilePath, "%s/%s",
			szTargetFolder, szSourceFileName);
	}
	
	if (jobinfo.m_nType == CJobInfo::TYPE_MOVIE_DVD)
	{
		strcpy(szTargetFolder, m_szMovieTargetFolder);

		if (jobinfo.m_nYear > 0)
			sprintf(m_szTargetFilePath, "%s/%s (%d)/%s",
				szTargetFolder,
				jobinfo.m_szTitle,
				jobinfo.m_nYear,
				szSourceFileName);
		else
			sprintf(m_szTargetFilePath, "%s/%s/%s",
				szTargetFolder,
				jobinfo.m_szTitle,
				szSourceFileName);
	}

	
	strcpy(m_szCopySourceFileName, szSourceFileName);
	strcpy(m_szCopyTargetFolder, szTargetFolder);
	strcpy(m_szCopySourceFolder, szSourceFolder);

	printf("Moving file: %s to %s\n", lpszFilePath, m_szTargetFilePath);
	
	// make sure target file does not exist..
	if (jobinfo.m_nType != CJobInfo::TYPE_MOVIE && stat(m_szTargetFilePath, &st) != -1) {
		// file exists..
		if (unlink(m_szTargetFilePath) == -1)
		{
			perror("Unable to delete target");
			return false;
		}
	}

	printf("/bin/mv %s %s\n", lpszFilePath, m_szTargetFilePath);
	if (execl("/bin/mv", "/bin/mv", lpszFilePath, m_szTargetFilePath, NULL) == -1)
	{
		perror("File copy failed");
		unlink(m_szTargetFilePath);
		return false;
	}
	
	printf("Copy completed: %s to %s\n", jobinfo.m_szTitle, m_szTargetFilePath);

	return true;
}


bool ExtractRAR(const char* lpszArchiveFile, const char* lpszExtractFolder)
{
	printf("extracting %s to %s...\n", lpszArchiveFile, lpszExtractFolder);

	char cmd[FILENAME_MAX];
	sprintf(cmd, "unrar e -o+ -inul %s %s", lpszArchiveFile, lpszExtractFolder);
	int status = system(cmd);
	return status == 0;
}
