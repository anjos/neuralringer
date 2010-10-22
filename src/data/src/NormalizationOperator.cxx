//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/NormalizationOperator.cxx
 *
 * Implements the Mean Removal operator for Databases.
 */

#include "data/NormalizationOperator.h"

data::NormalizationOperator::NormalizationOperator (const data::Database& db)
  : m_mean(db.pattern_size(),0),
    m_sd(db.pattern_size(),1)
{
  data::PatternSet ps(1,1);
  db.merge(ps);
  for (unsigned int i=0; i<ps.pattern_size(); ++i) { //for all ensembles
    data::Ensemble e = ps.ensemble(i);
    const gsl_vector* v = abuse(e);
    m_mean[i] = gsl_stats_mean(v->data, v->stride, v->size);
    m_sd[i] = gsl_stats_sd(v->data, v->stride, v->size);
    if (m_sd[i] < 1e-5) m_sd[i] = 1; ///to prevent overflowing...
    RINGER_DEBUG1("Database mean for ensemble[" << i << "] is " << m_mean[i]);
    RINGER_DEBUG1("Database standard deviation for ensemble[" << i 
		  << "] is " << m_sd[i]);
  }
}

void data::NormalizationOperator::operator() (const data::Pattern& in, 
					      data::Pattern& out) const
{
  out = in - m_mean;
  out /= m_sd;
}
