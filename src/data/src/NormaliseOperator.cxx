//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/NormaliseOperator.cxx
 *
 * Implements the Mean Removal operator for Databases.
 */

#include "data/NormaliseOperator.h"

data::NormaliseOperator::NormaliseOperator(const data::Database& db)
  : m_max(db.pattern_size(),0)
{
  data::PatternSet ps(1,1);
  db.merge(ps);
  for (unsigned int i=0; i<ps.pattern_size(); ++i) { //for all ensembles
    data::Ensemble e = ps.ensemble(i);
    m_max[i] = fabs(gsl_vector_max(abuse(e)));
    data::Feature min = fabs(gsl_vector_min(abuse(e)));
    if (min > m_max[i]) m_max[i] = min;
    RINGER_DEBUG3("Database absolute maximum for ensemble[" 
		<< i << "] is " << m_max[i]);
  }
}

void data::NormaliseOperator::operator() (const data::Pattern& in, 
					  data::Pattern& out) const
{
  out = in / m_max;
}

