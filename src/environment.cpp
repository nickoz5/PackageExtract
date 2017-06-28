#include "main.h"
#include "environment.h"

const std::string environment::s_env_default_prefix = "ENV:";

// Contains a list of all application environment variables and a fixed
// default value (optional) Use the system environment to override.
const std::string environment::s_env_defaults[][2] =
{
	{ "PKGEXT_PATH", "." },			// path to source packages
	{ "PKGEXT_PATH_TV", "" },		// path to destination tv show files extracted from package(s)
	{ "PKGEXT_PATH_MOVIES", "" },	// path to destination movie files extracted from package(s)
#ifdef _WIN32
	{ "PKGEXT_TEMP", "ENV:TEMP" }	// temp folder used for extracting
#else
	{ "PKGEXT_TEMP", "/var/tmp" }	// full path to unrar executable
#endif // _DEBUG
};

const struct tr_option environment::s_cmdline_options[] =
{
	{ 'p', "path", "Torrent path", "p", 1, "<path>" },
	{ 'o', "option", "Sets an environment option value", "o", 1, "<key=valye>" },
	{ 0, NULL, NULL, NULL, 0, NULL }
};

environment::mapKeyValue environment::s_env_map;


bool environment::init(int argc, char** argv)
{
	const char * optarg = NULL;

	environment::read_defaults();

	// parse command line..
	while (int c = tr_getopt(get_usage(), argc, (const char**)argv, s_cmdline_options, &optarg))
	{
		switch (c)
		{
			case 'p':
				environment::set("PKGEXT_PATH", optarg);
				break;
			case 'o':
			{
				std::string keyvalue(optarg);
				if (keyvalue.empty())
					break;

				size_t pos = keyvalue.find('=');
				if (pos != std::string::npos)
				{
					std::string key(keyvalue.substr(0, pos));
					std::string value(keyvalue.substr(pos + 1, keyvalue.length() - pos - 1));
					environment::set(key, value);
				}
				break;
			}
		}
	}

	dump_environment();

	return true;
}

void environment::dump_environment()
{
	LOG("--- Environment:");
	for (mapKeyValue::iterator iter = s_env_map.begin(); iter != s_env_map.end(); iter++)
		LOG("{}={}",iter->first, iter->second);
	LOG("--- End");
}

const char * environment::get_usage(void)
{
	return
		"Extractor " LONG_VERSION_STRING "\n"
		"\n"
		"Usage: " APP_NAME " [options]";
}

void environment::read_defaults()
{
	size_t count = sizeof(s_env_defaults) / (sizeof(std::string) * 2);
	for (size_t i = 0; i < count; i++)
	{
		std::string key = s_env_defaults[i][0];

		// use the fixed default value
		std::string value = s_env_defaults[i][1];

		// check if the system environment exists
		char* env_value = getenv(key.c_str());
		if (env_value)
			value = env_value;

		// check if the fixed default specifies an environment variable
		if (value.length() > s_env_default_prefix.length())
		{
			size_t len = s_env_default_prefix.length();
			if (value.substr(0, len) == s_env_default_prefix)
			{
				std::string env_key = value.substr(len, value.length() - len);
				env_value = getenv(env_key.c_str());
				if (env_value)
					value = env_value;
				else
					value.clear();
			}
		}

		environment::set(key, value);
	}
}

std::string environment::get(const std::string& key)
{
	std::string value;
	mapKeyValue::iterator iter = s_env_map.find(key);
	if (iter != s_env_map.end())
		value = iter->second;
	return value;
}

void environment::set(const std::string& key, const std::string& value)
{
	s_env_map[key] = value;
}

