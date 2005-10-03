//Dear emacs, this is -*- c++ -*-

/**
 * @file rbuild/RingSet.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre dos Anjos</a>
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief Describes a set of rings with associated values.
 */

#ifndef RINGER_RBUILD_RINGSET_H
#define RINGER_RBUILD_RINGSET_H

#include "roiformat/Cell.h"
#include <vector>
#include "rbuild/RingConfig.h"
#include "data/Pattern.h"

/**
 * Encompasses all classes related to ring building and manipulation
 */
namespace rbuild {

  /**
   * Builds a new set of rings
   */
  class RingSet {

  public: //interface

    /**
     * Builds a new RingSet, starting from a ring definition. The RingSet
     * starts empty.
     *
     * @param config This RingSet configuration
     */
    RingSet (const RingConfig& config);

    /**
     * to create an empty RingSet is possible, but watch out, you can't do
     * much with it...
     */
    RingSet ();

    /**
     * Virtualises the destructor
     */
    virtual ~RingSet();

    /**
     * Access the configuration for this object
     */
    inline const rbuild::RingConfig& config (void) const { return m_config; }

    /**
     * Adds some set of cells to this RingSet.
     *
     * @param c The cells to add
     * @param eta_center Where, in eta, I should center my rings
     * @param phi_center Where, in phi, I should center my rings
     */
    void add (const std::vector<const roiformat::Cell*>& c,
	      const double& eta_center, const double& phi_center);

    /**
     * Returns the (current) ring values.
     */
    inline data::Pattern& pattern (void) { return m_val; }

    /**
     * Resets all current values
     */
    inline void reset (void) { m_val = 0.0; }

  private: //representation

    rbuild::RingConfig m_config; ///< my own configuration for ring building
    data::Pattern m_val; ///< my current values

  };

}

#endif /* RINGER_RBUILD_RINGSET_H */
