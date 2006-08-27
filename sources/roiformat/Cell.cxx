//Dear emacs, this is -*- c++ -*-

/**
 * @file roiformat/src/Cell.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Implements the Cell class
 */

#include "roiformat/Cell.h"
#include "sys/debug.h"
#include "sys/Exception.h"
#include <cmath>

roiformat::Cell::Cell (Sampling sampling, const double& eta, 
		       const double& phi, const double& r, 
		       const double& deta, const double& dphi,
		       const double& dr, const double& energy)
  : m_samp(sampling),
    m_eta(eta),
    m_phi(phi),
    m_r(r),
    m_deta(deta),
    m_dphi(dphi),
    m_dr(dr),
    m_energy(energy)
{
  //correct for the cases where values of phi are between -PI and +PI instead
  //of between 0 and 2*PI
  if (m_phi < 0) m_phi += 2*M_PI;

  RINGER_DEBUG3("New cell (scratch) {" << m_samp << "," << m_eta << ","
		<< m_phi << "," << m_r << "," << m_deta << "," << m_dphi
		<< "," << m_dr << "," << m_energy << "}");
}

sys::File& roiformat::Cell::operator<< (sys::File& f)
{
  char t; //comma, to ignore
  unsigned int sampling;
  f >> sampling >> t >> m_eta >> t >> m_phi >> t >> m_r >> t >> m_deta 
    >> t >> m_dphi >> t >> m_dr >> t >> m_energy;
  m_samp = (roiformat::Cell::Sampling)sampling;
  //correct for the cases where values of phi are between -PI and +PI instead
  //of between 0 and 2*PI
  if (m_phi < 0) m_phi += 2*M_PI;
  RINGER_DEBUG3("Cell read from File {" << m_samp << "," << m_eta << "," 
		<< m_phi << "," << m_r << "," << m_deta << "," << m_dphi
		<< "," << m_dr << "," << m_energy << "}");
  return f;
}

sys::File& operator>> (sys::File& f, roiformat::Cell& c)
{
  return c << f;
}

sys::File& operator<< (sys::File& f, const roiformat::Cell& c)
{
  std::string t = "";
  t += f.separator();
  f << c.sampling() << t << c.eta() << t << c.phi() << t << c.r() << t 
    << c.deta() << t << c.dphi() << t << c.dr() << t << c.energy() << "\n";
  RINGER_DEBUG3("Cell written to File {" << c.sampling() << "," << c.eta() 
		<< "," << c.phi() << "," << c.r() << "," << c.deta() << "," 
		<< c.dphi() << "," << c.dr() << "," << c.energy() << "}");
  return f;
}

std::ostream& operator<< (std::ostream& os, const roiformat::Cell& c)
{
  os << "{" << sampling2str(c.sampling()) << "; eta = " << c.eta() 
     << "; phi = " << c.phi() << "; " << " energy = " << c.energy() << " MeV}";
  return os;
}

roiformat::Cell::Sampling roiformat::str2sampling (const std::string& s)
{
  if (s == "PSBARREL") return Cell::PSBARREL;
  else if (s == "EMBARREL1") return Cell::EMBARREL1;
  else if (s == "EMBARREL2") return Cell::EMBARREL2;
  else if (s == "EMBARREL3") return Cell::EMBARREL3;
  else if (s == "PSENDCAP") return Cell::PSENDCAP;
  else if (s == "EMENDCAP1") return Cell::EMENDCAP1;
  else if (s == "EMENDCAP2") return Cell::EMENDCAP2;
  else if (s == "EMENDCAP3") return Cell::EMENDCAP3;
  else if (s == "HADENCAP0") return Cell::HADENCAP0;
  else if (s == "HADENCAP1") return Cell::HADENCAP1;
  else if (s == "HADENCAP2") return Cell::HADENCAP2;
  else if (s == "HADENCAP3") return Cell::HADENCAP3;
  else if (s == "TILEBARREL0") return Cell::TILEBARREL0;
  else if (s == "TILEBARREL1") return Cell::TILEBARREL1;
  else if (s == "TILEBARREL2") return Cell::TILEBARREL2;
  else if (s == "TILEGAPSCI0") return Cell::TILEGAPSCI0;
  else if (s == "TILEGAPSCI1") return Cell::TILEGAPSCI1;
  else if (s == "TILEGAPSCI2") return Cell::TILEGAPSCI2;
  else if (s == "TILEEXTB0") return Cell::TILEEXTB0;
  else if (s == "TILEEXTB1") return Cell::TILEEXTB1;
  else if (s == "TILEEXTB2") return Cell::TILEEXTB2;
  else if (s == "FORWCAL0") return Cell::FORWCAL0;
  else if (s == "FORWCAL1") return Cell::FORWCAL1;
  else if (s == "FORWCAL2") return Cell::FORWCAL2;
  return Cell::UNKNOWN;
}

std::string roiformat::sampling2str (const Cell::Sampling& s)
{
  std::string retval = "UNKNOWN";
  switch (s) {
  case Cell::PSBARREL:
    retval = "PSBARREL";
    break;
  case Cell::EMBARREL1:
    retval = "EMBARREL1";
    break;
  case Cell::EMBARREL2:
    retval = "EMBARREL2";
    break;
  case Cell::EMBARREL3:
    retval = "EMBARREL3";
    break;
  case Cell::PSENDCAP:
    retval = "PSENDCAP";
    break;
  case Cell::EMENDCAP1:
    retval = "EMENDCAP1";
    break;
  case Cell::EMENDCAP2:
    retval = "EMENDCAP2";
    break;
  case Cell::EMENDCAP3:
    retval = "EMENDCA3";
    break;
  case Cell::HADENCAP0:
    retval = "HADENCAP0";
    break;
  case Cell::HADENCAP1:
    retval = "HADENCAP1";
    break;
  case Cell::HADENCAP2:
    retval = "HADENCAP2";
    break;
  case Cell::HADENCAP3:
    retval = "HADENCAP3";
    break;
  case Cell::TILEBARREL0:
    retval = "TILEBARREL0";
    break;
  case Cell::TILEBARREL1:
    retval = "TILEBARREL1";
    break;
  case Cell::TILEBARREL2:
    retval = "TILEBARREL2";
    break;
  case Cell::TILEGAPSCI0:
    retval = "TILEGAPSCI0";
    break;
  case Cell::TILEGAPSCI1:
    retval = "TILEGAPSCI1";
    break;
  case Cell::TILEGAPSCI2:
    retval = "TILEGAPSCI2";
    break;
  case Cell::TILEEXTB0:
    retval = "TILEEXTB0";
    break;
  case Cell::TILEEXTB1:
    retval = "TILEEXTB1";
    break;
  case Cell::TILEEXTB2:
    retval = "TILEEXTB2";
    break;
  case Cell::FORWCAL0:
    retval = "FORWCAL0";
    break;
  case Cell::FORWCAL1:
    retval = "FORWCAL1";
    break;
  case Cell::FORWCAL2:
    retval = "FORWCAL2";
    break;
  default:
    retval = "UNKNOWN";
  }
  return retval;
} 

void roiformat::max (const std::vector<const roiformat::Cell*>& vcell, 
		     double& eta, double& phi)
{
  double current = 0.0;
  const roiformat::Cell* c = 0;
  //get at least the first cell, in case of panic (all zeroes for instance)
  if (vcell.begin() != vcell.end()) c = *(vcell.begin());
  for (std::vector<const roiformat::Cell*>::const_iterator it = vcell.begin();
       it != vcell.end(); ++it) {
    if ((*it)->energy() > current) {
      current = (*it)->energy();
      c = (*it);
    }
  }
  if (!c) {
    RINGER_DEBUG1("I couldn't find any cell with energy >= 0. Check your "
		  << "inputs again. Throwing exception...");
    throw RINGER_EXCEPTION("Cannot find maximum > 0.");
  }
  RINGER_DEBUG3("Peak energy found is " << c->energy() << " MeV.");
  eta = c->eta();
  phi = c->phi();
}


void roiformat::max (const std::vector<const roiformat::Cell*>& vcell, 
		     double& eta, double& phi, const double& eta_ref, 
		     const double& phi_ref, const double& eta_window, 
		     const double& phi_window)
{
  double current = 0.0;
  const roiformat::Cell* c = 0;
  const double etamin = eta_ref - (0.5 * eta_window);
  const double etamax = eta_ref + (0.5 * eta_window);
  const double phimin = phi_ref - (0.5 * phi_window);
  const double phimax = phi_ref + (0.5 * phi_window);

  //are we, possibly at the wrap-around region for phi?
  bool wrap = false;
  if (phi_ref > (TWO_PI - PI_THRESHOLD)) {
      wrap = true;
      RINGER_DEBUG3(phi_ref << " is greater than " 
		    << (TWO_PI - PI_THRESHOLD));
      RINGER_DEBUG3("Possible Ring window at the phi wrap around"
		    << " region *DETECTED*.");
  }
  bool reverse_wrap = false;
  if (phi_ref < PI_THRESHOLD) {
      reverse_wrap = true;
      RINGER_DEBUG3(phi_ref << " is smaller than " << PI_THRESHOLD);
      RINGER_DEBUG3("Possible (reverse) Ring window at the phi wrap around"
		    << " region *DETECTED*.");
  }

  //get at least the first cell, in case of panic (all zeroes for instance)
  for (std::vector<const roiformat::Cell*>::const_iterator it = vcell.begin();
       it != vcell.end(); ++it) {
    //first we check the location, taking into consideration the
    //window around the position given
    double phi_use = (*it)->phi(); //use this value for phi (wrap protection)
    if ( wrap && ((*it)->phi() < M_PI) ) phi_use += TWO_PI;
    if ( reverse_wrap && ((*it)->phi() > M_PI) ) phi_use -= TWO_PI;
    if ((*it)->eta() > etamin && (*it)->eta() < etamax && 
	  phi_use > phimin && phi_use < phimax) {
      //if that works, check if this is the first cell to get here, or
      //actually there is more energy
      if (!c || (*it)->energy() > current) {
	c = (*it);
	current = (*it)->energy();
      }
    }
  }
  if (!c) {
    RINGER_DEBUG1("I couldn't find any cell with energy >= 0. Check your " 
		  << "inputs again. Using EM2 center.");
    eta = eta_ref;
    phi = phi_ref;
  }
  else {
    RINGER_DEBUG3("Peak energy found is " << c->energy() << " MeV.");
    eta = c->eta();
    phi = c->phi();
  }
}
