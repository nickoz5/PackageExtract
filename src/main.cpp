#include "main.h"
#include "package.h"
#include "environment.h"

#include "rar.hpp"



int main(int argc, char ** argv)
{
#ifdef _UNIX
	setlocale(LC_ALL, "");
#endif
	
	ErrHandler.SetSignalHandlers(true);
		
#ifdef _WIN_ALL
	SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif

	if (!environment::init(argc, argv))
	{
		std::cout << "Error: initializing environment" << std::endl;
		return -1;
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

