//Dear emacs, this is -*- c++ -*-

/**
 * @file MaxExtractor.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the minima extractor for a Pattern.
 */

#include "data/MinExtractor.h"

data::Feature data::MinExtractor::operator() (const data::Pattern& in) const
{
  return gsl_vector_min(abuse(in));
}



