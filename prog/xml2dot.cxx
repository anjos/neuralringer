//Dear emacs, this is -*- c++ -*-

/**
 * @file xml2dot.cxx
 *
 * Builds a dot-file with the network layout.
 */

#include "network/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/util.h"
#include "sys/debug.h"
#include "data/Database.h"
#include "data/RemoveMeanOperator.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 3) RINGER_FATAL(reporter, "usage: " << argv[0] 
			      << " <network-file> <dot-file>");
  try {
    if (!sys::exists(argv[1])) {
      RINGER_DEBUG1("Network file " << argv[1] << " doesn't exist.");
      throw RINGER_EXCEPTION("Network file doesn't exist");
    }
    network::Network net(argv[1], reporter);
    net.dot(argv[2]);
  }
  catch (sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what()); 
    RINGER_FATAL(reporter,
		 "I caught an exception, I'm sorry but I have to exit. Bye.");
  }
}


