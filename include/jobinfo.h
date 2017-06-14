#pragma once

#define STR_LEN 256


typedef struct tagCommandLineInfo {
	char szTitle[STR_LEN];
	char szDirectory[STR_LEN];
	char szStatus[STR_LEN];
	int nStatusCode;
} CommandLineInfo;

class CJobInfo
{
public:
	CJobInfo();

public:
	//! Torrent status flags
	typedef enum tagStatusEnum
	{
		STATUS_STARTED		= 0x01,
		STATUS_CHECKING		= 0x02,
		STATUS_AFTER_CHECK	= 0x04,
		STATUS_CHECKED		= 0x08,
		STATUS_ERROR		= 0x10,
		STATUS_PAUSED		= 0x20,
		STATUS_AUTO			= 0x40,
		STATUS_LOADED		= 0x80
	} StatusEnum;
	
	typedef enum tagTorrentTypeEnum {
		TYPE_UNKNOWN	= 0x00,
		TYPE_TV_SERIES	= 0x01,
		TYPE_MOVIE		= 0x02,
		TYPE_MOVIE_DVD	= 0x03
	} TorrentTypeEnum;


	//
	// passed from command line..

	char m_szTorrentTitle[STR_LEN];
	char m_szTorrentDir[STR_LEN];
	int m_nTorrentStatus;
	char m_szTorrentStatus[STR_LEN];

	//
	// calculated by ParseTorrentName();

	char m_szTitle[STR_LEN];
	int m_nType;

	// tv show info
	int m_nSeasonNum;
	int m_nEpisodeNum;

	// movie info
	int m_nYear;


	bool m_bCopyStarted;

public:
	bool ParseTorrentName();


	//void WriteData(FILE* file);
	//void ReadData(FILE* file);
protected:
	void FixTitleCase();

};
