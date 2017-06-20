#pragma once

#include <stdio.h>
#include <string>
#include <errno.h>
#include <iostream>
#include "spdlog/spdlog.h"

#ifndef _WIN32
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/param.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

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
#else
#define stricmp strcasecmp
#endif

extern std::shared_ptr<spdlog::logger> logger();
#define LOG(...) if (logger()) logger()->info(__VA_ARGS__);
#define LOG_ERROR(...) if (logger()) logger()->error(__VA_ARGS__);

std::string to_title_case(const std::string& string);
std::string to_lower(const std::string& string);
