//Dear emacs, this is -*- c++ -*-

/**
 * @file LMS.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the Least Mean Square discriminator
 */

#include "network/LMS.h"
#include "network/InputNeuron.h"
#include "network/OutputNeuron.h"
#include "network/Synapse.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include "data/RandomInteger.h"
#include "config/NeuronBackProp.h"
#include "config/SynapseBackProp.h"
#include "config/type.h"

static data::RandomInteger rnd; //default initializer, do it once

/**
 * Creates a new synapse based on strategy types and parameters. The returned
 * value should be deleted by yourself.
 *
 * @param type The global synapse strategy type for this network
 * @param params The global synapse strategy parameters to apply for
 * this network
 */
network::Synapse* create_lms_synapse (const config::SynapseStrategyType& type,
                                      const config::Parameter* params)
{
  data::Feature weight = rnd.draw(RAND_MAX);
  weight -= ((data::Feature)RAND_MAX)/2;
  weight /= 10*((data::Feature)RAND_MAX)/2;
  return new network::Synapse(weight, type, params);
}

network::LMS::LMS (const size_t input,
                   const data::Feature& learn_rate,
                   const data::Pattern& input_subtract,
                   const data::Pattern& input_divide,
                   sys::Reporter& reporter)
  : network::Network(reporter)
{
  RINGER_DEBUG2("Creating simple LMS discriminator " << input << "-1" 
	      << "...");
  std::vector<network::Neuron*> neurons; //all neurons, unorganized

  //check the input normalization factor sizes
  if (input_subtract.size() != input)
    throw RINGER_EXCEPTION("Input subtraction factor and input size differ.");
  if (input_divide.size() != input)
    throw RINGER_EXCEPTION("Input division factor and input size differ.");

  //create input neurons
  for (size_t i=0; i<input; ++i) {
    network::Neuron* n = 
      new network::InputNeuron(input_subtract[i], input_divide[i]);
    neurons.push_back(n);
  }

  //create output neuron
  config::NeuronStrategyType nstrat = config::NEURON_BACKPROP;
  config::NeuronBackProp::ActivationFunction actfun = 
    config::NeuronBackProp::LINEAR;
  config::Parameter* nsparam = new config::NeuronBackProp(actfun);
  network::Neuron* n = new network::OutputNeuron(nstrat, nsparam);
  neurons.push_back(n);

  config::SynapseStrategyType sstrat = config::SYNAPSE_BACKPROP;
  config::Parameter* ssparam = 
    new config::SynapseBackProp(learn_rate, 0.0, 1.0);

  //Fully connect input to output
  std::vector<network::Synapse*> synapses; //all synapses, unorganized
  for (size_t i=0; i<(neurons.size()-1); ++i) {
	network::Synapse* s = create_lms_synapse(sstrat, ssparam);
	s->connect(neurons[i], neurons[neurons.size()-1]);
	synapses.push_back(s);
  }
  RINGER_DEBUG2("Connected input layer.");

  //Build parent network and that is it! Have fun:)
  network::Network::adopt(neurons, synapses);
  RINGER_DEBUG2("Sent information for parent network.");
}




