//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/test_RandomInteger.cxx
 *
 * @brief Tests the RandomInteger class and some of its
 * functionality. 
 *
 * The purpose of this test is to make sure that the RandomInteger is
 * responding as expected. That means, it is genearating really a
 * random sequence of integers between <code>0</code> and a limit.
 */

#include <iostream>
#include <sstream>
#include <valarray>

#include "data/RandomInteger.h"
#include "sys/Reporter.h"

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if ( argc != 3 ) 
    RINGER_FATAL(reporter, "usage: " << argv[0] << " <max int> <no. of values>");
  std::istringstream iss(argv[1]);
  size_t max;
  iss >> max;
  std::istringstream iss2(argv[2]);
  size_t no;
  iss2 >> no;

  data::RandomInteger ri(time(0));
  std::vector<size_t> rnd(no);
  ri.draw(max,rnd);

  //print results with commas
  for (size_t i=0; i<no; ++i)
    std::cout << rnd[i] << std::endl;

  return 0;
}
