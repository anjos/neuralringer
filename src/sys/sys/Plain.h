//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/Plain.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief Implements a plain text file read/write type.
 */

#ifndef RINGER_SYS_PLAIN_H
#define RINGER_SYS_PLAIN_H

#include "sys/FileImplementation.h"
#include <locale>
#include <bits/ios_base.h>
#include <fstream>

namespace sys {

  /**
   * A generic interface to read data from files.
   */
  class Plain : public FileImplementation {

  public: //interface

    /**
     * Opens a new plain text file.
     *
     * @param filename The name of the file to open
     * @param m The openning mode.
     */
    Plain (const std::string& filename, 
	   std::ios_base::openmode m = std::ios_base::in);

    /**
     * A virtual destructor, to rhyme with the rest...
     */ 
    virtual ~Plain ();

    /**
     * Streams out an integer
     *
     * @param i The integer to assign the value to.
     */
    virtual Plain& operator>> (int& i);

    /**
     * Streams out an unsigned integer
     *
     * @param ui The unsigned integer to assign the value to.
     */
    virtual Plain& operator>> (unsigned int& ui);

    /**
     * Streams out a double precision number
     *
     * @param d The double to assign the value to.
     */
    virtual Plain& operator>> (double& d);

    /**
     * Streams out a character
     *
     * @param c The character to assign the value to.
     */
    virtual Plain& operator>> (char& c);

    /**
     * Streams out a string
     *
     * @param s The string to assign the value to.
     */
    virtual Plain& operator>> (std::string& s);

    /**
     * Streams in an integer
     *
     * @param i The integer to stream in.
     */
    virtual Plain& operator<< (int i);

    /**
     * Streams in an unsigned integer
     *
     * @param ui The unsigned integer to stream in.
     */
    virtual Plain& operator<< (unsigned int ui);

    /**
     * Streams in a double precision number
     *
     * @param d The double to stream in
     */
    virtual Plain& operator<< (const double& d);

    /**
     * Streams in a string
     *
     * @param s The string to stream in.
     */
    virtual Plain& operator<< (const std::string& s);

    /**
     * Streams in a character
     *
     * @param c The character to stream  in.
     */
    virtual Plain& operator<< (char c);

    /**
     * Tests if the file is at its end
     */
    virtual bool eof (void) const;

    /**
     * Tests if the file is still good, meaning that the next read might
     * succeed.
     */
    virtual bool good (void) const;

    /**
     * Returns <b>false</b> if the RoI finishes at the current position. This
     * should be used to define when the RoI finishes, in a while loop.
     */
    virtual bool readmore (void) const;

  private: //representation

    std::string m_filename; ///< The name of the opened file
    std::fstream m_file; ///< The file itself.

  };

}

#endif /* RINGER_SYS_PLAIN_H */
