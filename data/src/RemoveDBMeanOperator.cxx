//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/RemoveDBMeanOperator.cxx
 *
 * Implements the Mean Removal operator for Databases.
 */

#include "data/RemoveDBMeanOperator.h"
#include "sys/debug.h"
#include <gsl/gsl_statistics_double.h>

data::RemoveDBMeanOperator::RemoveDBMeanOperator(const data::Database& db)
  : m_mean(db.pattern_size(),0)
{
  data::PatternSet ps(1,1);
  db.merge(ps);
  for (unsigned int i=0; i<ps.pattern_size(); ++i) { //for all ensembles
    data::Ensemble e = ps.ensemble(i);
    const gsl_vector* v = abuse(e);
    m_mean[i] = gsl_stats_mean(v->data, v->stride, v->size);
    RINGER_DEBUG3("Database mean for ensemble[" << i << "] is " << m_mean[i]);
  }
}

void data::RemoveDBMeanOperator::operator() (const data::Pattern& in, 
					     data::Pattern& out) const
{
  out = in - m_mean;
}
