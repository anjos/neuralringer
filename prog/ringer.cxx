//Dear emacs, this is -*- c++ -*-

/**
 * @file ringer.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Arranges calorimetry cell data (roiformat dump) in concentric rings of
 * configurable size and center and dump an XML file containing the ring'ified
 * information for every RoI and their metadata (LVL1 and RoI identifiers and
 * etaxphi information).
 */

#include "rbuild/Config.h"
#include "rbuild/RingSet.h"
#include "roiformat/Database.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/debug.h"
#include "sys/util.h"
#include "data/RoIPatternSet.h"
#include "data/SumExtractor.h"
#include "data/Database.h"
#include <iostream>
#include <popt.h>
#include <cstdio>
#include <cmath>
#include <ctime>

/**
 * Defines the energy threshold for divisions, in MeV's
 */
const double ENERGY_THRESHOLD = 0.001;

/**
 * Returns the basename of a file, without its extension and diretory prefixes
 * 
 * @param fullname The name of the file
 */
std::string stripname (const std::string& fullname)
{
  //find location of last '/'
  size_t start = fullname.rfind("/");
  if (start == std::string::npos) start = 0;
  else ++start;
  //stopped here
  size_t end = fullname.rfind(".rfd");
  return fullname.substr(start, end-start);
}

typedef struct param_t {
  std::string roidump; ///< roi dump file to read data from 
  std::string ringconfig; ///< ring configuration XML file
  bool global_center; ///< should I calculate a peak on each calo layer?
} param_t;

/**
 * Checks and validates program options.
 *
 * @param argc The number of arguments given to the program execution
 * @param argv The arguments given to program execution
 * @param p The parameters, already parsed
 * @param reporter The reporter to use when reporting problems to the user
 */
bool checkopt (int& argc, char**& argv, param_t& p, sys::Reporter& reporter)
{
  char* ringconfig=0;
  char* roidump=0;
  int global_center=0;
  //return val is kept to zero always so popt processes everything
  //automatically
  struct poptOption optionsTable[] = {
    { "ring-config", 'c', POPT_ARG_STRING, &ringconfig, 0,
      "location of the Ring Configuration XML file to use", "path" },
    { "global-center", 'g', POPT_ARG_NONE, &global_center, 0,
      "Use etaxphi center as provided by layer 2 (default is false)", "" },
    { "roi-dump", 'r', POPT_ARG_STRING, &roidump, 0,
      "location of the RoI dumpfile to read data from", "path" },
    POPT_AUTOHELP
    { 0, 0, 0, 0, 0 }
  };

  poptContext optCon = poptGetContext(NULL, argc, (const char**)argv,
				      optionsTable, 0);

  if (argc < 2) {
    poptPrintUsage(optCon, stderr, 0);
    return false;
  }

  char c = poptGetNextOpt(optCon);

  if (c < -1) {
    /* an error occurred during option processing */
    RINGER_FATAL(reporter, "Error during option processing with popt! "
		 << poptBadOption(optCon, POPT_BADOPTION_NOALIAS) << ": "
		 << poptStrerror(c));
  }

  //copy all
  p.roidump = roidump;
  p.ringconfig = ringconfig;
  p.global_center = (global_center)?true:false;

  if (!p.roidump.size()) {
    poptPrintUsage(optCon, stderr, 0);
    throw RINGER_EXCEPTION("No dump to read data from");
  }
  if (!sys::exists(p.roidump)) {
    RINGER_DEBUG1("RoI dump file " << p.roidump << " doesn't exist.");
    throw RINGER_EXCEPTION("RoI dump file doesn't exist");
  }
  if (!p.ringconfig.size()) {
    poptPrintUsage(optCon, stderr, 0);
    throw RINGER_EXCEPTION("No ring configuration file");
  }
  if (!sys::exists(p.ringconfig)) {
    RINGER_DEBUG1("Ring config file " << p.ringconfig << " doesn't exist.");
    throw RINGER_EXCEPTION("Ring config file doesn't exist");
  }
  poptFreeContext(optCon);

  RINGER_DEBUG1("Command line options have been read.");
  RINGER_REPORT(reporter, "Using ring-config=\"" << p.ringconfig
		<< "\"; roi-dump=\"" << p.roidump << "; global-center=\""
		<< p.global_center << "\".");
  return true;
}

/**
 * Calculates and applies the sequential normalisation vector that results
 * from the following arithmetic:
 *
 * @f[
 * N0 = E
 * N1 = E - E0
 * N2 = E - E0 - E1 = N1 - E1
 * N3 = E - E0 - E1 - E2 = N2 - E2
 * ...
 * NX = N(X-1) - E(X-1)
 * @f]
 *
 * Where Ni are the normalisation factors, E is the total layer energy and Ei
 * are the energy values for each ring. 
 *
 * @param rings The calculated ring values one wishes to normalise
 * @param stop_energy The threshold to judge when to stop this normalisation
 * strategy and start using the total layer energy instead, in MeV.
 */
void sequential (data::Pattern& rings, const data::Feature& stop_energy=100.0)
{
  //if the ENERGY_THRESHOLD is greater than `stop', use stop instead.
  data::Feature stop = stop_energy;
  if (ENERGY_THRESHOLD > stop) {
    RINGER_DEBUG1("I detected that the \"stop\" value is *smaller* than the"
		  << " global energy threshold (" << stop_energy
		  << " < " << ENERGY_THRESHOLD << "). This is an invalid"
		  << " consideration. I'll overwrite the \"stop\" value with"
		  << " the energy threshold and proceed.");
    stop = ENERGY_THRESHOLD;
  }

  data::Pattern norm(rings.size(), 0);
  data::SumExtractor sum;
  norm[0] = sum(rings);
  //if the sum is less than stop, apply layer normalisation to all rings
  if (std::fabs(norm[0]) < stop) {

    //if the sum is even less than the threshold, do not apply any
    //normalisation at all here!
    if (std::fabs(norm[0]) < ENERGY_THRESHOLD) {
      RINGER_DEBUG2("Layer sum is less than \"" << ENERGY_THRESHOLD << "\","
		    << " skipping normalisation for this set.");
      return;
    }
    RINGER_DEBUG2("Layer sum is less than \"" << stop << "\","
		  << " normalising all rings with total layer energy.");
    rings /= norm[0];
    return;
  }

  //I'm ok to proceed with the calculations
  bool fixed = false; //shall I stop to apply a variant normalisation factor?
  for (unsigned int i=1; i<rings.size(); ++i) {
    norm[i] = norm[i-1] - rings[i-1];
    //if the normalization factor is less than `stop', just apply a fixed,
    //known to be great enough, value instead of that. Otherwise, the noise
    //will get too amplified.
    if (fixed || norm[i] < stop) {
      norm[i] = norm[0];
      if (!fixed) {
	fixed = true;
	RINGER_DEBUG2("Stopped sequential normalisation for values starting"
		      << " from ring[" << i << "], because my stop value is"
		      << " set to \"" << stop 
		      << "\" MeV and the factors got to that value.");
      }
    }
  }
  rings /= norm;
}

/**
 * Calculates the center of interation based on the second e.m. layer
 * information or return false, indicating a center could not be found.
 *
 * @param reporter A system-wide reporter to use
 * @param roi The RoI to study
 * @param eta The eta value calibrated to the center
 * @param phi The phi value calibrated to the center
 *
 * @return <code>true</code> if everything goes Ok, or <code>false</code>
 * otherwise.
 */
bool find_center(sys::Reporter& reporter,
		 const roiformat::RoI* roi, double& eta, double& phi)
{
  std::vector<const roiformat::Cell*> cells;
  roi->cells(roiformat::Cell::EMBARREL2, cells);
  roi->cells(roiformat::Cell::EMENDCAP2, cells);
  if (!cells.size()) {
    RINGER_REPORT(reporter,
		  "I couldn't find any cells for layer e.m. second layer" 
		  << " in RoI" << " with L1Id #" << roi->lvl1_id() 
		  << " and RoI #" << roi->roi_id());
    return false;
  }
  RINGER_DEBUG2("I've found " << cells.size() << " at e.m. second layer...");
  roiformat::max(cells, eta, phi);
  RINGER_DEBUG2("The maximum happens at eta=" << eta << " and phi=" << phi);
  return true;
}

/**
 * Calculates based on the RoI input and on the center previously calculated.
 *
 * @param reporter A system-wide reporter to use
 * @param roi The RoI dump to use as starting point
 * @param rset The ring set configuration to use for creating the rings
 * @param own_center If this value is set to <code>false</code> a layer based
 * center is calculated for the ring center. Otherwise, the values given on
 * the following variables are considered.
 * @param eta The center to consider when building the rings
 * @param phi The center to consider when building the rings
 */
void build_rings(sys::Reporter& reporter,
		 const roiformat::RoI* roi,
		 std::vector<rbuild::RingSet>& rset, bool own_center,
		 const double& eta, const double& phi)
{
  //for each RingSet (calculate primary ring values, w/o normalization)
  for (std::vector<rbuild::RingSet>::iterator 
	 jt=rset.begin(); jt!=rset.end(); ++jt) {
    jt->reset(); //reset this ringset
    const std::vector<roiformat::Cell::Sampling>& dets = 
      jt->config().detectors();
    //for all detectors in a given RingSet
    std::vector<const roiformat::Cell*> cells;
    for (std::vector<roiformat::Cell::Sampling>::const_iterator 
	   kt=dets.begin(); kt!=dets.end(); ++kt) {
      //get all cells for that RoI, that are from detectors I'm interested
      roi->cells(*kt, cells);
    } //for relevant detectors

    if (!cells.size()) {
      RINGER_REPORT(reporter, "I couldn't find any cells for ring set \""
		    << jt->config().name() << "\" in RoI"
		    << " with L1Id #" << roi->lvl1_id() 
		    << " and RoI #" << roi->roi_id());
      continue;
    }
    RINGER_DEBUG2("I've found " << cells.size() << " cells for ring set"
		  << " \"" << jt->config().name() << "\"...");

    //add the ring values for those cells, based on the center given or
    //calculate its own center.
    if (!own_center) {
      double my_eta, my_phi;
      roiformat::max(cells, my_eta, my_phi);
      jt->add(cells, my_eta, my_phi);
    }
    else jt->add(cells, eta, phi);

  } //for each RingSet
}

/**
 * Apply normalization based on the ring set configuration
 *
 * @param reporter A system-wide reporter to use
 * @param rset The ring set configuration to use for creating the rings
 */
void normalize_rings(sys::Reporter& reporter,
		     std::vector<rbuild::RingSet>& rset)
{
  //at this point, I have all ring sets, separated
  double emsection = 0; // energy at e.m. section
  double hadsection = 0; // energy at hadronic section

  //for each RingSet (first iteration) -- apply set dependent norms.
  for (std::vector<rbuild::RingSet>::iterator 
	 jt=rset.begin(); jt!=rset.end(); ++jt) {
    //calculate the relevant energies
    double setenergy = 0;
    for (size_t i=0; i<jt->pattern().size(); ++i) 
      setenergy += jt->pattern()[i];
    if (jt->config().section() == rbuild::RingConfig::EM) 
      emsection += setenergy;
    else hadsection += setenergy;

    //what is the normalisation strategy here? Can I do something already?
    switch(jt->config().normalisation()) {
    case rbuild::RingConfig::SET:
      if (setenergy > ENERGY_THRESHOLD)
	jt->pattern() /= fabs(setenergy);
      else {
	RINGER_WARN(reporter, "Ignoring normalisation command \""
		    << norm2str(jt->config().normalisation())
		    << "\" for RingSet \"" << jt->config().name() 
		    << " because the denominator is bellow the " 
		    << ENERGY_THRESHOLD << " MeV threshold (value="
		    << setenergy << ").");
      }
      break;
    case rbuild::RingConfig::SEQUENTIAL:
      sequential(jt->pattern());
      break;
    default: //do nothing
      break;
    }
    RINGER_DEBUG1("Set energy for \"" << jt->config().name() << "\" = "
		  << setenergy);
  } //for each RingSet (first iteration)

  double event = emsection + hadsection; // event energy
  RINGER_DEBUG1("Event energy = " << event);
  RINGER_DEBUG1("E.m. energy = " << emsection);
  RINGER_DEBUG1("Hadronic energy = " << hadsection);

  //for each RingSet (third iteration) -- now accumulate and store
  for (std::vector<rbuild::RingSet>::iterator
	 jt=rset.begin(); jt!=rset.end(); ++jt) {
    //what is the normalisation strategy here? Do the rest of options
    switch(jt->config().normalisation()) {
    case rbuild::RingConfig::EVENT:
      if (event > ENERGY_THRESHOLD) jt->pattern() /= fabs(event);
      else {
	RINGER_WARN(reporter, "Ignoring normalisation command \""
		    << norm2str(jt->config().normalisation())
		    << "\" for RingSet \"" << jt->config().name() 
		    << " because the denominator is bellow the " 
		    << ENERGY_THRESHOLD << " MeV threshold (value="
		    << event << ").");
      }
      break;
    case rbuild::RingConfig::SECTION:
      if (jt->config().section() == rbuild::RingConfig::EM) {
	if (emsection > ENERGY_THRESHOLD) 
	  jt->pattern() /= fabs(emsection);
	else {
	  RINGER_WARN(reporter, "Ignoring normalisation command \""
		      << norm2str(jt->config().normalisation())
		      << "\" for RingSet \"" << jt->config().name() 
		      << " because the denominator is bellow the "
		      << ENERGY_THRESHOLD << " MeV threshold (value="
		      << emsection << ").");
	}
      }
      else {
	if (hadsection > ENERGY_THRESHOLD) jt->pattern() /= fabs(hadsection);
	else {
	  RINGER_WARN(reporter, "Ignoring normalisation command \""
		      << norm2str(jt->config().normalisation())
		      << "\" for RingSet \"" << jt->config().name() 
		      << " because the denominator is bellow the "
		      << ENERGY_THRESHOLD << " MeV threshold (value="
		      << hadsection << ").");
	}
      }
      break;
    default: //do nothing
      break;
    }
  }
}

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  param_t par;

  try {
    if (!checkopt(argc, argv, par, reporter))
      RINGER_FATAL(reporter, "Terminating execution.");
  }
  catch (sys::Exception& ex) {
    RINGER_EXCEPT(reporter, ex.what());
    RINGER_FATAL(reporter, "I can't handle that exception. Aborting.");
  }

  //start processing
  std::string bname = stripname(par.roidump);

  try {

    //load roidump first
    roiformat::Database roidump(reporter);
    roidump.load(par.roidump);
    RINGER_REPORT(reporter, "Loaded RoI dump database at \"" << par.roidump 
		  << "\".");
    //then config
    rbuild::Config config(par.ringconfig, reporter);
    RINGER_REPORT(reporter, "Loaded Ring configuration at \""
		  << par.ringconfig << "\".");
    //do the processing:
    //1. For each configured ring set, create a *real* RingSet
    std::vector<rbuild::RingSet> rset;
    const std::map<unsigned int, rbuild::RingConfig>& rconfig = 
      config.config();
    unsigned int nrings = 0;
    for (std::map<unsigned int, rbuild::RingConfig>::const_iterator 
	   it=rconfig.begin(); it!=rconfig.end(); ++it) {
      rset.push_back(it->second);
      nrings += it->second.max();
    } //creates, obligatorily, ordered ring sets

    //2. Pass the relevant cells through the relevant RingSet(s).
    std::vector<const roiformat::RoI*> rois;
    roidump.rois(rois); //get a handle to all RoI's available
    data::RoIPatternSet ringdb(roidump.size(), nrings);

    size_t i=0;
    for (std::vector<const roiformat::RoI*>::const_iterator
	   it=rois.begin(); it!=rois.end(); ++it) {
      RINGER_REPORT(reporter, "Calculating rings for RoI -> L1Id #"
		    << (*it)->lvl1_id() << " and RoI #" << (*it)->roi_id());
      double eta, phi; //center values
      bool ok = find_center(reporter, *it, eta, phi);
      // only use layer 2 center, if an explicit request was made
      ok &= par.global_center; 
      build_rings(reporter, *it, rset, ok, eta, phi);
      normalize_rings(reporter, rset);

      //extra "magic" to place the entry at the output DB
      bool rings_not_init = true;
      data::Pattern these_rings(1);
      for (std::vector<rbuild::RingSet>::iterator 
	     jt=rset.begin(); jt!=rset.end(); ++jt) {
	//Now store the values as a data::Pattern before returning
	if (rings_not_init) {
	  these_rings = jt->pattern();
	  rings_not_init = false;
	}
	else these_rings.append(jt->pattern());
      } //for each RingSet (third iteration)

      data::RoIPatternSet::RoIAttribute attr;
      attr.lvl1_id = (*it)->lvl1_id();
      attr.roi_id = (*it)->roi_id();
      attr.eta = (*it)->eta();
      attr.phi = (*it)->phi();
      ringdb.set_pattern(i, these_rings, attr);
      ++i;

    } //for all RoI's

    //prepare the database to dump
    std::string comment = "Originated from file ";
    comment += par.roidump;
    data::Header h("Andre ANJOS", bname, "2.0", time(0), comment);
    std::map<std::string, data::RoIPatternSet*> psmap;
    psmap[bname] = &ringdb;
    data::Database<data::RoIPatternSet> db(&h, psmap, reporter);
    std::string outfilename = bname;
    outfilename += "-rings.xml";
    db.save(outfilename);
    RINGER_REPORT(reporter, "Output file \"" << outfilename
		  << "\" was correctly saved and closed.");

  } //try clause

  catch (sys::Exception& ex) {
    RINGER_FATAL(reporter, ex.what());
  }
  //dump ring cells in plain text format
  RINGER_REPORT(reporter, "Successful exit. Bye");
}




