//Dear emacs, this is -*- c++ -*-

/**
 * @file config/src/NeuronBackProp.cxx
 *
 * Implements Back Propagation parameter readout.
 */

#include "config/NeuronBackProp.h"
#include "sys/xmlutil.h"
#include "sys/debug.h"
#include "sys/Exception.h"

config::NeuronBackProp::NeuronBackProp(const xmlNodePtr node)
{
  std::string funct = sys::get_attribute_string(node, "activationFunction");
  if (funct == "tanh") {
    RINGER_DEBUG2("I will use tanh as the activation function.");
    m_af = config::NeuronBackProp::TANH;
  }
  else if (funct == "sigmoid") {
    RINGER_DEBUG2("I will use sigmoid as the activation function.");
    m_af = config::NeuronBackProp::SIGMOID;
  }
  else if (funct == "linear") {
    RINGER_DEBUG2("I will use identity as the activation function.");
    m_af = config::NeuronBackProp::LINEAR;
  }
  else {
    RINGER_DEBUG1("Backpropagation activation function \"" << funct
		<< "\" is unknown to RINGER. Exception thrown.");
    throw RINGER_EXCEPTION("Unknown backprop activation function");
  }
}

config::NeuronBackProp::NeuronBackProp
(const ActivationFunction& af)
  : m_af(af)
{
}

config::NeuronBackProp::NeuronBackProp(const NeuronBackProp& other)
  : m_af(other.m_af)
{
}

config::NeuronBackProp& config::NeuronBackProp::operator= 
(const NeuronBackProp& other)
{
  m_af = other.m_af;
  return *this;
}

config::Parameter* config::NeuronBackProp::clone () const
{
  return new NeuronBackProp(m_af);
}

xmlNodePtr config::NeuronBackProp::node ()
{
  xmlNodePtr root = sys::make_node("backPropagation");
  switch (m_af) {
  case TANH:
    sys::put_attribute_text(root, "activationFunction", "tanh");
    break;
  case SIGMOID:
    sys::put_attribute_text(root, "activationFunction", "sigmoid");
    break;
  case LINEAR:
    sys::put_attribute_text(root, "activationFunction", "linear");
    break;
  }
  return root;
}

