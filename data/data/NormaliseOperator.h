//Dear emacs, this is -*- c++ -*-

/**
 * @file data/NormaliseOperator.h
 *
 * @brief This operator removes the mean of a Pattern.
 */

#ifndef DATA_NORMALISEOPERATOR_H
#define DATA_NORMALISEOPERATOR_H

#include "data/PatternOperator.h"
#include "data/Database.h"

namespace data {

  class NormaliseOperator : public PatternOperator {

  public:

    /**
     * Constructor. To normalise ensembles in a DB, I have to compute
     * first the maximum for all classes in a DB.
     *
     * @param db The database to extract the ensemble mean from
     */
    NormaliseOperator(const data::Database& db);

    /**
     * Destructor virtualisation
     */
    virtual ~NormaliseOperator() {}

    /**
     * Generates a Pattern with the same length as the previous, but with the
     * mean removed from each Pattern feature.
     *
     * @param in The Pattern to transform using this operator
     * @param out The Pattern to output
     */
    virtual void operator() (const data::Pattern& in,
			     data::Pattern& out) const;

  private: //representation

    data::Pattern m_max; ///< The maximum for each ensemble of the DB used

  };

}

#endif /* DATA_NORMALISEOPERATOR_H */
