//Dear emacs, this is -*- c++ -*-

/**
 * @file test_batchtrain.cxx
 *
 * Loads a neural network for batch training.
 */

#include "network/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/debug.h"
#include "data/util.h"
#include "data/Database.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 4) RINGER_FATAL(reporter, "usage: " << argv[0]
			    << " <network-file> <database-file> <epoch>");
  network::Network* net = new network::Network(argv[1], reporter);
  try {
    data::Database database(argv[2], reporter);
    data::PatternSet data(1, 1);
    database.merge(data);
    data::PatternSet target(1, 1);
    database.merge_target(true, -1, +1, target);
    unsigned int epoch = strtoul(argv[3], 0, 0);
    RINGER_REPORT(reporter, "Running input data:" << std::endl << data);
    
    data::PatternSet output(data.size(), 1, 0);
    net->run(data, output);
    RINGER_REPORT(reporter, "I got this output:" << std::endl << output);
    
    //train
    double rms_error = 1;
    double prev_rms_error = 0; //previous
    double variation = 1;
    size_t i = 0;
    while (variation > 0.01 || rms_error > 0.05) {
      net->train(data, target, epoch);
      net->run(data, output);
      data::PatternSet error(target);
      error -= output;
      prev_rms_error = rms_error;
      rms_error = data::rms(error);
      variation = std::fabs((prev_rms_error-rms_error)/prev_rms_error);
      //abs_error = data::abs_mean(error);
      RINGER_REPORT(reporter, "[epoch " << i << "] RMS error = " << rms_error);
      //RINGER_REPORT(reporter, "[epoch " << i << "] variation = " << variation);
      if (rms_error < 0.01 && variation < 0.01)
	RINGER_WARN(reporter, "I'll leave the training loop.");
      ++i;
    }

    //save result
    net->save("xor-trained.xml");
  }
  catch (sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter,
	       "I caught an exception, I'm sorry but I have to exit. Bye.");
  }
  delete net;
}




