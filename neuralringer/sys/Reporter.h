//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/Reporter.h
 *
 * @brief This file contains the definition of what is a sys::Reporter. The
 * reporter should, among other things, report debugging messages
 * correctly. This is controlled by a small set of macros that correctly
 * address the debugging facilities of the Reporter. For using those, you have
 * to set a compilation macro accordingly: <code>DEBUG</code>=LOW, NORMAL or
 * HIGH. If this macro is undefined for the time of compilation, no
 * compilation options are coded in.
 */

#ifndef RINGER_SYS_REPORTER_H
#define RINGER_SYS_REPORTER_H

#include <string>
#include <sstream>
#include "sys/ReporterImplementation.h"

/**
 * The <em>System</em> namespace contains system access objects.
 *
 * This namespace contains the functionality required by RINGER to get
 * in touch with the user environment, be it in form of messages that
 * are handled to the user or for reporting errors.
 */
namespace sys {

  /**
   * Defines the interface to the Reporting System.
   *
   * This class defines how the messages and errors are sent to the
   * monitoring user. Since the implementation of this class can
   * change depending on the environment, we have decoupled from its
   * interface by using the <em>bridge</em> pattern.
   */
  class Reporter {

  public:
    
    /**
     * The only constructor.
     * @param type The type of Reporter to use.
     */
    Reporter(const std::string& type ="local");

    /**
     * Destructor virtualisation.
     */
    ~Reporter();

  public:

    /**
     * Report something to the user.
     * @param info What to report.
     */
    bool report (const std::string& info);

    /**
     * Warn the user about a problem.
     * @param info What to warn about.
     */
    bool warn (const std::string& info);

    /**
     * Warn the user about a problem and std::exit() afterwards.
     * @param info What to report about the problem.
     */
    bool fatal (const std::string& info);

    /**
     * Warn the user about an exception.
     * @param info What to report about the problem.
     */
    bool except (const std::string& info);

  private: ///Forbidden

    /**
     * Copy construct
     *
     * @param other The reporter to copy
     */
    Reporter (const Reporter& other);

    /**
     * Assignment operation
     *
     * @param other The reporter to copy the parameters from
     */
    Reporter& operator= (const Reporter& other);

  private: ///Representation
    sys::ReporterImplementation* m_impl; ///< The underlaying impl.

  };
  
}

/**
 * Defines a simpler way to report messages
 */
#define RINGER_REPORT(r,m) { \
    std::ostringstream s;  \
    s << m;                \
    r.report(s.str());	   }

/**
 * Defines a simpler way to report messages
 */
#define RINGER_FATAL(r,m) { \
    std::ostringstream s;  \
    s << m;                \
    r.fatal(s.str());	   }

/**
 * Defines a simpler way to report messages
 */
#define RINGER_EXCEPT(r,m) { \
    std::ostringstream s;  \
    s << m;                \
    r.except(s.str());	   }

/**
 * Defines a simpler way to report messages
 */
#define RINGER_WARN(r,m) { \
    std::ostringstream s;  \
    s << m;                \
    r.warn(s.str());	   }

#endif /* RINGER_SYS_REPORTER_H */ 

