//Dear emacs, this is -*- c++ -*-

/**
 * @file config/config/NeuronBackProp.h
 *
 * @brief Describes parameters for a neuron in backpropagation mode.
 */

#ifndef CONFIG_NEURONBACKPROP_H
#define CONFIG_NEURONBACKPROP_H

#include "config/Parameter.h"

namespace config {

  /**
   * Describes how to load back propagation parameters
   */
  class NeuronBackProp : public Parameter {

  public: //types of activation functions accepted
    enum ActivationFunction { TANH=0, ///< classical hyperbolic tangent
			      SIGMOID=1, ///< sigmoid function
			      LINEAR=2}; ///< linear (y=x)
    
    typedef enum ActivationFunction ActivationFunction;

  public: //interface
    
    /**
     * How to start up from a node
     *
     * @param node The XML node where my parameters are
     */
    NeuronBackProp(const xmlNodePtr node);

    /**
     * How to start up from simple values
     *
     * @param af The activation function to use
     */
    NeuronBackProp(const ActivationFunction& af);

    /**
     * Copies these parameters in construction
     *
     * @param other The other set of parameters to copy
     */
    NeuronBackProp(const NeuronBackProp& other);

    /**
     * Copies these parameters in operation
     *
     * @param other The other set of parameters to copy
     */
    NeuronBackProp& operator= (const NeuronBackProp& other);

    /**
     * Destructor virtualisation
     */
    virtual ~NeuronBackProp() {}

    /**
     * Returns the activation function
     */
    const ActivationFunction& activation_function() const { return m_af; }

    /**
     * Clones this object
     */
    virtual Parameter* clone () const;

    /**
     * Gets the XML representation for these parameter
     */
    virtual xmlNodePtr node ();

  private: //representation

    ActivationFunction m_af; ///< my activation function

  };

}

#endif /* CONFIG_NEURONBACKPROP_H */
