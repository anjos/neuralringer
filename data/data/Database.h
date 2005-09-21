//Dear emacs, this is -*- c++ -*-

/**
 * @file data/Database.h
 *
 * Loads a database in memory. The database file consists of a header
 * description and a set of entries each of which, contains one or more data
 * sets classified.
 */

#ifndef DATA_DATABASE_H
#define DATA_DATABASE_H

#include <string>
#include <map>
#include "data/Header.h"
#include "data/PatternOperator.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/util.h"
#include "sys/xmlutil.h"
#include "sys/debug.h"
#include "sys/XMLProcessor.h"
#include "data/Ensemble.h"
#include <cmath>

namespace data {

  /**
   * Builds a new database from an XML configuration file
   */
  template <class TSet> class Database {

  public:

    /**
     * Builds a new interface from a configuration file
     *
     * @param filename The name of the file to parse with the configuration
     * @param reporter The reporter to give to the configuration system
     */
    Database (const std::string& filename, sys::Reporter& reporter);

    /**
     * Builds a new database out of scratch parameters
     *
     * @param header This database header
     * @param data The PatternSets for this database, classified
     * @param reporter The reporter to give to the configuration system
     */
    Database (const data::Header* header,
	      const std::map<std::string, TSet*>& data,
	      sys::Reporter& reporter);

    /**
     * Copy constructor
     *
     * @param other The other database to copy data from
     */
    Database (const Database& other);

    /**
     * Destructor virtualisation
     */
    virtual ~Database();

    /**
     * Returns a class that represents the Header entity
     */
    inline const data::Header* header() const { return m_header; }

    /**
     * Returns the number of classes I have in this database.
     */
    inline size_t size() const { return m_data.size(); }

    /**
     * Returns the size of each pattern in this database
     */
    inline size_t pattern_size() const { return m_patsize; }

    /**
     * Returns the input data set for a particular class
     *
     * @param class_id The class identifier
     */
    inline const TSet* data (const std::string& class_id)
    { return m_data[class_id]; }

    /**
     * Returns all data sets
     */
    inline const std::map<std::string, TSet*>& data () const
    { return m_data; }

    /**
     * Fills in a vector with all my class names
     *
     * @param cn Where to place my class names for the caller
     */
    void class_names (std::vector<std::string>& cn);

    /**
     * Saves this database.
     *
     * @param filename The name of the file where to save this database.
     */
    bool save (const std::string& filename);

    /**
     * Merges this database in a single PatternSet
     *
     * @param ps The PatternSet to put the result to
     */
    void merge (TSet& ps) const;

    /**
     * Returns a PatternSet which express the class of each 'merged' Pattern
     * returned by merge(). There are two possible options "minimal"
     * representation, in which the number of ensembles will be minimised to
     * the closest power of two possibility. For example, if I have two
     * classes, only one ensemble will exist (<code>min</code> indicates the
     * value for the first class of Pattern's while <code>max</code> indicates
     * the value for the second class of Pattern's). The second possibility is
     * "normal" where each output will represent one and only one class. In
     * this case, looking at the targets, the maximum happening in ensemble 0,
     * indicates the Pattern's are from class 0, when the maximum happens at
     * ensemble 2, the Pattern's related belong to class 2, and on.
     *
     * @param minimal Do we use "minimal" representation? A value of
     * <code>false</code> imples we use the "normal" representation.
     * @param min The minimum value the reader can interpret (neuron off).
     * @param max The maximum value the reader can interpret (neuron on).
     * @param target The target PatternSet according to the parameters before
     */
    void merge_target (const bool minimal, const double& min,
		       const double& max, 
		       TSet& target) const;

    /**
     * Normalises the database contents with respect to its classes. This
     * process will calculate the number of Patterns in each class and will
     * concatenate each PatternSet (class) so each class has the same amount
     * of Pattern's.
     */
    void normalise (void);

    /**
     * This will split the Database in two components: train and test data,
     * according to the proportion given by the argument (which should be
     * greater than zero and less than the unity). The <b>new</b> databases
     * for training and testing are returned to the caller.
     *
     * @param prop The amount (between <i>0&lt;prop&lt;1</i>) of the data that
     * will be dedicated to training. If this amount is between 0 and -1, the
     * partition happens the contrary.
     * @param train The newly created training database
     * @param test The newly created testing database
     *
     * @return <code>true</code> if the split was successful,
     * <code>false</code> otherwise. This method can return <code>false</code>
     * in the case the value of prop is less or equal to 0 or greater or equal
     * to 1.
     */
    bool split (const double& prop, Database*& train, Database*& test) const;

    /**
     * This will shuffle all PatternSet's inside this database, randomly.
     */
    void shuffle ();

    /**
     * Applies the given Operator to all Patterns in all my PatternSet
     * classes.
     *
     * @param op The PatternOperator to apply
     */
    void apply_pattern_op (const data::PatternOperator& op);

    /**
     * Applies the given Operator to all Ensembles in all my PatternSet
     * classes.
     *
     * @param op The EnsembleOperator to apply
     */
    void apply_ensemble_op (const data::PatternOperator& op);

  private: //forbidden

    /**
     * Assignment
     */
    Database& operator= (const Database& other);

  private: //representation
    
    data::Header* m_header; ///< The header information for this database
    std::map<std::string, TSet*> m_data; ///< The data
    sys::Reporter& m_reporter; ///< The system reporter for errors/warnings
    size_t m_patsize; ///< The number of feature each pattern has

  };

}

/**
 * Dumps a database with a nice representation on a ostream
 *
 * @param os The output stream to use
 * @param db The database to dump
 */
template <class TSet>
std::ostream& operator<< (std::ostream& os, const data::Database<TSet>& db);

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
			   const double& min, const double& max);

//------------------------------
// Template Implementation
//------------------------------

template <class TSet>
data::Database<TSet>::Database (const std::string& filename, 
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
  
  sys::xml_ptr top = sys::get_first_child(root);
  //read header info
  m_header = new data::Header(top);
  top = sys::get_next_element(top);

  //for all classes
  for (sys::xml_ptr jt=sys::get_first_child(top); jt; 
       jt=sys::get_next_element(jt)) {
#ifndef XERCES_XML_BACK_END
    if (jt->type != XML_ELEMENT_NODE) continue;
#endif
    std::string name = sys::get_attribute_string(jt, "name");
    if (m_data.find(name) != m_data.end()) {
      RINGER_DEBUG1("Error! Class name \"" << name << "\" already exists!"
		    << " Exception thrown.");
      throw RINGER_EXCEPTION("Repeated DB class names");
    }

    RINGER_REPORT(m_reporter, "Loading entries for class \"" << name << "\".");
    //Instantiates a simple PatternSet, which is the most reasonable
    //assumption I can have in the lack of more information
    m_data[name] = new TSet(jt);
    RINGER_REPORT(m_reporter, "Database class \"" << name << "\" has "
		  << m_data[name]->size() << " entries.");
  }

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (typename std::map<std::string, TSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		      << " have a different pattern size ("
		      << it->second->pattern_size() << ") then the rest of"
		      << " the database (" << m_patsize 
		      <<"). Exception thrown.");
	//unwind
	delete m_header;
	for (typename std::map<std::string, TSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw RINGER_EXCEPTION
	  ("Uncoherent database (different pattern sizes)");
      }
    }
  }

  //check pattern sizes (not a simple way to do it with libxml2 as 2.6.16...
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (typename std::map<std::string, TSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		      << " have a different pattern size ("
		      << it->second->pattern_size() << ") then the rest of"
		      << " the database (" << m_patsize 
		      << "). Exception thrown.");
	//unwind
	delete m_header;
	for (typename std::map<std::string, TSet*>::const_iterator it 
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

template <class TSet>
data::Database<TSet>::Database (const data::Header* header, 
				const std::map<std::string, TSet*>& data,
				sys::Reporter& reporter)
  : m_header(0),
    m_data(),
    m_reporter(reporter),
    m_patsize(0)
{
  m_header = new data::Header(*header);
  for (typename std::map<std::string, TSet*>::const_iterator 
	 it = data.begin(); it != data.end(); ++it)
    m_data[it->first] = new TSet(*it->second);

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (typename std::map<std::string, TSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		      << " have a different pattern size ("
		      << it->second->pattern_size() << ") then the rest of"
		      << " the database (" << m_patsize 
		      << "). Exception thrown.");
	//unwind
	delete m_header;
	for (typename std::map<std::string, TSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw 
	  RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  RINGER_DEBUG3("Database created from scratch.");
}

template <class TSet>
data::Database<TSet>::Database (const Database& other)
  : m_header(0),
    m_data(),
    m_reporter(other.m_reporter),
    m_patsize(0)
{
  m_header = new data::Header(*other.m_header);
  for (typename std::map<std::string, TSet*>::const_iterator 
	 it = other.m_data.begin(); it != other.m_data.end(); ++it)
    m_data[it->first] = new TSet(*it->second);

  //check pattern sizes
  if (m_data.size() != 0) {
    m_patsize = m_data.begin()->second->pattern_size();
    for (typename std::map<std::string, TSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it) {
      if (m_patsize != it->second->pattern_size()) {
	RINGER_DEBUG1("PatternSet from class \"" << it->first << "\" seems to"
		      << " have a different pattern size ("
		      << it->second->pattern_size() << ") then the rest of"
		      << " the database (" << m_patsize 
		      << "). Exception thrown.");
	//unwind
	delete m_header;
	for (typename std::map<std::string, TSet*>::const_iterator it 
	       = m_data.begin(); it != m_data.end(); ++it) delete it->second;
	throw 
	  RINGER_EXCEPTION("Uncoherent database (different pattern sizes)");
      }
    }
  }

  RINGER_DEBUG3("Database created from copy.");
}

template <class TSet>
data::Database<TSet>::~Database() 
{
  delete m_header;
  for (typename std::map<std::string, TSet*>::const_iterator 
	 it = m_data.begin(); it != m_data.end(); ++it) delete it->second;
  RINGER_DEBUG3("Database destroyed.");
}

template <class TSet>
void data::Database<TSet>::class_names (std::vector<std::string>& cn)
{
  for (typename std::map<std::string, TSet*>::const_iterator 
	 it = m_data.begin(); it != m_data.end(); ++it) 
    cn.push_back(it->first);
}

template <class TSet>
bool data::Database<TSet>::save (const std::string& filename)
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
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    RINGER_DEBUG2("XML'ing class \"" << it->first << "\".");
    sys::put_node(data, it->second->dump(root, it->first, index));
    index += it->second->size();
  }
  sys::put_node(root, data);
  RINGER_DEBUG2("Finally saving file...");
  if (!xmlproc.write(filename)) return false;
  RINGER_DEBUG2("File \"" << filename << "\" was saved.");
  return true;
}

template <class TSet>
void data::Database<TSet>::merge (TSet& ps) const
{
  bool init = false;
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    if (!init) {
      ps = *it->second;
      init = true;
    }
    else ps.merge(*it->second);
  }
}

template <class TSet>
void data::Database<TSet>::merge_target (const bool minimal,
					 const double& min,
					 const double& max,
					 TSet& target) const
{
  //The next test probably characterizes this should be split in two
  //functions, but let's leave it this way for the time being
  if (minimal) {
    //Condense the targets to the minimal binary set possible
    size_t total_size = 0;
    for (typename std::map<std::string, TSet*>::const_iterator
	   it = m_data.begin(); it != m_data.end(); ++it)
      total_size += it->second->size();
    size_t next_power_of_2 = lrint(std::ceil(log2(m_data.size())));
    TSet my_target(total_size, next_power_of_2, min);
    size_t class_counter=0;
    size_t pattern_counter=0;
    for (typename std::map<std::string, TSet*>::const_iterator
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
      TSet tmp(m_data.begin()->second->size(), m_data.size(), min);
      data::Ensemble tmpe(m_data.begin()->second->size(), max);
      tmp.set_ensemble(0, tmpe);
      target = tmp;
    }
    if (m_data.size() > 1) {
      size_t i=1;
      bool init = false;
      for (typename std::map<std::string, TSet*>::const_iterator
	     it = m_data.begin(); it != m_data.end(); ++it) {
	TSet tmp(it->second->size(), m_data.size(), min);
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

template <class TSet>
void data::Database<TSet>::normalise (void)
{
  //get the greatest PatternSet size in this Database
  size_t greater = 0;
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) {
    if (it->second->size() > greater) greater = it->second->size();
  }

  //check who is 50% less and adjust it
  for (typename std::map<std::string, TSet*>::iterator
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
	TSet merge_this(*it->second, patnumber);
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
			<< ") than this DB's greatest size (" 
			<< greater << ")."
			<< " So, I'm increasing it by " << to_copy << ".");
	  std::vector<size_t> patnumber(to_copy);
	  for (size_t i=0; i<to_copy; ++i) patnumber[i] = i;
	  TSet merge_this(*it->second, patnumber);
	  it->second->merge(merge_this);
	}
      }
    }
  }
}

template <class TSet>
bool data::Database<TSet>::split (const double& prop, Database<TSet>*& train, 
				  Database<TSet>*& test) const
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
  std::map<std::string, TSet*> train_data;
  std::map<std::string, TSet*> test_data;
  for (typename std::map<std::string, TSet*>::const_iterator
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
    train_data[it->first] = new TSet(*it->second, patnumber);
    if (prop > 0) {
      patnumber.resize(it->second->size() - first_part);
      for (size_t i=first_part; i<it->second->size(); ++i)
	patnumber[i-first_part] = i;
    }
    else {
      patnumber.resize(first_part);
      for (size_t i=0; i<first_part; ++i) patnumber[i] = i;
    }
    test_data[it->first] = new TSet(*it->second, patnumber);
  }
  train = new data::Database<TSet>(&train_header, train_data, m_reporter);
  for (typename std::map<std::string, TSet*>::iterator
	 it = train_data.begin(); it != train_data.end(); ++it) 
    delete it->second;
  test = new data::Database<TSet>(&test_header, test_data, m_reporter);
  for (typename std::map<std::string, TSet*>::iterator
	 it = test_data.begin(); it != test_data.end(); ++it) 
    delete it->second;
  return true;
}

template <class TSet>
void data::Database<TSet>::shuffle ()
{
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) it->second->shuffle();
}

template <class TSet>
void data::Database<TSet>::apply_pattern_op (const data::PatternOperator& op)
{
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it) 
    it->second->apply_pattern_op(op);
}

template <class TSet>
void data::Database<TSet>::apply_ensemble_op (const data::PatternOperator& op)
{
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = m_data.begin(); it != m_data.end(); ++it)
    it->second->apply_ensemble_op(op);
}

template <class TSet>
std::ostream& operator<< (std::ostream& os, const data::Database<TSet>& db)
{
  for (typename std::map<std::string, TSet*>::const_iterator
	 it = db.data().begin(); it != db.data().end(); ++it)
    os << "Database class [" << it->first << "] is:" << std::endl
       << *it->second << std::endl;
  return os;
}


#endif /* DATA_DATABASE_H */
