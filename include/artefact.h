#pragma once

void find_series();

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
		TYPE_SERIES = 0x01,
		TYPE_MOVIE = 0x02
	} TypeEnum;
	TypeEnum get_type() const { return m_type; }

	std::string get_title() const { return m_title; }


public:
	inline std::string get_type_string()
	{
		std::string type("Unknown");
		if (m_type == artefact::TYPE_SERIES)	type = "Series";
		if (m_type == artefact::TYPE_MOVIE)		type = "Movie";
		return type;
	}

	bool parse();

	// reset all attributes
	void clear();


	bool get_destination_path(std::string& destpath);


private:
	std::string m_filepath;

	static const std::string s_packagetype_tags[];
	static const std::string s_quality_tags[];

	static const std::string s_source_tv_tags[];
	static const std::string s_source_disc_tags[];
	static const std::string s_source_web_tags[];

	std::string m_title;			// artefact title

	TypeEnum m_type;

	int m_year;
	int m_month;
	int m_day;

	// season/episode format info
	int m_season_no;
	int m_episode_no;

private:
	bool find_series_info();
	bool find_movie_info();

	size_t find_year_pos(int& year, size_t offset = 0);

	bool is_number(const char c);
	bool is_seasonep_id(const std::string& title_word, size_t pos);
	bool is_valid_artefact(const std::string& name);
	bool is_quality_tag(const std::string& word);

	bool find_source_tv_tag(std::string title);
	bool find_source_disc_tag(std::string title);
	bool find_source_web_tag(std::string title);

	bool get_series_path(std::string& destpath);
	bool get_movies_path(std::string& destpath);
};
