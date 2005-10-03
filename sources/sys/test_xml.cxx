//Dear emacs, this is -*- c++ -*-

/**
 * @file test_xml.cxx
 *
 * Tests the XML parsing and writing support in RINGER.
 */

#include "sys/XMLProcessor.h"
#include "sys/xmlutil.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include <cstdlib>

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");

  if (argc != 3) {
    std::string usage = "usage: ";
    usage += argv[0];
    usage += " <schema> <document> ";
    reporter.fatal(usage);
  }

  sys::XMLProcessor xmlproc(argv[1], reporter);
  sys::xml_ptr xmldoc;
  try {
    xmldoc = xmlproc.read(argv[2]);
  }
  catch (const sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter, 
	       "RINGER Exception caught at top-level. I have to exit. Bye.");
  }
  catch (const std::exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter, 
	       "std::exception caught at top-level. I have to exit. Bye.");
  }
  catch (...) {
    RINGER_FATAL(reporter, 
	       "Uncaught exception at top-level! I have to exit. Bye.");
  }
  if (xmldoc) {
    std::string message;
    message = "The document root object is \"";
    message += sys::get_element_name(xmldoc);
    message += "\"";
    reporter.report(message);
    if (sys::get_attribute_string(xmldoc, "version").size() != 0) {
      message = "The schema version is \"";
      message += sys::get_attribute_string(xmldoc, "version");
      message += "\"";
      reporter.report(message);
    }
  }
  reporter.report("The document is XML and XML Schema valid.");
  std::exit(1);
}
