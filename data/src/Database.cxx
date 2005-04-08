//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/Database.cxx
 *
 * Implements the database readout and saving.
 */
#include "data/Database.h"

data::Pattern make_target (const size_t n, const size_t s,
			   const double& min, const double& max)
{
  size_t next_power_of_2 = lrint(std::ceil(log2(n+1)));
  if (s < next_power_of_2) {
    RINGER_DEBUG1("I cannot represent \"" << n << "\" in binary in a vector of"
		  << " size \"" << s << "\". I would need at least \""
		  << next_power_of_2 << "\" positions. Exception thrown.");
    throw RINGER_EXCEPTION("Unproper Pattern length on binary representation");
  }
  data::Pattern retval(s, min);
  size_t v = n;
  for (size_t i=0; i<next_power_of_2; ++i) {
    if (v%2) retval[i] = max;
    v = v/2;
  }
  return retval;
}
