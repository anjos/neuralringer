//Dear emacs, this is -*- c++ -*-

/**
 * @file Config.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements configuration readout.
 */

#include "rbuild/Config.h"
#include "sys/xmlutil.h"
#include "sys/XMLProcessor.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include "sys/util.h"
#include <cstdlib>
#include <libxml/tree.h>

rbuild::Config::Config (const std::string& filename, sys::Reporter& reporter)
  : m_reporter(reporter)
{
  std::string schema = sys::getenv("RINGER_SCHEMA_PATH");
  if (schema.length() == 0) {
    RINGER_DEBUG1("I cannot find the standard schema path. Have you set"
		  << " RINGER_SCHEMA_PATH? Exception thrown.");
    throw RINGER_EXCEPTION("RINGER_SCHEMA_PATH not set");
  }
  schema += "/ring.xsd";
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
      //ignore
      continue;
    }
    if (it->type == XML_ELEMENT_NODE && 
	sys::get_element_name(it) == "config") {
      //loads the job layout
      for (xmlNodePtr jt=it->children; jt; jt=sys::get_next_element(jt)) {
	if (jt->type != XML_ELEMENT_NODE) continue;
	std::string name = sys::get_element_name(jt);
	if (jt->type == XML_ELEMENT_NODE && name == "set") {
	  typedef std::map<unsigned int, rbuild::RingConfig> map_type;
	  unsigned int id = sys::get_attribute_uint(jt, "id");
	  map_type::const_iterator it = m_config.find(id);
	  if ( it != m_config.end() ) { //oops, that one already exists!
	    //obs: this could be simplified when XML uniqueness start to
	    //work under libxml2. It didn't as version 2.6.8
	    RINGER_WARN(m_reporter, "RingSet with id=" << id << " already"
			<< " exists. Ignoring new entry.");
	    continue;
	  }
	  //ok, its a unique entry
	  m_config[id] = rbuild::RingConfig(jt);
	}
      }
    }
  }

  RINGER_DEBUG2("Ring configuration file \"" << filename << "\" has "
		<< m_config.size() << " valid entry(ies).");
  RINGER_DEBUG3("Configuration created from file \"" << filename << "\".");
}

rbuild::Config::~Config ()
{
}





