#pragma once

class artefact
{
public:
	artefact(const std::string& filepath);

public:
	int get_year() const { return m_year; }
	int get_season() const { return m_season_no; }
	int get_episode() const { return m_episode_no; }
	
	typedef enum
	{
		TYPE_UNKNOWN = 0x00,
		TYPE_TV_SERIES = 0x01,
		TYPE_MOVIE = 0x02,
		TYPE_MOVIE_DVD = 0x03
	} TypeEnum;
	TypeEnum get_type() const { return m_type; }

	std::string get_title() const { return m_title; }


public:
	std::string get_type_string()
	{
		std::string type("Unknown");
		if (m_type == artefact::TYPE_TV_SERIES)	type = "TV Series";
		if (m_type == artefact::TYPE_MOVIE)		type = "Movie";
		return type;
	}

	bool parse();

	bool find_movieyear_id(const std::string& title_word);

	bool is_number(const char c);

	bool find_seasonep_id(const std::string& title_word, size_t pos);

	bool get_destination_path(std::string& destpath);


private:
	std::string m_filepath;

	static const std::string s_packagetype_tags[];
	static const std::string s_quality_tags[];

	std::string m_title;			// artefact title

	TypeEnum m_type;

	// tv show info
	int m_season_no;
	int m_episode_no;

	// movie info
	int m_year;

	bool is_valid_artefact(const std::string& name);

	bool is_quality_tag(const std::string& word);
	void fix_case();

	bool get_series_path(std::string& destpath);
	bool get_movies_path(std::string& destpath);
};
