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

#include "sys/Reporter.h"
#include "data/Header.h"
#include "data/PatternSet.h"
#include "data/PatternOperator.h"
#include <string>
#include <map>

namespace data {

  /**
   * Builds a new database from an XML configuration file
   */
  class Database {

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
	      const std::map<std::string, data::PatternSet*>& data,
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
    inline const data::PatternSet* data (const std::string& class_id) 
    { return m_data[class_id]; }

    /**
     * Returns all data sets
     */
    inline const std::map<std::string, data::PatternSet*>& data () const
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
    void merge (data::PatternSet& ps) const;

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
		       const double& max, data::PatternSet& target) const;

    /**
     * Normalises the database contents with respect to its classes. This
     * process will calculate the number of Patterns in each class and will
     * concatenate each PatternSet (class) so each class has the same amount
     * of Pattern's.
     */
    void normalise (void);

    /**
     * This will split the Database in two components: train and test
     * data, according to the proportion given by the argument (which should
     * be greater than zero and less than the unity). The <b>new</b> databases
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
    std::map<std::string, data::PatternSet*> m_data; ///< The data
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
std::ostream& operator<< (std::ostream& os, const data::Database& db);

#endif /* DATA_DATABASE_H */
