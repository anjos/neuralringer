//Dear emacs, this is -*- c++ -*-

/**
 * @file test_train.cxx
 *
 * Loads a neural network for training. This example will train (in the online
 * sense) the network for every pattern in your data file and then re-run the
 * input set.
 */

#include "network/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 5) RINGER_FATAL(reporter, "usage: " << argv[0] 
			    << " <network-file> <data-file> "
			    << "<target-file> <number-of-times>");
  network::Network* net = new network::Network(argv[1], reporter);
  try {
    data::PatternSet data(argv[2], &reporter);
    data::PatternSet target(argv[3], &reporter);
    unsigned int N = strtoul(argv[4], 0, 0);
    RINGER_REPORT(reporter, "Running input data:" << std::endl
		<< data);
    
    data::PatternSet output(data.size(), 1, 0);
    net->run(data, output);
    RINGER_REPORT(reporter, "I got this output:" << std::endl
		<< output);
    
    //train
    for (unsigned int j=0; j<N; ++j) {
      for (unsigned int i=0; i<data.size(); ++i) {
	net->train(data.pattern(i), target.pattern(i));
      }
    }

    net->run(data, output);
    RINGER_REPORT(reporter, "I got this output:" << std::endl
		<< output);

    net->save("xor-trained.xml");

  }
  catch (sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter,
	       "I caught an exception, I'm sorry but I have to exit. Bye.");
  }
  delete net;
}




