//Dear emacs, this is -*- c++ -*-

/**
 * @file type.h
 *
 * @brief Defines available types for neurons and synapses.
 */

#ifndef CONFIG_TYPE_H
#define CONFIG_TYPE_H

namespace config {

  enum NeuronType { INPUT=0, ///< neurons that serve as network input
		    BIAS=1, ///< inputless neurons that provide constants
		    HIDDEN=2, ///< neurons in the hidden layers
		    OUTPUT=3 }; ///< fan out neurons

  typedef enum NeuronType NeuronType;

  enum NeuronStrategyType { NEURON_BACKPROP=0 };
  typedef enum NeuronStrategyType NeuronStrategyType;

  enum SynapseStrategyType { SYNAPSE_BACKPROP=0 }; 
  typedef enum SynapseStrategyType SynapseStrategyType;

}

#endif /* CONFIG_TYPE_H */
