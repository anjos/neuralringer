//Dear emacs, this is -*- c++ -*-

/**
 * @file test_run.cxx
 *
 * Loads a neural network for running.
 */

#include "network/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "data/Database.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 3) RINGER_FATAL(reporter, "usage: " << argv[0] 
			    << " <network-file> <database-file>");
  try {
    network::Network net(argv[1], reporter);
    data::Database db(argv[2], reporter);
    data::PatternSet data(1,1);
    db.merge(data);
    RINGER_REPORT(reporter, "Running input data:" << std::endl
		<< data);
    
    data::PatternSet output(data.size(), 1, 0);
    net.run(data, output);
    RINGER_REPORT(reporter, "I got this output:" << std::endl
		<< output);
  }
  catch (sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what()); 
    RINGER_FATAL(reporter,
	       "I caught an exception, I'm sorry but I have to exit. Bye.");
  }
}

