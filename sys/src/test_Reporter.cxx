//Dear emacs, this is -*- c++ -*-

/**
 * @file test_Reporter.cxx
 *
 * @brief Do brief tests on sys::Report. 
 */

#include "sys/Reporter.h"
#include "sys/debug.h"

int main (void)
{
  sys::Reporter r("local");

  r.report ("This is an example report");
  r.warn ("This is an example warning");
  RINGER_DEBUG1("This is a LOW priority debugging message");
  RINGER_DEBUG2("This is a NORMAL priority debugging message");
  RINGER_DEBUG3("This is a HIGH priority debugging message");
  return(0);
}
