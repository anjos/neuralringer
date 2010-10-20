//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/FileImplementation.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief Describes the abstract interface to read data from a stream.
 */

#ifndef RINGER_SYS_FILEIMPLEMENTATION_H
#define RINGER_SYS_FILEIMPLEMENTATION_H

#include <string>

namespace sys {

  /**
   * A generic interface to read data from files.
   */
  class FileImplementation {

  public: //interface

    /**
     * A virtual destructor, to rhyme with the rest...
     */ 
    virtual ~FileImplementation () {};

    /**
     * Streams out an integer
     *
     * @param i The integer to assign the value to.
     */
    virtual FileImplementation& operator>> (int& i) =0;

    /**
     * Streams out an unsigned integer
     *
     * @param ui The unsigned integer to assign the value to.
     */
    virtual FileImplementation& operator>> (unsigned int& ui) =0;

    /**
     * Streams out a double precision number
     *
     * @param d The double to assign the value to.
     */
    virtual FileImplementation& operator>> (double& d) =0;

    /**
     * Streams out a character
     *
     * @param c The character to assign the value to.
     */
    virtual FileImplementation& operator>> (char& c) =0;

    /**
     * Streams out a string
     *
     * @param s The string to assign the value to.
     */
    virtual FileImplementation& operator>> (std::string& s) =0;

    /**
     * Streams in an integer
     *
     * @param i The integer to stream in.
     */
    virtual FileImplementation& operator<< (int i) =0;

    /**
     * Streams in an unsigned integer
     *
     * @param ui The unsigned integer to stream in.
     */
    virtual FileImplementation& operator<< (unsigned int ui) =0;

    /**
     * Streams in a double precision number
     *
     * @param d The double to stream in
     */
    virtual FileImplementation& operator<< (const double& d) =0;

    /**
     * Streams in a string
     *
     * @param s The string to stream in.
     */
    virtual FileImplementation& operator<< (const std::string& s) =0;

    /**
     * Streams in a character
     *
     * @param c The character to stream  in.
     */
    virtual FileImplementation& operator<< (char c) =0;

    /**
     * Tests if the file is at its end
     */
    virtual bool eof (void) const =0;

    /**
     * Tests if the file is still good, meaning that the next read might
     * succeed.
     */
    virtual bool good (void) const =0;

    /**
     * Returns <b>false</b> if the RoI finishes at the current position. This
     * should be used to define when the RoI finishes, in a while loop.
     */
    virtual bool readmore (void) const =0;

  };

}

#endif /* RINGER_SYS_FILEIMPLEMENTATION_H */
