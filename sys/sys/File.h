//Dear emacs, this is -*- c++ -*-

/**
 * @file sys/File.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre dos Anjos</a>
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief This class describes a common interface to file readout.
 */

#ifndef RINGER_SYS_FILE_H
#define RINGER_SYS_FILE_H

#include <string>
#include <bits/ios_base.h>
#include "sys/FileImplementation.h"

/**
 * Encompass all entities to read, write and manipulate RoI dump files
 * produced with the modified T2Calo.
 */
namespace sys {

  /**
   * Objects of this class can access data stored on different kinds of files
   * and serve them to higher level objects. Objects can also be dumped using
   * analog operations.
   */
  class File {

  public: //interface

    /**
     * Creates an object of this type, taking as parameter the originating
     * filename.
     *
     * @param filename The name of the file to bind to.
     * @param openmode How to open the file, i.e., permissions
     * @param sep The standard separator to use
     */
    File (const std::string& filename, 
	  std::ios_base::openmode m = std::ios_base::in,
	  const char sep=',');
    
    /**
     * Standard destructor
     */
    virtual ~File ();

    /**
     * Defines the field separator which, by default, is ','.
     */
    virtual void separator (const char sep) { m_sep = sep; }

    /**
     * Returns the current separator to be used for this file
     */
    inline char separator (void) { return m_sep; }

    /**
     * Streams out an integer
     *
     * @param i The integer to assign the value to.
     */
    inline File& operator>> (int& i) { (*m_fimpl) >> i; return *this; }

    /**
     * Streams out an unsigned integer
     *
     * @param ui The unsigned integer to assign the value to.
     */
    inline File& operator>> (unsigned int& ui) 
    { (*m_fimpl) >> ui; return *this; }

    /**
     * Streams out a double precision number
     *
     * @param d The double to assign the value to.
     */
    inline File& operator>> (double& d) { (*m_fimpl) >> d; return *this; }

    /**
     * Streams out a string
     *
     * @param s The string to assign the value to.
     */
    inline File& operator>> (std::string& s) 
    { (*m_fimpl) >> s; return *this; }

    /**
     * Streams out a character
     *
     * @param c The character to assign the value to.
     */
    inline File& operator>> (char& c) { (*m_fimpl) >> c; return *this; }

    /**
     * Streams in an integer
     *
     * @param i The integer to stream in.
     */
    inline File& operator<< (int i) { (*m_fimpl) << i; return *this; }

    /**
     * Streams in an unsigned integer
     *
     * @param ui The unsigned integer to stream in.
     */
    inline File& operator<< (unsigned int ui) 
    { (*m_fimpl) << ui; return *this; }

    /**
     * Streams in a double precision number
     *
     * @param d The double to stream in
     */
    inline File& operator<< (const double& d) 
    { (*m_fimpl) << d; return *this; }

    /**
     * Streams in a character
     *
     * @param c The character to stream  in.
     */
    inline File& operator>> (const char& c) 
    { (*m_fimpl) << c; return *this; }

    /**
     * Streams in a string
     *
     * @param s The string to stream in.
     */
    inline File& operator<< (const std::string& s) 
    { (*m_fimpl) << s; return *this; }

    /**
     * Says the filename this object is bound to, without the extension.
     */
    std::string filename_no_ext (void) const;

    /**
     * Tests if the file is at its end
     */
    bool eof (void) const;
    
    /**
     * Tests if the file is still good, meaning that the next read might
     * succeed.
     */
    bool good (void) const;

    /**
     * Returns <b>false</b> if the RoI finishes at the current position. This
     * should be used to define when the RoI finishes, in a while loop.
     */
    bool readmore (void) const;

  private: //representation

    sys::FileImplementation* m_fimpl; ///< The file impl. to use
    std::string m_filename; ///< The name of the file I'm bound to
    char m_sep; ///< The standard field separator

  };

}

#endif /* RINGER_SYS_FILE_H */
