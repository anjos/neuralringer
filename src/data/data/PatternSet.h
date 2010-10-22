//Dear emacs, this is -*- c++ -*-

/**
 * @file data/PatternSet.h
 *
 * @brief Declares the PatternSet class.
 */

#ifndef DATA_PATTERNSET_H
#define DATA_PATTERNSET_H

#include <gsl/gsl_matrix.h>
#include <iostream>
#include "data/Pattern.h"
#include "data/PatternOperator.h"
#include "data/Ensemble.h"
#include "sys/Reporter.h"
#include "sys/File.h"
#include "sys/xmlutil.h"
#include "data/PatternSet.h"

namespace data {

  /**
   * This class represents a set of data::Pattern's.
   *
   * A <b>data::Pattern set</b> is an entity that holds, virtually, any number
   * of data::Pattern's. The set can be queried for special methods, allows
   * random access with a reasonable speed and data::Pattern's to be inserted
   * and removed from it. Some sort of normalisation strategies are also
   * possible. A PatternSet can also be used to produce smaller
   * <b>sets</b> that match a certain criteria. This class and related methods
   * are strongly based on the GNU Scientific Library (GSL)
   * vector/matrix/block objects. Please, refer to the GSL manual to
   * understand better its limitations and virtudes (try at your shell
   * <code>info gsl</code> or <code>info gsl-ref</code>).
   *
   * While data::Pattern's represent each event available in a
   * PatternSet, a data::Ensemble represents a given Feature for every
   * event in a PatternSet. Through this abstraction, it is possible to
   * also manipulate a PatternSet with respect to its "columns", erasing
   * or setting them.
   *
   * All types developed to be used in conjunction with this class may be
   * derived types through the use of GSL's views of blocks, matrices and
   * vectors.
   */
  class PatternSet {

  public: //construction and destruction

    /**
     * Creates an empty PatternSet with given sizes and an optional
     * initialization value.
     *
     * @param size The number of Patterns inside of the PatternSet.
     * @param p_size The size of each Pattern inside the PatternSet.
     * @param init An optional initialization value
     */
    PatternSet(const size_t& size, const size_t& p_size, 
		     const double& init=0);

    /** 
     * Creates a PatternSet from another PatternSet. This is the
     * copy constructor.
     *
     * @param other The PatternSet to be cloned.
     */
    PatternSet(const PatternSet& other);

    /** 
     * Reads a PatternSet from an XML file node
     *
     * @param node The root node where I'm in
     */
    PatternSet(sys::xml_ptr_const node);

    /**
     * Creates a PatternSet from another PatternSet, by selecting
     * a set of patterns of interest.
     *
     * @param other The PatternSet to copy data from
     * @param pats The set of patterns to take from the original set.
     */
    PatternSet(const PatternSet& other,
		     const std::vector<size_t>& pats);

    /**
     * Creates a PatternSet from a set of Patterns, litterally
     *
     * @param pats The set of patterns to use for building this set.
     */
    PatternSet(const std::vector<Pattern*>& pats);

    /**
     * The default destructor.
     */
    virtual ~PatternSet();

  public: //other interfaces

    /**
     * This method returns the set size, i.e., the number of data::Pattern's
     * it contains.
     */
    size_t size () const;

    /**
     * Returns the current size of each Pattern on the set.
     */
    size_t pattern_size () const;

    /**
     * This method returns a constant reference of the data::Pattern required,
     * checking the range of the set before returning, by value, the required
     * data::Pattern. It's an error to address a non-existing position inside
     * the set. The data::Pattern is <b>not</b> created by data copying.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>
     */
    const Pattern pattern (const size_t& pos) const;

    /**
     * This method returns a constant reference of the data::Ensemble
     * required, checking the range of the set before returning, by value, the
     * required data::Ensemble. It's an error to address a non-existing
     * position inside the set. The data::Ensemble is <b>not</b> created by
     * data copying.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>
     */
    const Ensemble ensemble (const size_t& pos) const;

    /**
     * This method sets a specific data::Pattern inside the set to a new
     * value, also given as parameter. The new data::Pattern is checked for
     * size compatibility with the old one. If both have the same size, the
     * value is assigned, otherwise, the operation is <b>not</b>
     * performed. The range is also checked.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>.
     * @param pat The new value of the data::Pattern
     */
    void set_pattern (const size_t& pos, const Pattern& pat);

    /**
     * This method sets a specific data::Ensemble inside the set to a new
     * value, also given as parameter. The new data::Ensemble is checked for
     * size compatibility with the old one. If both have the same size, the
     * value is assigned, otherwise, the operation is <b>not</b>
     * performed. The range is also checked.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>.
     * @param ens The new value of the data::Ensemble
     */
    void set_ensemble (const size_t& pos, const Ensemble& ens);

    /**
     * This method deletes a data::Pattern from the set. It's an error to call
     * this method with a data::Pattern number that doesn't exist. It takes
     * some computation time to delete() a Pattern from a set, so be sure
     * before doing it.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>
     */
    void erase_pattern (const size_t& pos);

    /**
     * This method deletes a data::Ensemble from the set. It's an error to
     * call this method with a data::Ensemble number that doesn't exist. It
     * takes some computation time to delete() an Ensemble from a set, so be
     * sure before doing it.
     *
     * @param pos The relative position inside the set, starting from
     * <code>0</code>
     */
    void erase_ensemble (const size_t& pos);

    /**
     * Makes a copy of this PatternSet in dynamic memory
     */
    PatternSet* clone (void) const;

    /**
     * Makes a selective copy of this PatternSet in dynamic memory
     *
     * @param pats The set of patterns to take from the original set.
     */
    PatternSet* clone (const std::vector<size_t>& pats) const;

    /**
     * Shuffles the order of data inside this PatternSet.
     */
    void shuffle (void);

    /**
     * Dumps the set as a set of XML nodes
     *
     * @param any Any node in the XML tree.
     * @param cname The class name to use when dumping
     * @param start_id The initial number to take in consideration when
     * writing the entry identifiers.
     */
    sys::xml_ptr dump (sys::xml_ptr any,
			       const std::string& cname,
			       const size_t start_id=0) const;

    /**
     * Applies the given PatternOperator to all my Pattern's.
     *
     * @param op The operator to apply
     */
    void apply_pattern_op (const data::PatternOperator& op);

    /**
     * Applies the given PatternOperator to all my Ensemble's.
     * 
     * @param op The operator to apply
     */
    void apply_ensemble_op (const data::PatternOperator& op);

    /**
     * Dumps a pattern set with a nice representation on a ostream
     *
     * @param os The output stream to use
     */
    std::ostream& stream_out (std::ostream& os) const;
	
    /**
     * Dumps a pattern set with a nice representation on a ostream
     *
     * @param f The output stream to use
     */
    sys::File& stream_out (sys::File& f) const;

  public: //particular class interfaces

    /**
     * Merges two distinct PatternSet's.
     *
     * This method will copy the given PatternSet Pattern's into the
     * current set, enlarging it. We check if the Pattern sizes are the same
     * previous to the copying. This method returns a reference to the current
     * set being manipulated.
     *
     * @param other The PatternSet to be copied
     */
    PatternSet& merge (const PatternSet& other);

    /**
     * Sets this PatternSet starting from another PatternSet, by
     * selecting a set of patterns of interest.
     *
     * @param other The PatternSet to copy data from 
     * @param pats The set of patterns to take from the original set.
     */
    PatternSet& assign(const PatternSet& other,
			     const std::vector<size_t>& pats);

    /**
     * Reset the PatternSet from a set of Patterns, litterally
     *
     * @param pats The set of patterns to use for building this set.
     */
    PatternSet& operator= (const std::vector<Pattern*>& pats);

    /**
     * This method defines how to copy a PatternSet.
     *
     * @param other The PatternSet to be copied.
     */
    PatternSet& operator= (const PatternSet& other);

    /**
     * Subtracts, from this PatternSet, the value given
     *
     * @param other The PatternSet to be used in the operation.
     */
    PatternSet& operator-= (const PatternSet& other);

  private: //representation
    gsl_matrix* m_data; ///< my internal data
    
  };
  
}

/**
 * Dumps a pattern set with a nice representation on a ostream
 *
 * @param os The output stream to use
 * @param p The set to dump
 */
inline std::ostream& operator<< (std::ostream& os, const data::PatternSet& p)
{ return p.stream_out(os); }

/**
 * Dumps a pattern set with a nice representation on a ostream
 *
 * @param f The output sys::File to use
 * @param p The set to dump
 */
inline sys::File& operator<< (sys::File& f, const data::PatternSet& p)
{ return p.stream_out(f); }

#endif //DATA_PATTERNSET_H
