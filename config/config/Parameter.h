//Dear emacs, this is -*- c++ -*-

/**
 * @file Parameter.h
 *
 * @brief Defines an abstract class to a parameter. It can be used to clone or
 * delete the parameter related to the object without knowing its type.
 */

#ifndef CONFIG_PARAMETER_H
#define CONFIG_PARAMETER_H

#include <libxml/tree.h>

namespace config {
  
  /**
   * An abstract interface to paramters given to (usually) hidden or output
   * neurons.
   */
  class Parameter {
    
  public: //interface, completely abstract

    /**
     * Destructor virtualisation
     */
    virtual ~Parameter() {};

    /**
     * Clones this object
     */
    virtual Parameter* clone () const =0;

    /**
     * Gets the XML representation for these parameters.
     */
    virtual xmlNodePtr node () =0;

  };

}

#endif /* CONFIG_PARAMETER_H */
