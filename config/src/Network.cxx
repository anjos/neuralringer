//Dear emacs, this is -*- c++ -*-

/**
 * @file config/src/Network.cxx
 *
 * Implements the neural network configuration interface class.
 */

#include "config/Network.h"
#include "sys/xmlutil.h"
#include "sys/XMLProcessor.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include "sys/util.h"
#include <cstdlib>
#include <libxml/tree.h>

config::Network::Network(const std::string& filename,
			 sys::Reporter& reporter)
  : m_reporter(reporter),
    m_header(0),
    m_synapse(),
    m_neuron()
{
  std::string schema = sys::getenv("RINGER_SCHEMA_PATH");
  if (schema.length() == 0) {
    RINGER_DEBUG1("I cannot find the standard schema path. Have you set"
		<< " RINGER_SCHEMA_PATH? Exception thrown.");
    throw RINGER_EXCEPTION("RINGER_SCHEMA_PATH not set");
  }
  schema += "/network.xsd";
  sys::XMLProcessor xmlproc(schema, m_reporter);
  RINGER_DEBUG2("Trying to parse " << filename);
  xmlNodePtr root = xmlproc.read(filename);
  if (!root) {
    RINGER_WARN(m_reporter, "XML network file " << filename 
	      << " cannot be parsed. Exception thrown.");
    throw RINGER_EXCEPTION("Cannot parse XML network file");
  }
  for (xmlNodePtr it=root->children; it; it=sys::get_next_element(it)) {
    if (it->type == XML_ELEMENT_NODE &&
	sys::get_element_name(it) == "header") {
      //loads the header information
      m_header = new config::Header(it);
      continue;
    }
    if (it->type == XML_ELEMENT_NODE && 
	sys::get_element_name(it) == "layout") {
      //loads the network layout
      size_t ne = 0;
      size_t sy = 0;
      for (xmlNodePtr jt=it->children; jt; jt=sys::get_next_element(jt)) {
	if (jt->type != XML_ELEMENT_NODE) continue;
	std::string name = sys::get_element_name(jt);
	if (jt->type == XML_ELEMENT_NODE && (name == "input" ||
					     name == "bias" ||
					     name == "hidden" ||
					     name == "output")) {
	  m_neuron.push_back(new Neuron(jt));
	  ++ne;
	}
	if (jt->type == XML_ELEMENT_NODE && name == "synapse") {
	  m_synapse.push_back(new Synapse(jt));
	  ++sy;
	}
      }
      RINGER_DEBUG2("Network file \"" << filename << "\" has "
		  << ne << " neuron(s).");
      RINGER_DEBUG2("Network file \"" << filename << "\" has " 
		  << sy << " synapse(s).");
    }
  }

  RINGER_DEBUG3("Configuration created from file \"" << filename << "\".");
}

config::Network::Network (const config::Header* header, 
			  const std::vector<Synapse*>& synapses,
			  const std::vector<Neuron*>& neurons,
			  sys::Reporter& reporter)
  : m_reporter(reporter),
    m_header(0),
    m_synapse(),
    m_neuron()
{
  if (header) m_header = new config::Header(*header);
  for (std::vector<Synapse*>::const_iterator it = synapses.begin(); 
       it != synapses.end(); ++it) m_synapse.push_back(new Synapse(**it));
  for (std::vector<Neuron*>::const_iterator it = neurons.begin(); 
       it != neurons.end(); ++it) m_neuron.push_back(new Neuron(**it));
  RINGER_DEBUG3("Configuration created from scratch.");
}

config::Network::~Network()
{
  delete m_header;
  for (std::vector<Synapse*>::iterator it = m_synapse.begin(); 
       it != m_synapse.end(); ++it) delete *it;
  for (std::vector<Neuron*>::iterator it = m_neuron.begin(); 
       it != m_neuron.end(); ++it) delete *it;
}

bool config::Network::save (const std::string& filename)
{
  RINGER_DEBUG2("Trying to save parsed document at \"" << filename << "\".");
  std::string schema = sys::getenv("RINGER_SCHEMA_PATH");
  if (schema.length() == 0) {
    RINGER_DEBUG1("I cannot find the standard schema path. Have you set"
		<< " RINGER_SCHEMA_PATH? Exception thrown.");
    throw RINGER_EXCEPTION("RINGER_SCHEMA_PATH not set");
  }
  schema += "/network.xsd";
  sys::XMLProcessor xmlproc(schema, m_reporter);
  xmlNodePtr root = xmlproc.new_document("network");
  sys::put_attribute_text(root, "version", "0.2");
  xmlAddChild(root, m_header->node());
  xmlNodePtr layout = sys::put_element(root, "layout");
  for (std::vector<Neuron*>::iterator it = m_neuron.begin(); 
       it != m_neuron.end(); ++it) xmlAddChild(layout, (*it)->node());
  for (std::vector<Synapse*>::iterator it = m_synapse.begin(); 
       it != m_synapse.end(); ++it) xmlAddChild(layout, (*it)->node());
  RINGER_DEBUG2("Finally saving file...");
  if (!xmlproc.write(filename)) return false;
  RINGER_DEBUG2("File \"" << filename << "\" was saved.");
  return true;
}
