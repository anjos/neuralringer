//Dear emacs, this is -*- c++ -*-

/**
 * @file network/Network.h
 *
 * Defines a gathering of Neurons and Synapses forming a network of
 * connections. This is a generic class and most of the load and save
 * functionality is coded here. For specialised builds, check on other
 * inherited classes (e.g. MLP).
 */

#ifndef NETWORK_NETWORK_H
#define NETWORK_NETWORK_H

#include <string>
#include <map>

#include "data/PatternSet.h"
#include "config/Network.h"
#include "network/Neuron.h"
#include "network/InputNeuron.h"
#include "network/BiasNeuron.h"
#include "network/OutputNeuron.h"
#include "network/Synapse.h"
#include "sys/Reporter.h"

namespace network {

  /**
   * Defines a common and easy to operate neural network.
   */
  class Network {

  public: //interface

    /**
     * Networks can only be saved and loaded. For building a new network,
     * please look at other inherited classes.
     *
     * @param config The filename of the configuration database
     * @param reporter The reporter to inform about changes or errors.
     */
    Network (const std::string& config, sys::Reporter& reporter);

    /**
     * Builds a network from a set of neurons and synapses which are
     * <b>already</b> connected and functional
     *
     * @param neurons A set of neurons
     * @param synapses A set of synapses that connect the previous neurons
     * together.
     * @param reporter The reporter to inform about changes or errors.
     */
    Network (const std::vector<network::Neuron*>& neurons,
	     const std::vector<network::Synapse*>& synapses,
	     sys::Reporter& reporter);

    /**
     * Destroyes the network and <b>doesn't</b> save its state
     */
    virtual ~Network ();

    /**
     * Save the current network state
     *
     * @param file The filename where to save the network state.
     * @param header An optional header to state information about this
     * (possibly) modified network.
     */
    virtual bool save (const std::string& file,
		       const config::Header* header=0) const;

    /**
     * Dumps the current network layout to a dot-file (graphviz)
     *
     * @param file The filename where to save the network layout.
     */
    virtual bool dot (const std::string& file) const;

    /**
     * Runs a Pattern over the network and gets the results. Notice that this
     * move will change the neurons/synapses internal states and therefore is
     * not a "const" operator.
     *
     * @param input The Pattern to run through the network
     * @param output The output of this run
     */
    virtual void run (const data::Pattern& input, 
		      data::Pattern& output);

    /**
     * Runs a PatternSet over the network and gets the results. This method
     * will resize the network capacity automatically, in order to run the
     * whole set in one shot.
     *
     * @param input The PatternSet to run through the network
     * @param output The output of the network is placed at this PatternSet
     */
    virtual void run (const data::PatternSet& input, 
		      data::PatternSet& output);
    
    /**
     * Trains the network with this Pattern.
     *
     * @param data The Pattern to train the neural network with.
     * @param target What is the network target for this Pattern
     */
    virtual void train (const data::Pattern& data, 
			const data::Pattern& target);

    /**
     * Trains the network with this PatternSet.
     *
     * @param data The PatternSet to train the neural network with.
     * @param target What is the network target for this supervisionised
     * training system.
     */
    virtual void train (const data::PatternSet& data, 
			const data::PatternSet& target);

    /**
     * Trains the network with this PatternSet. The training data is chosen
     * from the "data" PatternSet randomly, a number of times it is enough to
     * fill in an epoch. The targets are selected accordingly to keep the
     * system synchronised.
     *
     * @param data The PatternSet to train the neural network with.
     * @param target What is the network target for this supervisionised
     * training system.
     * @param epoch The epoch, number of patterns, with which the network
     * must be trained.
     */
    virtual void train (const data::PatternSet& data,
			const data::PatternSet& target,
			unsigned int epoch);

    /**
     * Returns the number of input neurons
     */
    inline size_t input_size (void) const { return m_input.size(); }

    /**
     * Returns the number of output neurons
     */
    inline size_t output_size (void) const { return m_output.size(); }

    /**
     * Returns the current reporter.
     */
    inline sys::Reporter& reporter(void) { return m_reporter; }

  protected: //for children
    
    /**
     * This will start-up an empty network.
     *
     * @param reporter The reporter to inform about changes or errors.
     */
    Network (sys::Reporter& reporter);

    /**
     * This reset the internal data to an external value.
     *
     * @param neurons A set of neurons
     * @param synapses A set of synapses that connect the previous neurons
     * together.
     */
    void adopt (const std::vector<network::Neuron*>& neurons,
		const std::vector<network::Synapse*>& synapses);

  private: //representation

    config::Network* m_config; ///< my private configuration
    sys::Reporter& m_reporter; ///< where to report errors
    std::map<unsigned int, Neuron*> m_neuron; ///< my neurons
    std::vector<InputNeuron*> m_input; ///< my input neurons
    std::vector<BiasNeuron*> m_bias; ///< my bias neurons
    std::vector<OutputNeuron*> m_output; ///< my output neurons
    std::map<unsigned int, Synapse*> m_synapse; ///< my synapses
  };

}

#endif /* NETWORK_NETWORK_H */
