//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/test_Pattern.cxx
 *
 * @brief Tests the Pattern class and some of its functionality.
 *
 * The purpose of this test is to make sure that the Pattern is
 * responding as expected. Obvioulsy, other classes are also a bit
 * tested in the mean time.
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <valarray>
#include <cmath>

#include "data/Pattern.h"
#include "sys/Reporter.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  data::Pattern sample1(10, 3.14);
  sample1[0] = 6.28;
  sample1[9] = 6.28;
  data::Pattern sample2(10);
  for (unsigned int i=0; i<sample2.size(); ++i) sample2[i] = i;

  RINGER_REPORT(reporter, "Making simple operations on Pattern's");
  RINGER_REPORT(reporter, "sample1: " << sample1);
  RINGER_REPORT(reporter, "sample2: " << sample2);
  RINGER_REPORT(reporter, "sample1.mean(): " << sample1.mean());
  RINGER_REPORT(reporter, "sample2.mean(): " << sample2.mean());
  
  sample1.remove_mean();
  sample2.remove_mean();
  RINGER_REPORT(reporter, "sample1-mean: " << sample1);
  RINGER_REPORT(reporter, "sample2-mean: " << sample2);
  RINGER_REPORT(reporter, "sample1.mean(): " << sample1.mean());
  RINGER_REPORT(reporter, "sample2.mean(): " << sample2.mean());
  RINGER_REPORT(reporter, "sample1.variance(): " << sample1.variance());
  RINGER_REPORT(reporter, "sample2.variance(): " << sample2.variance());

  sample1.energy_normalise();
  sample2.energy_normalise();
  RINGER_REPORT(reporter, "sample1/norm: " << sample1);
  RINGER_REPORT(reporter, "sample2/norm: " << sample2);

  data::Pattern sample3 = sample1 + sample2;
  RINGER_REPORT(reporter, "sample3 = sample1 + sample2: " << sample3);

  sample3 /= 2;
  RINGER_REPORT(reporter, "sample3 /= 2: " << sample3);
  
  sample3.apply(std::tanh);
  RINGER_REPORT(reporter, "sample3.apply(tanh): " << sample3);
  return 0;
}










