//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/MaxExtractor.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the maxima extractor for a Pattern.
 */

#include "data/MaxExtractor.h"

data::Feature data::MaxExtractor::operator() (const data::Pattern& in) const
{
  return gsl_vector_max(abuse(in));
}



