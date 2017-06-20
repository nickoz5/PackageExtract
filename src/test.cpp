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

void test_pkgext::test_movies()
{
	{
		artefact an_artefact("Kong Skull Island 2017 MULTi 1080p WEB H264-SiGeRiS");
		ASSERT(an_artefact.parse(), true);
		ASSERT(an_artefact.get_type(), artefact::TYPE_MOVIE);
		ASSERT(an_artefact.get_title(), "Kong Skull Island");
		ASSERT(an_artefact.get_season(), 0);
		ASSERT(an_artefact.get_episode(), 0);
		ASSERT(an_artefact.get_year(), 2017);
	}
	{
		artefact an_artefact("Enter the Warriors Gate 2016 BluRay 1080p Dts - HD Ma5 1 10Bit H265 - d3g");
		ASSERT(an_artefact.parse(), true);
		ASSERT(an_artefact.get_type(), artefact::TYPE_MOVIE);
		ASSERT(an_artefact.get_title(), "Enter The Warriors Gate");
		ASSERT(an_artefact.get_year(), 2016);
	}
}

void test_pkgext::test_series()
{
	{
		artefact an_artefact("love and hip hop atlanta S06E14 1080p WEB x264-WEBSTER");
		ASSERT(an_artefact.parse(), true);
		ASSERT(an_artefact.get_type(), artefact::TYPE_SERIES);
		ASSERT(an_artefact.get_title(), "Love And Hip Hop Atlanta");
		ASSERT(an_artefact.get_episode(), 14);
		ASSERT(an_artefact.get_season(), 6);
	}
	//{
	//	artefact an_artefact("Confederations Cup 2017 Group B Australia vs Germany 1080p HDTV x264-VERUM");
	//	ASSERT(an_artefact.parse(), true);
	//	ASSERT(an_artefact.get_type(), artefact::TYPE_SERIES);
	//	ASSERT(an_artefact.get_title(), "Confederations Cup");
	//	ASSERT(an_artefact.get_episode(), 0);
	//	ASSERT(an_artefact.get_season(), 0);
	//	ASSERT(an_artefact.get_year(), 2017);
	//}
	{
		artefact an_artefact("Jimmy Fallon 2017 06 19 Will Ferrell 720p WEB x264 - TBS");
		ASSERT(an_artefact.parse(), true);
		ASSERT(an_artefact.get_type(), artefact::TYPE_SERIES);
		ASSERT(an_artefact.get_title(), "Jimmy Fallon");
		ASSERT(an_artefact.get_episode(), 0);
		ASSERT(an_artefact.get_season(), 0);
		ASSERT(an_artefact.get_year(), 2017);
	}
	{
		artefact an_artefact("American Ninja Warrior S09E03 720p WEB x264 - TBS");
		ASSERT(an_artefact.parse(), true);
		ASSERT(an_artefact.get_type(), artefact::TYPE_SERIES);
		ASSERT(an_artefact.get_title(), "American Ninja Warrior");
		ASSERT(an_artefact.get_episode(), 3);
		ASSERT(an_artefact.get_season(), 9);
	}
}

