//Dear emacs, this is -*- c++ -*-

/**
 * @file roiformat/Cell.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * @brief Implements a Cell that can be readout of a roiformat::File or just
 * exist in the empty space. Scratch constructors are provided.
 */

#ifndef RINGER_ROIFORMAT_CELL_H
#define RINGER_ROIFORMAT_CELL_H

#include "sys/File.h"
#include <string>
#include <vector>

namespace roiformat {

  /**
   * Describes every cell in a calorimeter
   */
  class Cell {

  public: //interface

    enum Sampling { 
      PSBARREL    =  0, ///< pre-sampler, barrel section
      EMBARREL1   = 1, ///< electromagnetic calo, barrel section, front layer
      EMBARREL2   = 2, ///< electromagnetic calo, barrel section, middle layer
      EMBARREL3   = 3, ///< electromagnetic calo, barrel section, back layer
      PSENDCAP    =  4, ///< pre-sampler, endcap section
      EMENDCAP1   = 5, ///< electromagnetic calo, endcap section, front layer
      EMENDCAP2   = 6, ///< electromagnetic calo, endcap section, front layer
      EMENDCAP3   = 7, ///< electromagnetic calo, endcap section, front layer
      HADENCAP0   = 8, ///< hadronic calo, endcap section (LAr), layer 0
      HADENCAP1   = 9, ///< hadronic calo, endcap section (LAr), layer 1
      HADENCAP2   = 10, ///< hadronic calo, endcap section (LAr), layer 2
      HADENCAP3   = 11, ///< hadronic calo, endcap section (LAr), layer 3
      TILEBARREL0 = 12, ///< hadronic calo, barrel (Tiles), layer 0
      TILEBARREL1 = 13, ///< hadronic calo, barrel (Tiles), layer 1
      TILEBARREL2 = 14, ///< hadronic calo, barrel (Tiles), layer 2
      TILEGAPSCI0 = 15, ///< hadronic calo, barrel (Tiles), gap scint. layer 0
      TILEGAPSCI1 = 16, ///< hadronic calo, barrel (Tiles), gap scint. layer 1
      TILEGAPSCI2 = 17, ///< hadronic calo, barrel (Tiles), gap scint. layer 2
      TILEEXTB0   = 18, ///< hadronic calo, extended barrel (Tiles), layer 0
      TILEEXTB1   = 19, ///< hadronic calo, extended barrel (Tiles), layer 1
      TILEEXTB2   = 20, ///< hadronic calo, extended barrel (Tiles), layer 2
      FORWCAL0    = 21, ///< forward calo, layer 0
      FORWCAL1    = 22, ///< forward calo, layer 1
      FORWCAL2    = 23, ///< forward calo, layer 2
      UNKNOWN     = 24  ///< unknown piece of data, not supposed to occur:)
    };

    /**
     * An alias to simplify our lives.
     */
    typedef enum Sampling Sampling;

    /**
     * Scratch build up. Take default values (zeroes)
     */
    Cell () {};

    /**
     * Builds a new cell, specifying every cell value
     *
     * @param sampling This cell's sampling
     * @param eta This cell's center in eta
     * @param phi This cell's center in phi
     * @param r This cell's radius, with respect to ATLAS center
     * @param deta The size of the cell, in eta
     * @param dphi The size of the cell, in phi
     * @param dr The size of the cell, in the radius direction
     * @param energy The cell's energy
     */
    Cell (Sampling sampling, const double& eta, const double& phi, 
	  const double& r, const double& deta, const double& dphi,
	  const double& dr, const double& energy);

    /**
     * Destroyes the cell
     */
    virtual ~Cell () {}

    /**
     * Accessors
     */
    inline Sampling sampling(void) const { return m_samp; }
    inline const double& eta(void) const { return m_eta; }
    inline const double& phi(void) const { return m_phi; }
    inline const double& r(void) const { return m_r; }
    inline const double& deta(void) const { return m_deta; }
    inline const double& dphi(void) const { return m_dphi; }
    inline const double& dr(void) const { return m_dr; }
    inline const double& energy(void) const { return m_energy; } 

    /**
     * Streams in data into a cell.
     *
     * @param File The roiformat::File to take the data from.
     */
    sys::File& operator<< (sys::File& f);

  private: //representation

    Sampling m_samp; ///< This cell's sampling
    double m_eta; ///< The center of the cell, in eta
    double m_phi; ///< The center of the cell, in phi
    double m_r; ///< The center of the cell with respect to the central axis.
    double m_deta; ///< The size of the cell, in eta
    double m_dphi; ///< The size of the cell, in phi
    double m_dr; ///< The size of the cell, in radius direction
    double m_energy; ///< This cell's energy
    char m_sep; ///< The separator used when dumping
  };

  /**
   * Converts a string into a Cell::Sampling
   *
   * @param s string to convert
   */
  Cell::Sampling str2sampling (const std::string& s);
  
  /**
   * Converts a a Cell::Sampling into string
   *
   * @param s the sampling to convert
   */
  std::string sampling2str (const Cell::Sampling& s);

  /**
   * Returns the eta and phi of the cell with most energy deposition.
   * 
   * @param vcell A vector of constant cells
   * @param eta The eta value to be returned
   * @param phi The phi value to be returned
   */
  void max (const std::vector<const Cell*>& vcell, double& eta, double& phi);

}

/** 
 * How to read my own data from a roiformat::File
 *
 * @param f The roiformat::File to read the data from
 * @param c The cell where to put the data
 */
sys::File& operator>> (sys::File& f, roiformat::Cell& c);

/** 
 * How to write my own data to a roiformat::File
 *
 * @param f The roiformat::File to write the data to
 * @param c The cell where to take the data from
 */
sys::File& operator<< (sys::File& f, const roiformat::Cell& c);

#endif /* RINGER_ROIFORMAT_CELL_H */
