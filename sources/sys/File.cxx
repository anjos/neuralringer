//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/src/File.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * A common interface to file readout.
 */

#include "sys/File.h"
#include "sys/Plain.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include "sys/util.h"

sys::File::File (const std::string& filename, std::ios_base::openmode m,
		 const char sep)
  : m_fimpl(0),
    m_filename(filename),
    m_sep(sep)
{
  //no choices for the time being, only 1 reader implemented
  if (filename.find(".rfd.gz") != std::string::npos &&
      filename.find(".rfd.gz") == filename.size() - 7) { //a compressed file!
    RINGER_DEBUG1("Implementation to read gzip'ep files is not yet there!");
    throw RINGER_EXCEPTION("Cannot read gzip'ed files");
  }
  else if (filename.find(".rfd.bz2") != std::string::npos &&
	   filename.find(".rfd.bz2") == filename.size() - 8) { 
    //a compressed file!
    RINGER_DEBUG1("Implementation to read bzip2'ep files is not yet there!");
    throw RINGER_EXCEPTION("Cannot read bzip2'ed files");
  }
  else { //try as a plain file!
    try {
      if (!sys::backup(filename))
	throw RINGER_EXCEPTION("Cannot move an existing version of the file!");
      m_fimpl = new sys::Plain(filename, m);
      RINGER_DEBUG3("Plain file \"" << filename << "\" opened successfuly.");
    }
    catch (sys::Exception& e) {
      RINGER_DEBUG1("Openning \"" << m_filename << "\" caused an exception.");
      RINGER_DEBUG1(e.what());
      RINGER_DEBUG1("This became a *invalid* file.");
      delete m_fimpl;
      m_fimpl = 0;
    }
  }
}

sys::File::~File ()
{
  delete m_fimpl;
}

std::string sys::File::filename_no_ext (void) const
{
  return m_filename.substr(0, m_filename.rfind(".rfd"));
}

bool sys::File::eof (void) const
{
  if ( m_fimpl ) return m_fimpl->eof();
  return true;
}

bool sys::File::good (void) const
{
  if ( m_fimpl ) return m_fimpl->good();
  if ( eof() ) return false;
  return false;
}

bool sys::File::readmore (void) const
{
  if ( m_fimpl ) return m_fimpl->readmore();
  return false;
}


