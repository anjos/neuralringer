//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/util.cxx
 *
 * Implements the facilities described on the header file.
 */

#include "data/util.h"
#include "data/MeanExtractor.h"
#include "gsl/gsl_pow_int.h"

data::Feature data::mean_square (const data::PatternSet& p)
{
  data::MeanExtractor mean;
  data::Pattern tmp(p.pattern_size());
  data::Feature retval = 0;
  for (size_t i=0; i<p.size(); ++i) {
    tmp = p.pattern(i);
    tmp.apply(gsl_pow_2);
    retval += mean(tmp);
  }
  retval /= (p.size()*p.pattern_size());
  return retval;
}

data::Feature data::root_mean_square (const data::PatternSet& p)
{
  return std::sqrt(data::mean_square(p)); 
}

data::Feature data::abs_mean (const data::PatternSet& p)
{
  data::MeanExtractor mean;
  data::Pattern tmp(p.pattern_size());
  data::Feature retval = 0;
  for (size_t i=0; i<p.size(); ++i) {
    tmp = p.pattern(i);
    tmp.apply(std::fabs);
    retval += mean(tmp);
  }
  retval /= (p.size());
  return retval;
}


