//Dear emacs, this is -*- c++ -*-

/**
 * @file network/src/test_x.cxx
 *
 * Loads a neural network for running.
 */

#include "network/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "data/Database.h"
#include "data/RemoveMeanOperator.h"

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
    data::PatternSet target(1,1);
    db.merge_target(true, -1, 1, target);
    RINGER_REPORT(reporter, "Data set is:" << std::endl << data);
    RINGER_REPORT(reporter, "Target set is:" << std::endl << target);
    net.train(data, target);
    data::PatternSet output(1,1);
    net.run(data, output);
    net.train(data, target);
    net.train(data, target);
    net.train(data, target);
    net.dot("x.dot");
    net.save("x.xml");
  }
  catch (sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what()); 
    RINGER_FATAL(reporter,
	       "I caught an exception, I'm sorry but I have to exit. Bye.");
  }
}

