//Dear emacs, this is -*- c++ -*-

/**
 * @file config/config/Neuron.h
 *
 * @brief Defines a way to read neuron information from XML configuration
 * files.
 */

#ifndef CONFIG_NEURON_H
#define CONFIG_NEURON_H

#include "config/type.h"
#include "config/Parameter.h"
#include <libxml/tree.h>

namespace config {

  /**
   * Defines a neuron configuration
   */
  class Neuron {

  public:

    /**
     * Builds a neuron configuration from an XML DOM node
     */
    Neuron (const xmlNodePtr node);

    /**
     * Builds a neuron from scratch
     */
    Neuron (unsigned int id, const config::NeuronType& type,
	    const config::NeuronStrategyType* strategy=0,
	    const config::Parameter* params=0, const double& bias=0);

    /**
     * Builds a neuron configuration from another neuron
     */
    Neuron (const Neuron& other);

    /**
     * Copies a neuron configuration from another neuron
     */
    Neuron& operator= (const Neuron& other);

    /**
     * Destructor virtualisation
     */
    virtual ~Neuron();

    /**
     * Returns the node representation for this Neuron.
     */
    xmlNodePtr node ();

    /**
     * Answer stuff about this neuron
     */
    inline unsigned int id () const { return m_id; }
    inline config::NeuronType type() const { return m_type; }
    inline config::NeuronStrategyType strategy() const 
    { return m_strategy; }
    inline double bias () const { return m_bias; }
    inline const config::Parameter* parameters() const { return m_params; }

  private: //representation
    unsigned int m_id; ///< my unique identity
    config::NeuronType m_type; ///< my neuron type (enumeration)
    config::NeuronStrategyType m_strategy; ///< the activation function
    config::Parameter* m_params; ///< my strategy parameters
    double m_bias; ///< if I'm a bias neuron, I get this set

  };

}

#endif /* NEURON_H */
