#pragma once


#include <stdio.h>
#include <string>
#include <errno.h>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include "direct.h"
#endif

#define LONG_VERSION_STRING			"1.0.0"
#define APP_NAME					"Extractor"
#define TR_DEFAULT_RPC_WHITELIST	"127.0.0.1"


#ifdef _WIN32
#define access _access
#define stat _stat
#define stricmp _stricmp
#endif


std::string to_title_case(const std::string& string);
std::string to_lower(const std::string& string);