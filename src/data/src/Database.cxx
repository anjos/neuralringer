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

template <> void data::merge_sets<data::SimplePatternSet> 
(data::SimplePatternSet& dest, 
 const std::map<std::string, data::SimplePatternSet*>& source)
{
  bool init = false;
  for (std::map<std::string, data::SimplePatternSet*>::const_iterator
	 it = source.begin(); it != source.end(); ++it) {
    if (!init) {
      dest = *it->second;
      init = true;
    }
    else {
      dest.merge(*it->second);
    }
  }
}

/**
 * Some hackish way to emulate partial template specialization for the
 * merge_target() functionality on data::Database. This is the
 * data::SimplePatternSet specialization.
 *
 * @see data::Database::merge_target()
 * 
 * @param minimal Do we use "minimal" representation? A value of
 * <code>false</code> imples we use the "normal" representation.
 * @param min The minimum value the reader can interpret (neuron off).
 * @param max The maximum value the reader can interpret (neuron on).
 * @param target The target PatternSet according to the parameters before
 * @param source The source of data information that has to be merged
 */
template <> void data::merge_target_sets<data::SimplePatternSet>
(const bool minimal, const double& min, const double& max, 
 data::SimplePatternSet& target, 
 const std::map<std::string, data::SimplePatternSet*>& source)
{
  //The next test probably characterizes this should be split in two
  //functions, but let's leave it this way for the time being
  if (minimal) {
    //Condense the targets to the minimal binary set possible
    size_t total_size = 0;
    for (std::map<std::string, SimplePatternSet*>::const_iterator
	   it = source.begin(); it != source.end(); ++it)
      total_size += it->second->size();
    size_t next_power_of_2 = lrint(std::ceil(log2(source.size())));
    SimplePatternSet my_target(total_size, next_power_of_2, min);
    size_t class_counter=0;
    size_t pattern_counter=0;
    for (std::map<std::string, SimplePatternSet*>::const_iterator
	   it = source.begin(); it != source.end(); ++it) {
      for (size_t j=0; j<it->second->size(); ++j) {
	data::Pattern curr_target = make_target(class_counter, 
						next_power_of_2,
						min, max);
	my_target.set_pattern(pattern_counter, curr_target); 
	++pattern_counter;
      }
      ++class_counter;
    }
    target = my_target;
  }
  else {
    if (source.size() == 1) {
      SimplePatternSet tmp(source.begin()->second->size(), source.size(), min);
      data::Ensemble tmpe(source.begin()->second->size(), max);
      tmp.set_ensemble(0, tmpe);
      target = tmp;
    }
    if (source.size() > 1) {
      size_t i=1;
      bool init = false;
      for (std::map<std::string, SimplePatternSet*>::const_iterator
	     it = source.begin(); it != source.end(); ++it) {
	SimplePatternSet tmp(it->second->size(), source.size(), min);
	data::Ensemble tmpe(it->second->size(), max);
	tmp.set_ensemble(i++, tmpe);
	if (!init) {
	  target = tmp;
	  init = true;
	}
	else target.merge(tmp);
      }
    }
  }
}
