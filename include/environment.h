#pragma once

#include "tr-getopt.h"
#include <map>


class environment
{
private:
	environment() { }

public:
	// initialize the environment (call once only).
	static bool init(int argc, char** argv);

	static void dump_environment();

	// delete the environment instance
	static void destroy();


public:
	static std::string get(const std::string& key);
	static void set(const std::string& key, const std::string& value);


private:
	typedef std::map<std::string, std::string> mapKeyValue;
	static mapKeyValue s_env_map;

	static const struct tr_option s_cmdline_options[];

	static const std::string s_env_defaults[][2];
	static const std::string s_env_default_prefix;

	static void read_defaults();
	
	static const char* get_usage();
};

