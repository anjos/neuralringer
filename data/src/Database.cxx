//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/Database.cxx
 *
 * Implements the database readout and saving.
 */

#include "data/Database.h"
#include "sys/Exception.h"
#include "sys/util.h"
#include "sys/xmlutil.h"
#include "sys/debug.h"
#include "sys/XMLProcessor.h"
#include <cmath>

data::Database::Database (const std::string& filename, 
			  sys::Reporter& reporter)
  : m_header(0),
    m_data(),
    m_reporter(reporter),
    m_patsize(0)
{
  std::string schema = sys::getenv("RINGER_SCHEMA_PATH");
  if (schema.length() == 0) {
    RINGER_DEBUG1("I cannot find the standard schema path. Have you set"
		<< " RINGER_SCHEMA_PATH? Exception thrown.");
    throw RINGER_EXCEPTION("RINGER_SCHEMA_PATH not set");
  }
  schema += "/database.xsd";
  sys::XMLProcessor xmlproc(schema, m_reporter);

  RINGER_DEBUG2("Trying to parse " << filename);
  xmlNodePtr root = xmlproc.read(filename);
  if (!root) {
    RINGER_WARN(m_reporter, "XML database file " << filename 
	      << " cannot be parsed. Exception thrown.");
    throw RINGER_EXCEPTION("Cannot parse XML database file");
  }
  
  xmlNodePtr top = root->children;
  if (top->type != XML_ELEMENT_NODE) top = sys::get_next_element(top);
  //read header info
  m_header = new data::Header(top);
  top = sys::get_next_element(top);

  //for all classes
  for (xmlNodePtr jt=top->children; jt; jt=sys::get_next_element(jt)) {
    if (jt->type != XML_ELEMENT_NODE) continue;
    std::string name = sys::get_attribute_string(jt, "name");
    if (m_data.find(name) != m_data.end()) {
       RINGER_DEBUG1("Error! Class name \"" << name << "\" already exists!"
		   << " Exception thrown.");
       throw RINGER_EXCEPTION("Repeated DB class names");
    }
    RINGER_REPORT(m_reporter, 
		  "Loading entries for class \"" << name << "\".");
    m_data[name] = new data::PatternSet(jt);
    RINGER_REPORT(m_reporter, "Database class \"" << name << "\" has "
		  << m_data[name]->size() << " entries.");
  }

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		    << " have a different pattern size ("
		    << it->second->pattern_size() << ") then the rest of"
		    << " the database (" << m_patsize 
		    <<"). Exception thrown.");
	//unwind
	delete m_header;
	for (std::map<std::string, data::PatternSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		    << " have a different pattern size ("
		    << it->second->pattern_size() << ") then the rest of"
		    << " the database (" << m_patsize 
		    << "). Exception thrown.");
	//unwind
	delete m_header;
	for (std::map<std::string, data::PatternSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  RINGER_DEBUG2("Database file \"" << filename << "\" has " << m_data.size()
	      << " classes.");
  RINGER_DEBUG3("Database created from file \"" << filename << "\".");
}

data::Database::Database (const data::Header* header, 
			  const std::map<std::string, data::PatternSet*>& data,
			  sys::Reporter& reporter)
: m_header(0),
  m_data(),
  m_reporter(reporter),
  m_patsize(0)
{
  m_header = new data::Header(*header);
  for (std::map<std::string, data::PatternSet*>::const_iterator 
	 it = data.begin(); it != data.end(); ++it)
    m_data[it->first] = new data::PatternSet(*it->second); //copy

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		    << " have a different pattern size ("
		    << it->second->pattern_size() << ") then the rest of"
		    << " the database (" << m_patsize 
		    << "). Exception thrown.");
	//unwind
	delete m_header;
	for (std::map<std::string, data::PatternSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw 
	  RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  RINGER_DEBUG3("Database created from scratch.");
}

data::Database::Database (const Database& other)
: m_header(0),
  m_data(),
  m_reporter(other.m_reporter),
  m_patsize(0)
{
  m_header = new data::Header(*other.m_header);
  for (std::map<std::string, data::PatternSet*>::const_iterator 
	 it = other.m_data.begin(); it != other.m_data.end(); ++it)
    m_data[it->first] = new data::PatternSet(*it->second);

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		    << " have a different pattern size ("
		    << it->second->pattern_size() << ") then the rest of"
		    << " the database (" << m_patsize 
		    << "). Exception thrown.");
	//unwind
	delete m_header;
	for (std::map<std::string, data::PatternSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw 
	  RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  RINGER_DEBUG3("Database created from copy.");
}

data::Database::~Database() 
{
  delete m_header;
  for (std::map<std::string, data::PatternSet*>::const_iterator 
	 it = m_data.begin(); it != m_data.end(); ++it) delete it->second;
  RINGER_DEBUG3("Database destroyed.");
}

void data::Database::class_names (std::vector<std::string>& cn)
{
  for (std::map<std::string, data::PatternSet*>::const_iterator 
	 it = m_data.begin(); it != m_data.end(); ++it) 
    cn.push_back(it->first);
}

bool data::Database::save (const std::string& filename)
{
  RINGER_DEBUG2("Trying to save database at \"" << filename << "\".");
  std::string schema = sys::getenv("RINGER_SCHEMA_PATH");
  if (schema.length() == 0) {
    RINGER_DEBUG1("I cannot find the standard schema path. Have you set"
		<< " RINGER_SCHEMA_PATH? Exception thrown.");
    throw RINGER_EXCEPTION("RINGER_SCHEMA_PATH not set");
  }
  schema += "/database.xsd";
  sys::XMLProcessor xmlproc(schema, m_reporter);
  xmlNodePtr root = xmlproc.new_document("database");
  sys::put_attribute_text(root, "version", "0.1");
  xmlAddChild(root, m_header->node());
  xmlNodePtr data = sys::put_element(root, "data");
  size_t index = 0;
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    RINGER_DEBUG2("XML'ing class \"" << it->first << "\".");
    xmlAddChild(data, it->second->dump(it->first, index));
    index += it->second->size();
  }
  xmlAddChild(root, data);
  RINGER_DEBUG2("Finally saving file...");
  if (!xmlproc.write(filename)) return false;
  RINGER_DEBUG2("File \"" << filename << "\" was saved.");
  return true;
}

void data::Database::merge (data::PatternSet& ps) const
{
  bool init = false;
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    if (!init) {
      ps = *it->second;
      init = true;
    }
    else ps.merge(*it->second);
  }
}

/**
 * Returns a Pattern which represents 'n' in binary with the maximum and
 * minimum set by the available parameters. The length of the returned Pattern
 * is represented by 's'
 *
 * @param n The binary to represent
 * @param s The size of the Pattern to return
 * @param min The representation of "zero" to use
 * @param max The representation of "one" to use
 */
data::Pattern make_target (const size_t n, const size_t s,
			   const double& min, const double& max)
{
  size_t next_power_of_2 = lrint(std::ceil(log2(n+1)));
  if (s < next_power_of_2) {
    RINGER_DEBUG1("I cannot represent \"" << n << "\" in binary in a vector of"
		<< " size \"" << s << "\". I would need at least \""
		<< next_power_of_2 << "\" positions. Exception thrown.");
    throw RINGER_EXCEPTION("Unproper Pattern length on binary representation");
  }
  data::Pattern retval(s, min);
  size_t v = n;
  for (size_t i=0; i<next_power_of_2; ++i) {
    if (v%2) retval[i] = max;
    v = v/2;
  }
  return retval;
}

void data::Database::merge_target (const bool minimal,
				   const double& min,
				   const double& max,
				   data::PatternSet& target) const
{
  //The next test probably characterizes this should be split in two
  //functions, but let's leave it this way for the time being
  if (minimal) {
    //Condense the targets to the minimal binary set possible
    size_t total_size = 0;
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it)
      total_size += it->second->size();
    size_t next_power_of_2 = lrint(std::ceil(log2(m_data.size())));
    data::PatternSet my_target(total_size, next_power_of_2, min);
    size_t class_counter=0;
    size_t pattern_counter=0;
    for (std::map<std::string, data::PatternSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      for (size_t j=0; j<it->second->size(); ++j) {
	data::Pattern curr_target = make_target(class_counter, 
						next_power_of_2,
						min, max);
	my_target.set_pattern(pattern_counter, curr_target);
	++pattern_counter;
      }
      ++class_counter;
    }
    target = my_target;
  }
  else {
    if (m_data.size() >= 1) {
      data::PatternSet tmp(m_data.begin()->second->size(), m_data.size(), min);
      data::Ensemble tmpe(m_data.begin()->second->size(), max);
      tmp.set_ensemble(0, tmpe);
      target = tmp;
    }
    if (m_data.size() > 1) {
      size_t i=1;
      bool init = false;
      for (std::map<std::string, data::PatternSet*>::const_iterator
	     it = m_data.begin(); it != m_data.end(); ++it) {
	data::PatternSet tmp(it->second->size(), m_data.size(), min);
	data::Ensemble tmpe(it->second->size(), max);
	tmp.set_ensemble(i++, tmpe);
	if (!init) {
	  target = tmp;
	  init = true;
	}
	else target.merge(tmp);
      }
    }
  }
}

void data::Database::normalise (void)
{
  //get the greatest PatternSet size in this Database
  size_t greater = 0;
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    if (it->second->size() > greater) greater = it->second->size();
  }

  //check who is 50% less and adjust it
  for (std::map<std::string, data::PatternSet*>::iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    if (it->second->size() < 0.9*greater) {
      //increase its size by copying the Patterns all over
      if (it->second->size() > 0.5*greater) { //only partial copy
	size_t to_copy = greater - it->second->size();
	RINGER_DEBUG2("Class \"" << it->first 
		      << "\" is more than 10% smaller ("
		      << it->second->size() 
		      << ") than this DB's greatest size (" << greater << "),"
		      << " but it is still more than 50% of that size."
		      << " So, I'm increasing it by " << to_copy << ".");
	std::vector<size_t> patnumber(to_copy);
	for (size_t i=0; i<to_copy; ++i) patnumber[i] = i;
	PatternSet merge_this(*it->second, patnumber);
	it->second->merge(merge_this);
      }
      else { //first copy N times the PatternSet
	size_t n_times = lrint(std::floor(log2(greater/it->second->size())));
	RINGER_DEBUG2("Class \"" << it->first 
		      << "\" is more than 10% smaller ("
		      << it->second->size() 
		      << ") than this DB's greatest size (" << greater << ")."
		      << " I'll need to copy it over at least "
		      << n_times << " times.");
	for (size_t i=0; i<n_times; ++i) it->second->merge(*it->second);
	//if we are still bellow 10% difference
	if (it->second->size() < 0.9*greater) {
	  size_t to_copy = greater - it->second->size();
	  RINGER_DEBUG2("Class \"" << it->first 
		      << "\" is still more than 10% smaller (" 
		      << it->second->size() 
		      << ") than this DB's greatest size (" << greater << ")."
		      << " So, I'm increasing it by " << to_copy << ".");
	  std::vector<size_t> patnumber(to_copy);
	  for (size_t i=0; i<to_copy; ++i) patnumber[i] = i;
	  PatternSet merge_this(*it->second, patnumber);
	  it->second->merge(merge_this);
	}
      }
    }
  }
}

bool data::Database::split (const double& prop, Database*& train, 
			    Database*& test) const
{
  if (prop <= -1 || prop >= 1) return false;
  double prop_use = std::fabs(prop);
  RINGER_DEBUG2("Splitting database by " << prop_use*100 << "%.");
  std::string name = m_header->name();
  data::Header train_header(m_header->author(), name + " (TRAIN)",
			    m_header->version(), m_header->created(),
			    m_header->comment());
  data::Header test_header(m_header->author(), name + " (TEST)",
			   m_header->version(), m_header->created(),
			   m_header->comment());
  std::map<std::string, data::PatternSet*> train_data;
  std::map<std::string, data::PatternSet*> test_data;
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    size_t first_part = lrint(it->second->size() * prop_use);
    RINGER_DEBUG2("PatternSet for class \"" << it->first << "\" will provide "
		<< first_part << " patterns and "
		<< it->second->size() - first_part << " patterns.");
    std::vector<size_t> patnumber(first_part);
    if (prop > 0)
      for (size_t i=0; i<first_part; ++i) patnumber[i] = i;
    else {
      patnumber.resize(it->second->size() - first_part);
      for (size_t i=first_part; i<it->second->size(); ++i)
	patnumber[i-first_part] = i;
    }
    train_data[it->first] = new data::PatternSet(*it->second, patnumber);
    if (prop > 0) {
      patnumber.resize(it->second->size() - first_part);
      for (size_t i=first_part; i<it->second->size(); ++i)
	patnumber[i-first_part] = i;
    }
    else {
      patnumber.resize(first_part);
      for (size_t i=0; i<first_part; ++i) patnumber[i] = i;
    }
    test_data[it->first] = new data::PatternSet(*it->second, patnumber);
  }
  train = new data::Database(&train_header, train_data, m_reporter);
  test = new data::Database(&test_header, test_data, m_reporter);
  return true;
}

void data::Database::shuffle ()
{
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) it->second->shuffle();
}

void data::Database::apply_pattern_op (const data::PatternOperator& op)
{
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) 
    it->second->apply_pattern_op(op);
}

void data::Database::apply_ensemble_op (const data::PatternOperator& op)
{
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it)
    it->second->apply_ensemble_op(op);
}

std::ostream& operator<< (std::ostream& os, const data::Database& db)
{
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = db.data().begin(); it != db.data().end(); ++it)
    os << "Database class [" << it->first << "] is:" << std::endl
       << *it->second << std::endl;
  return os;
}
