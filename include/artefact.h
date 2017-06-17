#pragma once

class artefact
{
public:
	artefact(const std::string& filepath);

public:
	std::string m_title;			// artefact title

	typedef enum
	{
		TYPE_UNKNOWN	= 0x00,
		TYPE_TV_SERIES	= 0x01,
		TYPE_MOVIE		= 0x02,
		TYPE_MOVIE_DVD	= 0x03
	} TypeEnum;
	TypeEnum m_type;

	// tv show info
	int m_season_no;
	int m_episode_no;

	// movie info
	int m_year;


public:
	bool parse();

	bool find_movieyear_id(const std::string& title_word);

	bool is_number(const char c);

	bool find_seasonep_id(const std::string& title_word, size_t pos);



private:
	bool is_valid_artefact();

	bool is_quality_tag(const std::string& word);
	void fix_case();

	std::string m_filepath;

	static const std::string s_packagetype_tags[];
	static const std::string s_quality_tags[];
};
