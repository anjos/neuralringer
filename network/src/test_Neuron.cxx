//Dear Emacs, this is -*- c++ -*-

/**
 * @file test_Neuron.cxx
 *
 * @brief Tests for the network::Neuron and related classes.
 */

#include "network/Neuron.h"
#include "network/InputNeuron.h"
#include "network/BiasNeuron.h"
#include "network/OutputNeuron.h"
#include "network/HiddenNeuron.h"
#include "network/Synapse.h"
#include "network/SynapseBackProp.h"
#include "network/NeuronBackProp.h"
#include "sys/Reporter.h"
#include "data/PatternSet.h"
#include "config/NeuronBackProp.h"
#include "config/SynapseBackProp.h"

/**
 * Builds a small network and test the functionality of the different
 * components. The test we draw here is to solve the xor problem. The 
 * network configuration is very small: 2 input neurons, 2 hidden
 * neurons and 1 output neuron.
 */
int main (void)
{
  //Create a system reporter
  sys::Reporter reporter("local");

  //Create the network
  reporter.report("Creating Neuron BackPropagation Strategy...");
  config::NeuronStrategyType nstrat = config::NEURON_BACKPROP;
  config::Parameter* nsparam = 
    new config::NeuronBackProp(config::NeuronBackProp::TANH);

  reporter.report("Creating the Neurons...");
  network::Neuron* input[2] = {new network::InputNeuron,
			      new network::InputNeuron};
  network::Neuron* hidden[2] = {new network::HiddenNeuron(nstrat, nsparam),
				new network::HiddenNeuron(nstrat, nsparam)};
  network::Neuron* output = new network::OutputNeuron(nstrat, nsparam);
  delete nsparam;

  reporter.report("Creating Synapse BackPropagation Strategy...");
  config::SynapseStrategyType sstrat = config::SYNAPSE_BACKPROP;
  config::Parameter* ssparam = new config::SynapseBackProp(0.1, 0, 0);
  reporter.report("Creating Synapses...");
  network::Synapse* synapse[6] = {new network::Synapse(0.5, sstrat, ssparam),
				  new network::Synapse(0.5, sstrat, ssparam),
				  new network::Synapse(0.5, sstrat, ssparam),
				  new network::Synapse(0.5, sstrat, ssparam),
				  new network::Synapse(0.5, sstrat, ssparam),
				  new network::Synapse(0.5, sstrat, ssparam)};
  delete ssparam;
  reporter.report("Connecting the Neurons together...");
  synapse[0]->connect(input[0],hidden[0]);
  synapse[1]->connect(input[0],hidden[1]);
  synapse[2]->connect(input[1],hidden[0]);
  synapse[3]->connect(input[1],hidden[1]);
  synapse[4]->connect(hidden[0],output);
  synapse[5]->connect(hidden[1],output);

  ///Create the data to train
  reporter.report("Creating the Pattern Set to use...");
  data::Pattern p0(2,0); //0,0
  data::Pattern p1(2,0); //0,1
  p1[1] = 1;
  data::Pattern p2(2,0); //1,0
  p2[0] = 1;
  data::Pattern p3(2,1); //1,1
  data::PatternSet ps(4,2);
  ps.set_pattern(0,p0);
  ps.set_pattern(1,p1);
  ps.set_pattern(2,p2);
  ps.set_pattern(3,p3);

  ///Show we can run
  reporter.report("Running once with all Patterns...");
  for (size_t i = 0; i<ps.size(); ++i) {
    data::Ensemble x0(1,ps.pattern(i)[0]);
    data::Ensemble x1(1,ps.pattern(i)[1]);
    input[0]->run(x0);
    input[1]->run(x1);
    RINGER_REPORT(reporter, ps.pattern(i) << " -> " << output->state());
  }

  ///Delete resources
  for (size_t i = 0; i<6; ++i) delete synapse[i];
  delete input[0];
  delete input[1];
  delete hidden[0];
  delete hidden[1];
  delete output;
}
