//Dear emacs, this is -*- c++ -*-

/**
 * @file roiformat/src/RoI.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the RoI component.
 */

#include "roiformat/RoI.h"
#include "sys/debug.h"

roiformat::RoI::RoI ()
  : m_cells(),
    m_samp(),
    m_lvl1_id(0),
    m_roi_id(0),
    m_eta(0),
    m_phi(0)
{
}

roiformat::RoI::RoI (const std::vector<roiformat::Cell>& vc,
		     unsigned int lvl1_id, unsigned int roi_id,
		     const double& eta, const double& phi)
  : m_cells(vc),
    m_samp(),
    m_lvl1_id(lvl1_id),
    m_roi_id(roi_id),
    m_eta(eta),
    m_phi(phi)
{
  typedef std::vector<roiformat::Cell> vec_type;
  for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    roiformat::Cell::Sampling s = it->sampling();
    m_samp[s].push_back(&(*it));
  }
  RINGER_DEBUG2("Created RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << "} from scratch.");
}

roiformat::RoI::RoI (const RoI& other)
  : m_cells(other.m_cells),
    m_samp(),
    m_lvl1_id(other.m_lvl1_id),
    m_roi_id(other.m_roi_id),
    m_eta(other.m_eta),
    m_phi(other.m_phi)
{
  typedef std::vector<roiformat::Cell> vec_type;
    for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    roiformat::Cell::Sampling s = it->sampling();
    m_samp[s].push_back(&(*it));
  }
  RINGER_DEBUG2("Copied RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << "} from another RoI.");
}

roiformat::RoI& roiformat::RoI::operator= (const RoI& other)
{
  m_cells = other.m_cells;
  m_lvl1_id = other.m_lvl1_id;
  m_roi_id = other.m_roi_id;
  m_eta = other.m_eta;
  m_phi = other.m_phi;
  m_samp.clear(); ///< remove *all* entries for this map
  typedef std::vector<roiformat::Cell> vec_type;
  for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    roiformat::Cell::Sampling s = it->sampling();
    m_samp[s].push_back(&(*it));
  }
  RINGER_DEBUG2("Assigned RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << "} from another RoI.");
  return *this;
}

const std::vector<const roiformat::Cell*>* roiformat::RoI::cells
(const roiformat::Cell::Sampling& s) const 
{ 
  typedef std::map<roiformat::Cell::Sampling, 
    std::vector<const roiformat::Cell*> > map_type; 
  map_type::const_iterator it = m_samp.find(s);
  if (it == m_samp.end()) return 0;
  return &(const_cast<roiformat::RoI*>(this)->m_samp[s]);
}

void roiformat::RoI::cells (std::vector<const roiformat::Cell*>& vc) const
{
  typedef std::vector<roiformat::Cell> vec_type;
  for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    vc.push_back(&(*it));
  }
}

void roiformat::RoI::cells (const roiformat::Cell::Sampling& s,
			    std::vector<const roiformat::Cell*>& vc) const
{
  typedef std::map<roiformat::Cell::Sampling, 
    std::vector<const roiformat::Cell*> > map_type; 
  map_type::const_iterator it = m_samp.find(s);
  if (it == m_samp.end()) return;
  //append
  typedef std::vector<const roiformat::Cell*> vec_type;
  vec_type tv = const_cast<roiformat::RoI*>(this)->m_samp[s];
  vc.insert(vc.end(), tv.begin(), tv.end());
}

sys::File& roiformat::RoI::operator<< (sys::File& f)
{
  std::string s;
  m_cells.erase(m_cells.begin(), m_cells.end()); ///< clean-up
  f >> s >> m_roi_id >> s >> m_eta >> s >> m_phi >> s >> s >> m_lvl1_id;
  RINGER_DEBUG2("Reading RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << " eta: " << m_eta << " phi: " << m_phi 
	      << "} from File.");
  while (f.readmore() && !f.eof() && f.good()) {
    roiformat::Cell c;
    f >> c;
    m_cells.push_back(c);
  }
  typedef std::vector<roiformat::Cell> vec_type;
  for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    roiformat::Cell::Sampling s = it->sampling();
    m_samp[s].push_back(&(*it));
  }
  RINGER_DEBUG2("Read RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << "} from File.");
  return f;
}

sys::File& roiformat::RoI::operator>> (sys::File& f) const
{
  f << "RoI: " << roi_id() << " eta: " << eta() << " phi: " << phi()
    << " LVL1 ID: " << lvl1_id() << "\n";
  typedef std::vector<roiformat::Cell> vec_type;
  for (vec_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    f << *it;
  }
  RINGER_DEBUG2("Dumped RoI {LVL1ID: " << m_lvl1_id << " RoI: " 
	      << m_roi_id << "} into File.");
  return f;
}

bool roiformat::RoI::check (void) const
{
  //check for UNKNOWN calo
  typedef std::map<roiformat::Cell::Sampling, 
    std::vector<const roiformat::Cell*> > map_type;
  map_type::const_iterator mt = m_samp.find(roiformat::Cell::UNKNOWN);
  if ( mt != m_samp.end() ) {
    RINGER_DEBUG1("Found calo " << mt->second.size() << "cells of type UNKNOWN at"
		<< " {RoI: " << m_roi_id << " LVL1ID: " << m_lvl1_id);
    return false;
  }
  //check overlap of cells -- not implemented!
  return true;
}

sys::File& operator>> (sys::File& f, roiformat::RoI& r)
{
  return r << f;
}

sys::File& operator<< (sys::File& f, const roiformat::RoI& r)
{
  return r >> f;
}

