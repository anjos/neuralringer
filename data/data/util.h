//Dear emacs, this is -*- c++ -*-

/**
 * @file data/util.h
 *
 * @brief Describes a set of operations appliable to Pattern's and
 * PatternSet's.
 */

#ifndef DATA_UTIL_H
#define DATA_UTIL_H

#include "data/Feature.h"
#include "data/PatternSet.h"

namespace data {

  /**
   * Calculates the Mean Square (MS) of a given PatternSet, taking in
   * consideration both of its dimensions. The formulae for this calculation 
   * is given by:
   * @f[
   * \frac{{\sum_i}^M{{\sum_j}^N{{e_{ij}}^{2}}}}{M \times N}
   * @f]
   * Where <b>M</b> describes the number of patterns the set contains and
   * <b>N</b>, the number of ensembles there is.
   *
   * @param p The pattern set to use for calculating the RMS
   */
  data::Feature mean_square (const data::PatternSet& p);
	
  /**
   * Calculates the Root Mean Square (RMS) of a given PatternSet, taking in
   * consideration both of its dimensions. The formula for this calculation is
   * given by:
   * @f[
   * E = \sqrt{\frac{{\sum_i}^M{{\sum_j}^N{{e_{ij}}^{2}}}}{M \times N}}
   * @f]
   * Where <b>M</b> describes the number of patterns the set contains and
   * <b>N</b>, the number of ensembles there is.
   *
   * @param p The pattern to use for calculating the RMS
   */
  data::Feature root_mean_square (const data::PatternSet& p);

  /**
   * Calculates the mean of a sum of absolute values of a given PatternSet,
   * taking in consideration both of its dimensions. The formula for this
   * calculation is given by:
   * @f[
   * E = \frac{{\sum_i}^M{{\sum_j}^N{{|e_{ij}|}}}}{M \times N}
   * @f]
   * Where <b>M</b> describes the number of patterns the set contains and
   * <b>N</b>, the number of ensembles there is.
   *
   * @param p The pattern to use for calculating the RMS
   */
  data::Feature abs_mean (const data::PatternSet& p);

}

#endif /* DATA_UTIL_H */
