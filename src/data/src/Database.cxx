//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/Database.cxx
 *
 * Implements the database readout and saving.
 */
#include "data/Database.h"

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
  sys::xml_ptr root = xmlproc.read(filename);
  if (!root) {
    RINGER_WARN(m_reporter, "XML database file " << filename
		<< " cannot be parsed. Exception thrown.");
    throw RINGER_EXCEPTION("Cannot parse XML database file");
  }
  
  sys::xml_ptr_const top = sys::get_first_child(root);
  //read header info
  m_header = new data::Header(top);
  top = sys::get_next_element(top);

  //for all classes
  for (sys::xml_ptr_const jt=sys::get_first_child(top); jt; 
       jt=sys::get_next_element(jt)) {
    if (!sys::is_element(jt)) continue;
    std::string name = sys::get_attribute_string(jt, "name");
    if (m_data.find(name) != m_data.end()) {
      RINGER_DEBUG1("Error! Class name \"" << name << "\" already exists!"
		    << " Exception thrown.");
      throw RINGER_EXCEPTION("Repeated DB class names");
    }

    RINGER_REPORT(m_reporter, "Loading entries for class \"" << name << "\".");
    //Instantiates a simple data::PatternSet, which is the most reasonable
    //assumption I can have in the lack of more information
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
	throw RINGER_EXCEPTION
	  ("Uncoherent database (different pattern sizes)");
      }
    }
  }

  //check pattern sizes (not a simple way to do it with libxml2 as 2.6.16...
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
	throw RINGER_EXCEPTION
	  ("Uncoherent database (different pattern sizes)");
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
  for (std::map<std::string, data::Pattern*>::const_iterator
      it = m_target.begin(); it != m_target.end(); ++it) delete it->second;
  RINGER_DEBUG3("Database destroyed.");
}

void data::Database::class_names (std::vector<std::string>& cn) const
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
  sys::xml_ptr root = xmlproc.new_document("database");
  sys::put_attribute_text(root, "version", "0.1");
  sys::put_node(root, m_header->node(root));
  sys::xml_ptr data = sys::put_element(root, "data");
  size_t index = 0;
  for (std::map<std::string, data::PatternSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    RINGER_DEBUG2("XML'ing class \"" << it->first << "\".");
    sys::put_node(data, it->second->dump(root, it->first, index));
    index += it->second->size();
  }
  sys::put_node(root, data);
  RINGER_DEBUG2("Finally saving file...");
  if (!xmlproc.write(root, filename)) return false;
  RINGER_DEBUG2("File \"" << filename << "\" was saved.");
  return true;
}

void data::Database::merge (data::PatternSet& dest) const
{
  bool init = false;
  for (std::map<std::string, data::PatternSet*>::const_iterator
        it = m_data.begin(); it != m_data.end(); ++it) {
    if (!init) {
      dest = *it->second;
      init = true;
    }
    else dest.merge(*it->second);
  }
}

void data::Database::set_target
(const std::map<std::string, data::Pattern*>& targets) {
  if (targets.size() != m_data.size()) 
    throw RINGER_EXCEPTION("set_target: # of classes loaded is different");
  m_target.clear();
  std::map<std::string, data::Pattern*>& t2 = const_cast<std::map<std::string, data::Pattern*>&>(targets);
  for (std::map<std::string, data::PatternSet*>::const_iterator 
      it = m_data.begin(); it != m_data.end(); ++it) {
    m_target[it->first] = new data::Pattern(*t2[it->first]);
  }
}

void data::Database::merge_target (data::PatternSet& target) const {
  if (not m_target.size()) 
    throw RINGER_EXCEPTION("merge_target: targets are not set");
  std::vector<data::Pattern*> dp;
  std::map<std::string, data::Pattern*>& t2 = const_cast<std::map<std::string, data::Pattern*>&>(m_target);
  for (std::map<std::string, data::PatternSet*>::const_iterator
      it = m_data.begin(); it != m_data.end(); ++it) { //class iteration
    for (size_t j=0; j<it->second->size(); ++j) //pattern iteration
      dp.push_back(t2[it->first]);
  }
  target = dp;
}

void data::Database::normalise (void) {
  //get the greatest data::PatternSet size in this Database
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
      else { //first copy N times the data::PatternSet
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
			<< ") than this DB's greatest size (" 
			<< greater << ")."
			<< " So, I'm increasing it by " << to_copy << ".");
	  std::vector<size_t> patnumber(to_copy);
	  for (size_t i=0; i<to_copy; ++i) patnumber[i] = i;
	  data::PatternSet merge_this(*it->second, patnumber);
	  it->second->merge(merge_this);
	}
      }
    }
  }
}

bool data::Database::split (const double& prop, Database*& train, Database*& test) const
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
  for (std::map<std::string, data::PatternSet*>::iterator
	 it = train_data.begin(); it != train_data.end(); ++it) 
    delete it->second;
  test = new data::Database(&test_header, test_data, m_reporter);
  for (std::map<std::string, data::PatternSet*>::iterator
	 it = test_data.begin(); it != test_data.end(); ++it) 
    delete it->second;
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
