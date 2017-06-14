#include "jobinfo.h"

#include <stdlib.h>
#include <string.h>
#include <memory>
#ifdef _WIN32
#include <Shlwapi.h>
#endif

#ifdef _WIN32
#define strcasestr StrStrIA
#define strcasecmp strcmp
#define strncasecmp stricmp
#endif

#define BUFSIZ 512

CJobInfo::CJobInfo()
{
	m_nType = TYPE_UNKNOWN;
	m_nEpisodeNum = 0;
	m_nSeasonNum = 0;
	m_nYear = 0;
	m_szTitle[0] = '\0';
	m_szTorrentTitle[0] = '\0';
	m_szTorrentDir[0] = '\0';
	m_szTorrentStatus[0] = '\0';
	m_nTorrentStatus = 0;
	m_bCopyStarted = false;
}

bool CJobInfo::ParseTorrentName()
{
	// find show name
	char szTitle[BUFSIZ], szWord[BUFSIZ];
	int nTitleLen = 0, nWordLen = 0;
	const char* p = NULL;
	bool bFoundTitle = false;
	bool bFoundSeasonEp = false;

	memset(szTitle, 0, sizeof(szTitle));
	memset(szWord, 0, sizeof(szWord));

	//			
	// parse parameters (torrent name) (torrent target folder)
	// determine torrent is TV/movie;
	//

	//
	// find known TV tags
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "HDTV");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "WEB-DL");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "PDTV");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "DSR");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "WEB");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "WEBRip");
	
	if (p != NULL) {
		// is TV show..
		for (unsigned int i=0; m_szTorrentTitle[i] != '\0'; i++) {
			char ch = m_szTorrentTitle[i];

			if (ch == '.' || ch == ' ' || ch == '_' || i == strlen(m_szTorrentTitle) - 1) {
				// process this word..
				
				// looking for tags that indicate the title has ended.
				//
				if (!strcasecmp(szWord, "720p") || !strcasecmp(szWord, "1080p") || !strcasecmp(szWord, "HDTV")) {
					break;
				}

				// check S00E00
				if (nWordLen >= 6 && (szWord[0] == 'S' || szWord[0] == 's') && (szWord[3] == 'E' || szWord[3] == 'e'))
					if (szWord[1] >= '0' && szWord[1] <= '9' && szWord[2] >= '0' && szWord[2] <= '9' &&
						szWord[4] >= '0' && szWord[4] <= '9' && szWord[5] >= '0' && szWord[5] <= '9')
					{
						// found season/episode number
						m_nSeasonNum = atoi(szWord+1);
						m_nEpisodeNum = atoi(szWord+4);
						bFoundSeasonEp = true;
						break;
					}
				
				// check S00
				if (nWordLen >= 3 && (szWord[0] == 'S' || szWord[0] == 's'))
					if (szWord[1] >= '0' && szWord[1] <= '9' && szWord[2] >= '0' && szWord[2] <= '9')
					{
						// found season/episode number
						m_nSeasonNum = atoi(szWord+1);
						m_nEpisodeNum = 0;
						bFoundSeasonEp = true;
						break;
					}

				// check 00x00
				if (nWordLen == 5 && (szWord[2] == 'X' || szWord[2] == 'x'))
					if (szWord[0] >= '0' && szWord[0] <= '9' && szWord[1] >= '0' && szWord[1] <= '9' &&
						szWord[3] >= '0' && szWord[3] <= '9' && szWord[4] >= '0' && szWord[4] <= '9')
					{
						// found season/episode number
						m_nSeasonNum = atoi(szWord+0);
						m_nEpisodeNum = atoi(szWord+3);
						bFoundSeasonEp = true;
						break;
					}
					
				
				// looking for a year/date episode name
				//
				if (nWordLen == 4 && szWord[0] >= '1' && szWord[0] <= '2' &&
						(szWord[1] == '9' || szWord[1] == '0') &&
						szWord[2] >= '0' && szWord[2] <= '9' && szWord[3] >= '0' && szWord[3] <= '9')
				{
					if (i + 8 < strlen(m_szTorrentTitle))
					{	
						char szDate[7];
						for (unsigned int i2 = i; i2 <= i + 6; i2++)
							szDate[i2 - i] = m_szTorrentTitle[i2];

						// find a date..
						if ((szDate[0] == '.' || szDate[0] == ' ' || szDate[0] == '_' || szDate[0] == '/') &&
							szDate[1] >= '0' && szDate[1] <= '9' && szDate[2] >= '0' && szDate[2] <= '9' &&
							(szDate[3] == '.' || szDate[3] == ' ' || szDate[3] == '_' || szDate[3] == '/') &&
							szDate[4] >= '0' && szDate[4] <= '9' && szDate[5] >= '0' && szDate[5] <= '9')
						{
							// found season/episode number
							m_nSeasonNum = -1;
							m_nEpisodeNum = -1;
							bFoundSeasonEp = true;
							break;
						}
					}
				}

				if (nTitleLen > 0) {
					szTitle[nTitleLen++] = ' ';
					szTitle[nTitleLen] = '\0';
				}
				strcat(szTitle, szWord);
				nTitleLen += nWordLen;

				// start the next word..
				nWordLen = 0;
				memset(szWord, 0, sizeof(szWord));

				// skip '.' char
				continue;
			}

			szWord[nWordLen++] = ch;
		}
		if (nTitleLen > 0) {
			strcpy(m_szTitle, szTitle);
			bFoundTitle = true;
		}

		if (bFoundTitle && bFoundSeasonEp)
		{
			m_nType = TYPE_TV_SERIES;

			FixTitleCase();

			return (bFoundTitle && bFoundSeasonEp) ? true : false;
		}
	}
	
	nTitleLen = 0;
	nWordLen = 0;
	p = NULL;
	bFoundTitle = false;
	bFoundSeasonEp = false;
	szTitle[0] = '\0';

	//
	// find known movie tags
	bool bIsDVDR = false;
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "BluRay");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "BDRip");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "DTS");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "blu-ray");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "WEBSCR");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "DVDRip");
	if (p == NULL) p = strcasestr(m_szTorrentTitle, "WEB-DL");
	if (p == NULL) {
		p = strcasestr(m_szTorrentTitle, "DVDR");
		if (p != NULL) bIsDVDR = true;
	}

	if (p != NULL) {

		for (unsigned int i=0; m_szTorrentTitle[i] != '\0'; i++) {
			char ch = m_szTorrentTitle[i];

			if (ch == '.' || ch == ' ' || ch == '_' || i == strlen(m_szTorrentTitle) - 1) {
				// process this word..
				
				// looking for tags that indicate the title has ended.
				//
				if (!strncasecmp(szWord, "720p", 4) || !strncasecmp(szWord, "1080p", 5) || !strncasecmp(szWord, "BluRay", 6) ||
					!strncasecmp(szWord, "Blu-Ray", 7) || !strncasecmp(szWord, "LIMITED", 7) || !strncasecmp(szWord, "WEBSCR", 6) || !strncasecmp(szWord, "PROPER", 6) || !strncasecmp(szWord, "DVDR", 4)) {
					break;
				}

				if (nWordLen == 4 && (szWord[0] >= '0' && szWord[0] <= '9' && szWord[1] >= '0' && szWord[1] <= '9' &&
					szWord[2] >= '0' && szWord[2] <= '9' && szWord[3] >= '0' && szWord[3] <= '9'))
				{
					int iYear = atoi(szWord);
					if (iYear > 1970 && iYear < 2020) {
						// found movie year
						m_nYear = iYear;
						break;
					}
				}

				if (nTitleLen > 0) {
					szTitle[nTitleLen++] = ' ';
					szTitle[nTitleLen] = '\0';
				}
				strcat(szTitle, szWord);
				nTitleLen += nWordLen;

				// start the next word..
				nWordLen = 0;
				memset(szWord, 0, sizeof(szWord));

				// skip '.' char
				continue;
			}

			szWord[nWordLen++] = ch;
		}
		if (nTitleLen > 0) {
			strcpy(m_szTitle, szTitle);
			bFoundTitle = true;
		}

		if (bIsDVDR)
			m_nType = TYPE_MOVIE_DVD;
		else
			m_nType = TYPE_MOVIE;

		if(bFoundTitle)
		{
			FixTitleCase();
			return true;
		}
	}

	return false;
}

void CJobInfo::FixTitleCase()
{
	if (m_szTitle[0] == '\0')
		return;

	bool bNewWord = true;
	for (size_t i=0; i<sizeof(m_szTitle); i++)
	{
		if (m_szTitle[i] == '\0') break;
		if (m_szTitle[i] == ' ')
		{
			bNewWord = true;
			continue;
		}

		if (bNewWord)
		{
			// first letter, make upper case
			if (m_szTitle[i] >= 'a' && m_szTitle[i] <= 'z')
				m_szTitle[i] = 'A' + (m_szTitle[i] - 'a');
			bNewWord = false;
		}
		else
		{
			// make sure its lowercase
			if (m_szTitle[i] >= 'A' && m_szTitle[i] <= 'Z')
				m_szTitle[i] = 'a' + (m_szTitle[i] - 'A');

		}
	}
}

//
//void CJobInfo::WriteData(CFile& file)
//{
//	char buf[4096];
//	int nLen = 0;
//	static const int FILE_VERSION = 0x01;
//
//	TRY {
//		file.Write(&FILE_VERSION, sizeof(FILE_VERSION));
//
//		memset(buf, 0, sizeof(buf));
//		strcpy(buf, m_szTorrentTitle);
//		nLen = m_szTorrentTitle.GetLength();
//		file.Write(&nLen, sizeof(nLen));
//		file.Write(buf, nLen);
//		
//		memset(buf, 0, sizeof(buf));
//		strcpy(buf, m_szTorrentDir);
//		nLen = m_szTorrentDir.GetLength();
//		file.Write(&nLen, sizeof(nLen));
//		file.Write(buf, nLen);
//		
//		memset(buf, 0, sizeof(buf));
//		strcpy(buf, m_szTorrentStatus);
//		nLen = m_szTorrentStatus.GetLength();
//		file.Write(&nLen, sizeof(nLen));
//		file.Write(buf, nLen);
//
//		file.Write(&m_nTorrentStatus, sizeof(m_nTorrentStatus));
//
//	} CATCH(CFileException, e) {
//
//	}
//	END_TRY
//}
//
//void CJobInfo::ReadData(CFile& file)
//{
//	char buf[4096];
//	int nLen = 0;
//	int FILE_VERSION = 0;
//
//	TRY {
//		file.Read(&FILE_VERSION, sizeof(FILE_VERSION));
//
//		memset(buf, 0, sizeof(buf));
//		file.Read(&nLen, sizeof(nLen));
//		file.Read(buf, nLen);
//		m_szTorrentTitle = buf;
//		
//		memset(buf, 0, sizeof(buf));
//		nLen = m_szTorrentDir.GetLength();
//		file.Read(&nLen, sizeof(nLen));
//		file.Read(buf, nLen);
//		m_szTorrentDir = buf;		
//
//		nLen = m_szTorrentStatus.GetLength();
//		file.Read(&nLen, sizeof(nLen));
//		file.Read(buf, nLen);
//		m_szTorrentStatus = buf;
//
//		file.Read(&m_nTorrentStatus, sizeof(m_nTorrentStatus));
//
//	} CATCH(CFileException, e) {
//
//	}
//	END_TRY
//}
//
