/**
 * @file config.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds the interesting bits of the Config package
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "config/type.h"
#include "config/NeuronBackProp.h"
#include "config/SynapseRProp.h"
#include "config/SynapseBackProp.h"

using namespace boost::python;

void bind_config()
{
  enum_<config::NeuronType>("NeuronType")
    .value("INPUT", config::INPUT)
    .value("BIAS", config::BIAS)
    .value("HIDDEN", config::HIDDEN)
    .value("OUTPUT", config::OUTPUT)
    ;

  enum_<config::NeuronStrategyType>("NeuronStrategyType")
    .value("NEURON_BACKPROP", config::NEURON_BACKPROP)
    ;

  enum_<config::SynapseStrategyType>("SynapseStrategyType")
    .value("SYNAPSE_BACKPROP", config::SYNAPSE_BACKPROP)
    .value("SYNAPSE_RPROP", config::SYNAPSE_RPROP)
    ;

  class_<config::Parameter, boost::shared_ptr<config::Parameter>, boost::noncopyable>("Parameter", "A generic parameter object", no_init);

  scope in_object = class_<config::NeuronBackProp, boost::shared_ptr<config::NeuronBackProp>, bases<config::Parameter> >("NeuronBackProp", "The configuration of a back-propagation neuron", init<const config::NeuronBackProp::ActivationFunction&>((arg("activation_function"))))
    //.def("activation_function", &config::NeuronBackProp::activation_function, "Returns the currently configured activation function", return_internal_reference<>())
    ;
  
  enum_<config::NeuronBackProp::ActivationFunction>("ActivationFunction")
    .value("TANH", config::NeuronBackProp::TANH)
    .value("SIGMOID", config::NeuronBackProp::SIGMOID)
    .value("LINEAR", config::NeuronBackProp::LINEAR)
    ;

  class_<config::SynapseBackProp, boost::shared_ptr<config::SynapseBackProp>, bases<config::Parameter> >("SynapseBackProp", "The configuration of a classical back-prop synapse.", init<const double&, const double&, const double&>()) 
    .add_property("learning_rate", &config::SynapseBackProp::learning_rate)
    .add_property("momentum", &config::SynapseBackProp::momentum)
    .add_property("learning_rate_decay", &config::SynapseBackProp::learning_rate_decay)
    ;

  class_<config::SynapseRProp, boost::shared_ptr<config::SynapseRProp>, bases<config::Parameter> >("SynapseRProp", "The configuration of a classical back-prop synapse.", init<const double&>()) 
    .add_property("weight_update", &config::SynapseRProp::weight_update)
    ;
}
