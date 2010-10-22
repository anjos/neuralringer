//Dear emacs, this is -*- c++ -*-

/**
 * @file data/NormalizationOperator.h
 *
 * @brief This operator removes the mean of a Pattern.
 */

#ifndef DATA_NORMALIZATIONOPERATOR_H
#define DATA_NORMALIZATIONOPERATOR_H

#include "data/PatternOperator.h"
#include "data/Database.h"
#include "sys/debug.h"
#include <gsl/gsl_statistics_double.h>

namespace data {

  class NormalizationOperator : public PatternOperator {

  public:

    /**
     * Constructor. To remove the mean and variance of ensembles in a DB, I
     * have to compute first the mean for all classes in a DB.
     *
     * @param db The database to extract the ensemble mean from
     */
    NormalizationOperator(const data::Database& db);

    /**
     * Destructor virtualisation
     */
    virtual ~NormalizationOperator() {}

    /**
     * Generates a Pattern with the same length as the previous, but with the
     * mean removed and divided by the variance from each Pattern feature.
     *
     * @param in The Pattern to transform using this operator
     * @param out The Pattern to output
     */
    virtual void operator() (const data::Pattern& in,
			     data::Pattern& out) const;

    /**
     * Returns a handle to the means of all ensembles in the Database
     */
    inline const data::Pattern& mean (void) const { return m_mean; }

    /**
     * Returns a handle to the standard deviations of all ensembles in the
     * Database. 
     */
    inline const data::Pattern& stddev (void) const { return m_sd; }

  private: //representation

    data::Pattern m_mean; ///< The mean for the DB in question
    data::Pattern m_sd; ///< The standard deviation for the DB in question

  };

}

#endif /* DATA_NORMALIZATIONOPERATOR_H */
