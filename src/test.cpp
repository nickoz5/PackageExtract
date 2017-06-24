#include "main.h"
#include "test.h"
#include "artefact.h"


#ifdef _WIN32
#define TEST_ASSERT(expr, val) \
{ \
	if (!(expr == val))	{\
		LOG_ERROR("...failed: line {}, {} - ({} != {})", __LINE__, #expr, expr, #val);\
		_ASSERTE(expr == val); \
	}\
}
#else
#define TEST_ASSERT(expr, val) \
{ \
	if (!(expr == val))	{\
	LOG_ERROR("...failed: line {}, {} - ({} != {})", __LINE__, #expr, expr, #val);\
	}\
}
#endif // _WIN32

#define TEST_ARTEFACT(filename, title, etype, season, ep, year) \
{\
	LOG("[TESTING] - \"{}\"", filename);\
	artefact an_artefact(filename);\
	TEST_ASSERT(an_artefact.parse(), true);\
	TEST_ASSERT(an_artefact.get_type(), etype);\
	TEST_ASSERT(an_artefact.get_title(), title);\
	TEST_ASSERT(an_artefact.get_episode(), ep);\
	TEST_ASSERT(an_artefact.get_season(), season);\
	TEST_ASSERT(an_artefact.get_year(), year);\
}


bool test_pkgext::run()
{
	try
	{
		test_series();
		test_movies();
	}
	catch(test_exception& /*e*/)
	{
		return false;
	}
	return true;
}

void test_pkgext::test_movies()
{
	TEST_ARTEFACT(
		"Kong Skull Island 2017 MULTi 1080p WEB H264-SiGeRiS",
		"Kong Skull Island 2017", artefact::TYPE_MOVIE, 0, 0, 2017);
	TEST_ARTEFACT(
		"Enter the Warriors Gate 2016 BluRay 1080p Dts - HD Ma5 1 10Bit H265 - d3g",
		"Enter The Warriors Gate 2016", artefact::TYPE_MOVIE, 0, 0, 2016);

	TEST_ARTEFACT(
		"My 1942 Movie 2017 BluRay 1080p x264-MyGroup",
		"My 1942 Movie 2017", artefact::TYPE_MOVIE, 0, 0, 2017);
}

void test_pkgext::test_series()
{
	TEST_ARTEFACT(
		"love and hip hop atlanta S06E14 1080p WEB x264-WEBSTER",
		"Love And Hip Hop Atlanta", artefact::TYPE_SERIES, 6, 14, 0);

	TEST_ARTEFACT(
		"Jimmy Fallon 2017 06 19 Will Ferrell 720p WEB x264 - TBS",
		"Jimmy Fallon", artefact::TYPE_SERIES, 0, 0, 2017);

	TEST_ARTEFACT(
		"House Of CARDS 2013 S05E13 1080p WEBRip x264 - CONVOY",
		"House Of Cards 2013", artefact::TYPE_SERIES, 5, 13, 2013);

	TEST_ARTEFACT(
		"American Ninja Warrior S09E03 720p WEB x264 - TBS",
		"American Ninja Warrior", artefact::TYPE_SERIES, 9, 3, 0);

	TEST_ARTEFACT(
		"Confederations Cup 2017 Group B Australia vs Germany 1080p HDTV x264-VERUM",
		"Confederations Cup 2017", artefact::TYPE_SERIES, 0, 0, 2017);
}

