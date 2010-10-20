//Dear emacs, this is -*- c++ -*-

/**
 * @file test_date.cxx
 *
 * Testes ANSI/ISO to ISO8601 date conversion system.
 */

#include "sys/Reporter.h"
#include "sys/util.h"
#include "sys/debug.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 3) {
    RINGER_FATAL(reporter, "usage: " << argv[0] << " <xml|unix> " 
	       << "<date-string>" );
  }
  
  std::string what(argv[1]);
  if (what == "xml") {
    time_t t = sys::iso8601totime(argv[2]);
    RINGER_REPORT(reporter, "iso8601totime() answered \"" << 
	       	  sys::stringtime(&t) << "\"");
  }
  else if (what == "unix") {
    struct tm t;
    strptime(argv[2], "%s", &t);
    RINGER_DEBUG1("Captured time_t \"" << sys::stringtime(&t) << "\"");
    time_t tt = mktime(&t);
    std::string dummy = sys::timetoiso8601(&tt);
    RINGER_REPORT(reporter, "timetoiso8601() answered \"" << dummy << "\"");
  }
  else {
    RINGER_FATAL(reporter, "usage: " << argv[0] << " <xml|unix> " 
	         << "<date-string>" );
  }
}
