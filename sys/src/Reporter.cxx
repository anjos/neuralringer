//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/Reporter.cxx
 *
 * @brief This file contains the declaration of what is a
 * sys::Reporter. 
 */

#include <string>
#include <cstdlib>
#include <sstream>

#include "sys/Reporter.h"
#include "sys/LocalReporter.h"
#include "sys/debug.h"
#include "sys/util.h"

/**
 * @warn Notice that this c'tor is only called when s_instance is
 * 0. That launches the building process of a new reporter. Once this
 * is done, it is never repeated.
 */
sys::Reporter::Reporter(const std::string& type)
  : m_impl(0)
{
  if ( type == "local" )
    m_impl = new sys::LocalReporter();
  else {
    m_impl = new sys::LocalReporter();
    warn("Using \"LOCAL\" reporting system.");
  }

  ///Start-up report: the date
  std::ostringstream oss;
  oss << "This is the NeuralRinger v" << NR_VERSION << " Reporter, good day.";
  report(oss.str());
  RINGER_DEBUG1("Debug level 1 is activated.");
  RINGER_DEBUG2("Debug level 2 is activated.");
  RINGER_DEBUG3("Debug level 3 is activated.");
}

sys::Reporter::~Reporter()
{
  //Shutdown report.
  std::ostringstream oss;
  oss << "NeuralRinger v" << NR_VERSION << " Reporter has finished.";
  report(oss.str());
  delete m_impl;
}

bool sys::Reporter::report(const std::string& info)
{
  std::string s = sys::currenttime("short") + ": " + info + "\n";
  return m_impl->write(s);
}

bool sys::Reporter::warn(const std::string& info)
{
  std::string s = sys::currenttime("short") + ": **WARNING** " + info + "\n";
  return m_impl->write(s);
}

bool sys::Reporter::fatal(const std::string& info)
{
  std::string s = sys::currenttime("short") + ": **FATAL** " + info + "\n";
  m_impl->write(s);
  std::exit(1);
  return true;
}

bool sys::Reporter::except(const std::string& info)
{
  std::string s = sys::currenttime("short") + ": **EXCEPTION** "
    + info + "\n";
  return m_impl->write(s);
}
