//Dear emacs, this is -*- c++ -*-

/**
 * @file test_config.cxx
 *
 * Tests the network Configuration class.
 */

#include "config/Network.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 2)
    RINGER_FATAL(reporter, "usage: " << argv[0] << " <configuration file>");
  config::Network config(argv[1], reporter);
  const std::vector<config::Neuron*>& neuron = config.neurons();
  RINGER_REPORT(reporter,"This network has " << neuron.size() << " neurons.");
  const std::vector<config::Synapse*>& synapse = config.synapses();
  RINGER_REPORT(reporter,"This network has "<<synapse.size()<< " synapses.");
  if (!config.header()) {
    RINGER_FATAL(reporter, "File didn't load correctly! I'll leave now.");
  }
  try {
    config.save("test.xml");
  }
  catch (const sys::Exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter,
	       "RINGER exception caught at top-level. I have to exit. Bye.");
  }
  catch (const std::exception& e) {
    RINGER_EXCEPT(reporter, e.what());
    RINGER_FATAL(reporter,
	       "STD exception caught at top-level. I have to exit. Bye.");
  }
  catch (...) {
    RINGER_FATAL(reporter,
	       "Uncaught exception at top-level. I have to exit. Bye.");
  }
}
