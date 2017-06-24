#include "main.h"
#include "artefact.h"
#include "fileio.h"
#include "environment.h"

#include <sstream>

// identifying tags
const std::string artefact::s_source_tv_tags[] = { "hdtv", "ahdtv", "pdtv", "hr.pdtv" };
const std::string artefact::s_source_disc_tags[] = { "bluray", "bdrip", "blu-ray", "dvdrip" };
const std::string artefact::s_source_web_tags[] = { "webscr", "web", "web-dl" };
const std::string artefact::s_quality_tags[] = { "720p", "1080p", "hdtv", "limited", "webscr", "proper" };

const std::string artefact::s_packagetype_tags[] =
{
	"hdtv", "web-dl", "pdtv", "dsr", "web", "webrip", "bluray",
	"bdrip", "dts", "blu-ray", "webscr", "dvdrip", "web-dl"
};


artefact::artefact(const std::string& filepath)
{
	m_filepath = filepath;
	clear();
}

void artefact::clear()
{
	m_type = TYPE_UNKNOWN;
	m_episode_no = 0;
	m_season_no = 0;
	m_year = 0;
	m_month = 0;
	m_day = 0;
}

bool artefact::parse()
{
	// find show name
	std::string filename = to_lower(fileio(m_filepath).get_filename());

	if (!is_valid_artefact(filename))
		return false;
	
	clear();
	
	// try to extract series info
	if (find_series_info())
	{
		if ((m_season_no > 0 && m_episode_no > 0) || (m_month > 0 && m_day > 0) ||
			find_source_tv_tag(filename))
		{
			m_type = TYPE_SERIES;
			return true;
		}
	}

	if (find_movie_info())
	{
		m_type = TYPE_MOVIE;
		return true;
	}
	
	return false;
}

bool artefact::find_source_disc_tag(std::string title)
{
	for (int i = 0; i < sizeof(s_source_disc_tags) / sizeof(std::string); i++)
	{
		if (title.find(s_source_disc_tags[i], 0) != std::string::npos)
			return true;
	}
	return false;
}

bool artefact::find_source_web_tag(std::string title)
{
	for (int i = 0; i < sizeof(s_source_web_tags) / sizeof(std::string); i++)
	{
		if (title.find(s_source_web_tags[i], 0) != std::string::npos)
			return true;
	}
	return false;
}

bool artefact::find_source_tv_tag(std::string title)
{
	for (int i = 0; i < sizeof(s_source_tv_tags) / sizeof(std::string); i++)
	{
		if (title.find(s_source_tv_tags[i], 0) != std::string::npos)
			return true;
	}
	return false;
}

size_t artefact::find_year_pos(int& year, size_t offset)
{
	std::string filename = to_lower(fileio(m_filepath).get_filename());
	std::string current_word;
	bool found = false;

	for (size_t pos = offset; pos < filename.length(); pos++)
	{
		char ch = filename[pos];
		if (ch == '.' || ch == ' ' || ch == '_' || pos == filename.length() - 1)
		{
			if (current_word.length() == 4)
			{
				if (is_number(current_word[0]) && is_number(current_word[1]) &&
					is_number(current_word[2]) && is_number(current_word[3]))
				{
					int value = atoi(current_word.c_str());
					if (value > 1900 && value < 2099)
					{
						// found movie year
						year = value;
						found = true;

						size_t pos_n = find_year_pos(value, pos);
						if (pos_n > 0)
						{
							year = value;
							return pos_n;
						}

						return pos - 4;
					}
				}
			}

			current_word.clear();
		}
		else
			current_word += ch;
	}

	return 0;
}

bool artefact::is_number(const char c)
{
	return c >= '0' && c <= '9';
}

bool artefact::is_seasonep_id(const std::string& title_word, size_t pos)
{
	std::string filename = fileio(m_filepath).get_filename();

	// check S00E00
	if (title_word.length() >= 6 && (title_word[0] == 's') && (title_word[3] == 'e'))
	{
		if (is_number(title_word[1]) && is_number(title_word[2]) &&
			is_number(title_word[4]) && is_number(title_word[5]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 1);
			m_episode_no = atoi(title_word.c_str() + 4);
			return true;
		}
	}

	// check S00
	if (title_word.length() >= 3 && (title_word[0] == 's'))
	{
		if (is_number(title_word[1]) && is_number(title_word[2]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 1);
			m_episode_no = 0;
			return true;
		}
	}

	// check 00x00
	if (title_word.length() == 5 && (title_word[2] == 'x'))
	{
		if (is_number(title_word[0]) && is_number(title_word[1]) &&
			is_number(title_word[3]) && is_number(title_word[4]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 0);
			m_episode_no = atoi(title_word.c_str() + 3);
			return true;
		}
	}

	// check for date format season/episode
	if (title_word.length() == 4 &&
		title_word[0] >= '1' && title_word[0] <= '2' &&
		(title_word[1] == '9' || title_word[1] == '0') &&
		is_number(title_word[2]) && is_number(title_word[3]))
	{
		if (pos + 8 < filename.length())
		{
			char szDate[7];
			for (size_t i2 = pos; i2 <= pos + 6; i2++)
				szDate[i2 - pos] = filename[i2];

			// find a date..
			if ((szDate[0] == '.' || szDate[0] == ' ' || szDate[0] == '_' || szDate[0] == '/') &&
				szDate[1] >= '0' && szDate[1] <= '9' && szDate[2] >= '0' && szDate[2] <= '9' &&
				(szDate[3] == '.' || szDate[3] == ' ' || szDate[3] == '_' || szDate[3] == '/') &&
				szDate[4] >= '0' && szDate[4] <= '9' && szDate[5] >= '0' && szDate[5] <= '9')
			{
				// found season/episode number
				m_year = atoi(title_word.substr(0, 4).c_str());

				szDate[3] = szDate[6] = '\0';
				m_month = atoi(szDate + 1);
				m_day = atoi(szDate + 4);
				return true;
			}
		}
	}

	return false;
}

bool artefact::get_destination_path(std::string& destpath)
{
	if (m_type == TYPE_SERIES)
	{
		if (!get_series_path(destpath))
			return false;
	}

	if (m_type == TYPE_MOVIE)
	{
		if (!get_movies_path(destpath))
			return false;
	}

	return true;
}

bool artefact::is_valid_artefact(const std::string& name)
{
	// find known package tags
	for (size_t i = 0; i < sizeof(s_packagetype_tags) / sizeof(std::string); i++)
	{
		if (name.find(s_packagetype_tags[i]) != std::string::npos)
			return true;
	}
	return false;
}

bool artefact::is_quality_tag(const std::string& word)
{
	for (size_t i = 0; i < sizeof(s_quality_tags) / sizeof(std::string); i++)
	{
		if (word.find(s_quality_tags[i]) != std::string::npos)
			return true;
	}
	return false;
}

bool artefact::get_series_path(std::string& destpath)
{
	if (m_type != TYPE_SERIES)
		return false;

	std::ostringstream ospath;

	ospath << environment::get("PKGEXT_PATH_TV");
	if (!fileio(ospath.str()).exists())
	{
		LOG_ERROR("Destination unavailable \"%s\"", ospath.str());
		return false;
	}

	if (ospath.str()[ospath.str().length() - 1] != fileio::s_dir_sep)
		ospath << fileio::s_dir_sep;

	ospath << m_title << fileio::s_dir_sep;
	if (!fileio(ospath.str()).exists())
	{
		if (!fileio(ospath.str()).mkdir())
		{
			LOG_ERROR("Unable to create title folder \"%s\"", ospath.str());
			return false;
		}
	}

	if (m_season_no > 0)
	{
		ospath << "Season ";
		ospath << m_season_no;
		ospath << fileio::s_dir_sep;
		if (!fileio(ospath.str()).exists())
		{
			if (!fileio(ospath.str()).mkdir())
			{
				LOG_ERROR("Unable to create title folder \"%s\"", ospath.str());
				return false;
			}
		}
	}

	destpath = ospath.str();

	return true;
}

bool artefact::get_movies_path(std::string& destpath)
{
	std::ostringstream ospath;

	ospath << environment::get("PKGEXT_PATH_TV");
	if (!fileio(ospath.str()).exists())
	{
		LOG_ERROR("Destination unavailable \"%s\"", ospath.str());
		return false;
	}

	if (ospath.str()[ospath.str().length() - 1] != fileio::s_dir_sep)
		ospath << fileio::s_dir_sep;

	return true;
}

bool artefact::find_series_info()
{
	std::string filename = to_lower(fileio(m_filepath).get_filename());

	bool found = false;
	size_t pos = 0;
	std::string current_word;

	for (pos = 0; pos < filename.length(); pos++)
	{
		char ch = filename[pos];
		if (ch == '.' || ch == ' ' || ch == '_' || pos == filename.length() - 1)
		{
			// replace with space char
			filename[pos] = ' ';

			// look for tags that indicate the title has ended.
			if (is_quality_tag(current_word))
				break;

			// check for season/episode id
			if (is_seasonep_id(current_word, pos))
			{
				pos -= current_word.length();
				found = true;
				break;
			}

			current_word.clear();
		}
		else
			current_word += ch;
	}

	// check if a year is in the title somewhere
	int year = 0;
	size_t pos_year = find_year_pos(year);
	if (pos_year > 0)
	{
		// include the year in the title unless the episode info is yyyy/mm/dd format 
		if (m_month == 0 && m_day == 0)
			pos = pos_year + 5;
		m_year = year;
	}

	if (!found && pos_year > 0)
	{
		if (find_source_tv_tag(filename) || find_source_web_tag(filename))
		{
			// Found a TV series with only a year in the title.
			// Take the string up to (and including) the year as the title,
			// and consider the rest to be the episode description.
			found = true;
		}
	}

	if (pos > 1 && pos < filename.length())
	{
		m_title = filename.substr(0, pos - 1);
		m_title = to_title_case(m_title);
	}

	return found;
}

bool artefact::find_movie_info()
{
	std::string filename = to_lower(fileio(m_filepath).get_filename());
	int year = 0;

	// check for season/episode id
	size_t pos = 0;
	if (pos = find_year_pos(year, 0))
	{
		if (find_source_disc_tag(filename) || find_source_web_tag(filename))
		{
			m_year = year;
			m_title = filename.substr(0, pos + 4);
			m_title = to_title_case(m_title);
			return true;
		}
	}

	return false;
}
