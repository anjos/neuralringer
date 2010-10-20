//Dear emacs, this is -*- c++ -*-

/**
 * @file test_Synapse.cxx
 *
 * @brief Tests for the network::Synapse class.
 */

#include "network/InputNeuron.h"
#include "network/OutputNeuron.h"
#include "sys/Reporter.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include "data/Ensemble.h" 
#include "data/Pattern.h"
#include "data/PatternSet.h"
#include "network/SynapseBackProp.h"
#include "network/Synapse.h"
#include <sstream>
#include <fstream>
#include <iostream>

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc != 2) reporter.fatal(std::string("usage: ") + argv[0]
				+ " <db>");
  data::PatternSet mypat(argv[1], &reporter);
  RINGER_REPORT(reporter, "Loaded " << mypat.size() << " patterns (size=" 
	      << mypat.pattern_size() << ") from " << argv[1]);

  network::Neuron* in = new network::InputNeuron;
  config::NeuronStrategyType nstrat = config::NEURON_BACKPROP;
  config::Parameter* nsparam = 
    new config::NeuronBackProp(config::NeuronBackProp::TANH);
  network::Neuron* out = new network::OutputNeuron(nstrat, nsparam);
  delete nsparam;

  config::SynapseStrategyType sstrat = config::SYNAPSE_BACKPROP;
  config::Parameter* ssparam = new config::SynapseBackProp(0.1, 0, 0);
  network::Synapse* synapse = new network::Synapse(0.5, sstrat, ssparam);
  delete ssparam;

  synapse->connect(in,out);

  /**
   * Run a random Pattern from the file through the system.
   */
  for (unsigned int i=0; i<mypat.size(); ++i) {
    RINGER_DEBUG1("Running with pattern[" << i << "][0]");
    data::Pattern pat(1, 0);
    pat[0] = mypat.pattern(i)[0];  
    RINGER_DEBUG1("Input:  " << pat[0]);
    in->run(pat);
    RINGER_DEBUG1("Output: " << out->state());

    RINGER_REPORT(reporter, "Training for pattern[" << i << "][0]");
    data::Pattern target(1, 1);
    RINGER_DEBUG1("Error: " << target-out->state());
    out->train(target-out->state());
    RINGER_DEBUG1("Input: " << in->state());
  }

  delete synapse;
  delete in;
  delete out;
}
