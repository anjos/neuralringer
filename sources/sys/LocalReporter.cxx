//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/LocalReporter.cxx
 *
 * @brief This file contains the declaration of what is a
 * sys::LocalReporter. 
 */

#include "sys/LocalReporter.h"

/**
 * Measures a string size and check if it is bellow a certain
 * limit. If not, break it in a special way to fit the screen.
 * @param s The string to check
 */
std::string& check_string (std::string& s)
{
  const int lsize = 72; ///< The lines maximum size
  int left = s.size() - lsize;
  size_t counter = 1;
  while ( left > 0 ) {
    size_t pos = counter++ * lsize;
    s.insert(pos,"\\\n | ");
    ///stopped here!
    left -= lsize;
  }
  return s;
}

sys::LocalReporter::LocalReporter(std::ostream& os, std::ostream& es)
  : m_os(os), m_es(es)
{
}

sys::LocalReporter::~LocalReporter()
{
}

bool sys::LocalReporter::write (const std::string& info)
{
  m_os << info;
  if (m_os) return true;
  return false;
}

bool sys::LocalReporter::write_error (const std::string& info)
{
  m_es << info;
  if (m_es) return true;
  return false;
}
