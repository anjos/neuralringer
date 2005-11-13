//Dear emacs, this is -*- c++ -*-

/**
 * @file RingSet.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements a set of rings
 */

#include "rbuild/RingSet.h"
#include "sys/debug.h"
#include "roiformat/Cell.h"
#include <cmath>

rbuild::RingSet::RingSet (const RingConfig& config)
  : m_config(config),
    m_val(config.max())
{
  RINGER_DEBUG2("Created new RingSet with configuration parameters from \""
		<< m_config.name() << "\".");
}

rbuild::RingSet::RingSet ()
  : m_config(),
    m_val(0)
{
  RINGER_DEBUG2("Created new empty RingSet.");
}

rbuild::RingSet::~RingSet()
{
  RINGER_DEBUG2("Destroyed RingSet with configuration parameters from \""
		<< m_config.name() << "\".");
}

void rbuild::RingSet::add (const std::vector<const roiformat::Cell*>& c,
			   const double& eta_center, const double& phi_center)
{
  typedef std::vector<const roiformat::Cell*> vec_type;
  typedef std::vector<roiformat::Cell::Sampling> samp_type;

  RINGER_DEBUG1("Starting add procedure for cell vector with " << c.size()
		<< " entries. The current RingSet is configured for:");
  //   for (samp_type::const_iterator jt=m_config.detectors().begin();
  //        jt!=m_config.detectors().end(); ++jt) 
  //     RINGER_DEBUG3(" ## " << roiformat::sampling2str(*jt));
  unsigned int fit_counter = 0;

  //calculate ranges based on center point given. This is not very optimal and
  //could be made faster by configuring the window in advance.
  std::vector<double> etamax(m_config.max());
  std::vector<double> etamin(m_config.max());
  std::vector<double> phimin(m_config.max());
  std::vector<double> phimax(m_config.max());
  for (size_t i=0; i<m_config.max(); ++i) {
    double etasize = (i+0.5)*m_config.eta_size();
    double phisize = (i+0.5)*m_config.phi_size();
    etamax[i] = eta_center + etasize;
    etamin[i] = eta_center - etasize;
    phimax[i] = phi_center + phisize;
    phimin[i] = phi_center - phisize;
  }

  RINGER_DEBUG1("Considering center at (eta,phi) = (" << eta_center
		<< "," << phi_center << ")"); 
  //are we, possibly at the wrap-around region for phi?
  bool wrap = false;
  if (phi_center > (roiformat::TWO_PI - roiformat::PI_THRESHOLD)) {
      wrap = true;
      RINGER_DEBUG3(phi_center << " is greater than " 
		    << (roiformat::TWO_PI - roiformat::PI_THRESHOLD));
      RINGER_DEBUG3("Possible Ring window at the phi wrap around"
		    << " region *DETECTED*.");
  }
  bool reverse_wrap = false;
  if (phi_center < roiformat::PI_THRESHOLD) {
      reverse_wrap = true;
      RINGER_DEBUG3(phi_center << " is smaller than " << roiformat::PI_THRESHOLD);
      RINGER_DEBUG3("Possible (reverse) Ring window at the phi wrap around"
		    << " region *DETECTED*.");
  }

  //for all cells
  for (vec_type::const_iterator it=c.begin(); it!=c.end(); ++it) {
 
    //check if the cell is in a sampling I should handle. This is also useless
    //in a environment where the user knows the cell samplings (s)he is
    //feeding in. In this case, we could optimize the procedure eliminating
    //this check.
    bool belongs = false;
    for (samp_type::const_iterator jt=m_config.detectors().begin();
	 jt!=m_config.detectors().end(); ++jt) {
      if ((*it)->sampling() == *jt) {
	belongs = true;
	break;
      }
    }
    if (!belongs) continue; //go to next cell
    
    //if I get here, is because I have to include the cell energy. This
    //algorithm is fast, it goes from the inner ring to the outter ring,
    //looking the first place where the cell would fit. When it finds, it adds
    //up the cell value there and goes to the next cell. No need to do
    //anything later, because the sums are already correct!

    double phi_use = (*it)->phi(); //use this value for phi (wrap protection)
    if ( wrap && ((*it)->phi() < M_PI) ) phi_use += roiformat::TWO_PI;
    if ( reverse_wrap && ((*it)->phi() > M_PI) ) phi_use -= roiformat::TWO_PI;

    for (size_t i=0; i<m_config.max(); ++i) {
      if ((*it)->eta() > etamin[i] && (*it)->eta() < etamax[i] &&
	  phi_use > phimin[i] && phi_use < phimax[i]) {
	RINGER_DEBUG1(**it << " -> falls on ring[" << i << "]");
	m_val[i] += (*it)->energy();
	++fit_counter;
	break;
      }
    } //end for all rings

  } //end for all cells

  //give us Et instead of E
  double one_over = 1 / std::cosh(std::fabs(eta_center));
  for (size_t i=0; i<m_config.max(); ++i) m_val[i] *= one_over;

  //return happily
  RINGER_DEBUG2("A total of " << fit_counter << " (" 
		<<  (100*fit_counter)/c.size() << " %) cells were pertinent.");
  return;
}

