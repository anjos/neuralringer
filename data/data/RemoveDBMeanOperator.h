//Dear emacs, this is -*- c++ -*-

/**
 * @file data/RemoveDBMeanOperator.h
 *
 * @brief This operator removes the mean of a Pattern.
 */

#ifndef DATA_REMOVEDBMEANOPERATOR_H
#define DATA_REMOVEDBMEANOPERATOR_H

#include "data/PatternOperator.h"
#include "data/Database.h"

namespace data {

  class RemoveDBMeanOperator : public PatternOperator {

  public:

    /**
     * Constructor. To remove the mean of ensembles in a DB, I have to compute
     * first the mean for all classes in a DB.
     *
     * @param db The database to extract the ensemble mean from
     */
    RemoveDBMeanOperator(const data::Database& db);

    /**
     * Destructor virtualisation
     */
    virtual ~RemoveDBMeanOperator() {}

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

    data::Pattern m_mean; ///< The mean for the DB in question

  };

}

#endif /* DATA_REMOVEDBMEANOPERATOR_H */
