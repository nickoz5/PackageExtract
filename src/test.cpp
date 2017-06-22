#include "main.h"
#include "test.h"
#include "artefact.h"


#define ASSERT(expr, val) { \
	if (!(expr == val))	{\
	LOG_ERROR("Test failed: line {}, {} - ({} != {})", __LINE__, #expr, expr, #val);\
	throw test_exception(""); }}


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

void test_pkgext::expect(const std::string& filename, const std::string& title, artefact::TypeEnum etype, int season, int ep, int year)
{
	artefact an_artefact(filename);
	ASSERT(an_artefact.parse(), true);
	ASSERT(an_artefact.get_type(), etype);
	ASSERT(an_artefact.get_title(), title);
	ASSERT(an_artefact.get_episode(), ep);
	ASSERT(an_artefact.get_season(), season);
	ASSERT(an_artefact.get_year(), year);
}

void test_pkgext::test_movies()
{
	expect(
		"Kong Skull Island 2017 MULTi 1080p WEB H264-SiGeRiS",
		"Kong Skull Island",
		artefact::TYPE_MOVIE,
		0, 0, 2017);
	expect(
		"Enter the Warriors Gate 2016 BluRay 1080p Dts - HD Ma5 1 10Bit H265 - d3g",
		"Enter the Warriors Gate",
		artefact::TYPE_MOVIE,
		0, 0, 2016);
}

void test_pkgext::test_series()
{
	expect(
		"love and hip hop atlanta S06E14 1080p WEB x264-WEBSTER",
		"Love And Hip Hop Atlanta",
		artefact::TYPE_SERIES,
		6, 14, 0);

	expect(
		"Jimmy Fallon 2017 06 19 Will Ferrell 720p WEB x264 - TBS",
		"Jimmy Fallon",
		artefact::TYPE_SERIES,
		0, 0, 2017);

	expect(
		"House Of CARDS 2013 S05E13 1080p WEBRip x264 - CONVOY",
		"House Of Cards",
		artefact::TYPE_SERIES,
		5, 13, 2013);

	expect(
		"American Ninja Warrior S09E03 720p WEB x264 - TBS",
		"American Ninja Warrior",
		artefact::TYPE_SERIES,
		9, 3, 0);

	expect(
		"Confederations Cup 2017 Group B Australia vs Germany 1080p HDTV x264-VERUM",
		"Confederations Cup",
		artefact::TYPE_SERIES,
		0, 0, 2017);
}

