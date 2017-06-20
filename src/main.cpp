#include "main.h"
#include "package.h"
#include "environment.h"
#include "spdlog/spdlog.h"
#include "test.h"

std::shared_ptr<spdlog::logger> logger()
{
	std::shared_ptr<spdlog::logger> logger = spdlog::get("pkgext");
	return logger;
}

void init_logger()
{
	try
	{
		std::string filename = environment::get("PKGEXT_LOG");
		if (filename.empty())
			spdlog::stdout_color_mt("pkgext");
		else
			// Create a daily logger - a new file is created every day on 2:30am
			spdlog::daily_logger_mt("pkgext", filename, 2, 30);

		// trigger flush if the log severity is error or higher
		logger()->flush_on(spdlog::level::err);

		// Customize msg format for all messages
		spdlog::set_pattern("[%H:%M:%S %z] [thread %t] %v");
	}
	// Exceptions will only be thrown upon failed logger or sink construction (not during logging)
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

int main(int argc, char ** argv)
{
	setlocale(LC_ALL, "");

	if (!environment::init(argc, argv))
	{
		std::cout << "Error: initializing environment" << std::endl;
		return -1;
	}

	init_logger();

	environment::dump_environment();

	// run tests if requested..
	if (environment::get("TEST") == "ALL")
	{
		if (!test_pkgext::run())
			return -1;
		return 0;
	}

	package* a_package = new package();

	// execute the package extraction
	bool result = a_package->process();

	delete a_package;

	return result ? 0 : -1;
}

std::string to_title_case(const std::string& string)
{
	std::string out(string);
	bool bNewWord = true;
	for (size_t i = 0; i < out.length(); i++)
	{
		if (out[i] == ' ')
		{
			bNewWord = true;
			continue;
		}

		if (bNewWord)
		{
			// first letter, make upper case
			if (out[i] >= 'a' && out[i] <= 'z')
				out[i] = 'A' + (out[i] - 'a');
			bNewWord = false;
		}
		else
		{
			// make sure its lowercase
			if (out[i] >= 'A' && out[i] <= 'Z')
				out[i] = 'a' + (out[i] - 'A');

		}
	}
	return out;
}

std::string to_lower(const std::string& string)
{
	std::string out;
	for (size_t i = 0; i < string.length(); i++)
	{
		if (string[i] >= 'A' && string[i] <= 'Z')
			out.push_back('a' + (string[i] - 'A'));
		else
			out.push_back(string[i]);
	}
	return out;
}

