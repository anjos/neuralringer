//Dear emacs, this is -*- c++ -*-

/**
 * @file prog/getroi.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Rabello dos Anjos</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Gets a specific RoI from a file and dump it in a nicely readable format,
 * which could be easily interpreted by any statistical analysis program.
 */

#include "roiformat/Database.h"
#include "sys/debug.h"
#include "sys/util.h"
#include <cstdlib>
#include "sys/Reporter.h"

/**
 * Does all the work
 */
int main (int argc, char** argv) 
{
  sys::Reporter reporter("local");
  if (argc != 4) {
    RINGER_FATAL(reporter, "usage: " << argv[0] 
		 << " <LVL1 Id> <RoI Id> <filename.rfd>");
  }

  unsigned int lvl1_id = strtoul(argv[1], 0, 0);
  unsigned int roi_id = strtoul(argv[2], 0, 0);
  std::string filename = argv[3];
  std::string outfile = "roi-";
  outfile = outfile + argv[1] + "-" + argv[2] + ".rfd";

  RINGER_REPORT(reporter, "Re-dumping event " << lvl1_id << "/" 
	        << roi_id << " from file " << filename << "...");
  roiformat::Database db(reporter);
  if (!sys::exists(filename)) {
    RINGER_FATAL(reporter, "Input file " << filename << " doesn't exist.");
  }
  db.load(filename);
  const roiformat::RoI* roi = db.get(lvl1_id, roi_id);
  if (!roi) {
    RINGER_FATAL(reporter, 
		 "The RoI with these specs doesn't exist in the file!");
  }
  
  //dump only the cells, no identifiers now
  sys::File out (outfile, std::ios_base::out|std::ios_base::trunc, ',');
  std::string header = "sampling,eta,phi,r,deta,dphi,dr,energy\n";
  out << header; 
  std::vector<const roiformat::Cell*> cells;
  roi->cells(cells);
  for (std::vector<const roiformat::Cell*>::const_iterator it=cells.begin();
       it != cells.end(); ++it) {
    out << **it;
  }
  RINGER_REPORT(reporter, "Just finished.");
}



