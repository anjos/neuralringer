//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/LocalReporter.h
 *
 * @brief This file contains the definition of what is a
 * sys::LocalReporter. 
 */

#ifndef RINGER_SYS_LOCALREPORTER_H
#define RINGER_SYS_LOCALREPORTER_H

#include <iostream>
#include <string>

#include "sys/ReporterImplementation.h"

namespace sys {

  /**
   * Defines a way to talk to a user that is sitting locally with
   * respect to the executing program.
   */
  class LocalReporter : public ReporterImplementation {
    
  public:

    /**
     * The only constructor.
     * @param os The <em>normal</em> output stream
     * @param es The error stream
     */
    LocalReporter(std::ostream& os =std::cout, 
		  std::ostream& es =std::cerr);

    /**
     * Destructor virtualisation.
     */
    virtual ~LocalReporter();
    
    /**
     * Write something to the reporter.
     * @param info What to write to the available stream.
     */
    virtual bool write (const std::string& info);

    /**
     * Write something to the reporter, using a different strategy
     * though.
     * @param info What to write to the available stream.
     */
    virtual bool write_error (const std::string& info);

  private: ///Representation
    std::ostream& m_os; ///< The standard output stream
    std::ostream& m_es; ///< The error stream
  };

}

#endif /* RINGER_SYS_LOCALREPORTER_H */
