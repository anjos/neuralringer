//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/Plain.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the plain text file readout
 */

#include "sys/Plain.h"
#include "sys/debug.h"
#include "sys/Exception.h"

sys::Plain::Plain (const std::string& filename, 
			 std::ios_base::openmode m)
  : m_filename(filename),
    m_file(filename.c_str(), m)
{
  if ( !m_file ) {
    RINGER_DEBUG1("I could *not* open the file \"" << m_filename 
		  << "\". Exception thrown.");
    throw RINGER_EXCEPTION("Cannot open file.");
  }
  RINGER_DEBUG3("File \"" << m_filename << "\" opened successfuly.");
}

sys::Plain::~Plain ()
{
  m_file.close();
  RINGER_DEBUG3("File \"" << m_filename << "\" closed successfuly.");
}

sys::Plain& sys::Plain::operator>> (int& i)
{
  m_file >> i;
  return *this;
}

sys::Plain& sys::Plain::operator>> (unsigned int& ui)
{
  m_file >> ui;
  return *this;
}

sys::Plain& sys::Plain::operator>> (double& d)
{
  m_file >> d;
  return *this;
}

sys::Plain& sys::Plain::operator>> (std::string& s)
{
  m_file >> s;
  return *this;
}

sys::Plain& sys::Plain::operator>> (char& c)
{
  m_file >> c;
  return *this;
}

sys::Plain& sys::Plain::operator<< (int i)
{
  m_file << i;
  return *this;
}

sys::Plain& sys::Plain::operator<< (unsigned int ui)
{
  m_file << ui;
  return *this;
}

sys::Plain& sys::Plain::operator<< (const double& d)
{
  m_file << d;
  return *this;
}

sys::Plain& sys::Plain::operator<< (char c)
{
  m_file << c;
  return *this;
}

sys::Plain& sys::Plain::operator<< (const std::string& s)
{
  m_file << s;
  return *this;
}

bool sys::Plain::eof (void) const
{
  return m_file.eof();
}

bool sys::Plain::good (void) const
{
  return m_file.good();
}

bool sys::Plain::readmore (void) const
{
  //peek next char to see if its an 'R', if is, return false, else return
  //true. Put back the character in any case...
  char c;
  sys::Plain* p = const_cast<sys::Plain*>(this);
  p->m_file >> c;
  p->m_file.putback(c);
  if (c == 'R') return false;
  return true;
}

