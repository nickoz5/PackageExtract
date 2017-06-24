#pragma once

#include "artefact.h"

class test_exception : public std::exception
{
public:
	test_exception(std::string err)
	{
		m_err = err;
	}
	virtual const char* what() const throw()
	{
		return m_err.c_str();
	}
private:
	std::string m_err;
};

class test_pkgext
{
public:
	// run every test
	static bool run();


	// private tests.
private:
	static void test_series();
	static void test_movies();
};

