//Dear emacs, this is -*- c++ -*-

/**
 * @file eta-filter.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author$
 * $Revision$
 * $Date$
 *
 * Filters an RoI database based on the eta locations of every object.
 */

#include "data/Database.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/debug.h"
#include "sys/File.h"
#include "sys/util.h"
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <popt.h>
#include <sstream>
#include <cmath>

/**
 * Returns the basename of a file, without its extension and diretory prefixes
 * 
 * @param fullname The name of the file
 * @param ext The extension to strip
 */
std::string stripname (const std::string& fullname, const std::string& ext)
{
  //find location of last '/'
  size_t start = fullname.rfind("/");
  if (start == std::string::npos) start = 0;
  else ++start;
  //stopped here
  size_t end = fullname.rfind(ext);
  return fullname.substr(start, end-start);
}

typedef struct param_t {
  std::string db; ///< database to apply the filtering
  double eta; ///< the absolute eta location to use for splitting
  std::string less_db; ///< the db output for objects with smaller eta value
  std::string greater_db; ///< the db output for objects with greater eta value
  std::string e_loc; ///< the db name with eta locations for electrons
  std::string j_loc; ///< the db name with eta locations for jets
  std::string e_loc_less; ///< where to dump new electron locations (less)
  std::string e_loc_greater; ///< where to dump new electron loc. (greater)
  std::string j_loc_less; ///< where to dump new jet locations (less)
  std::string j_loc_greater; ///< where to dump new jet locations (greater)
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
  //defaults for each option
  char* db=0;
  double eta=0;
  char* e_loc=0;
  char* j_loc=0;

  //return `arg' is set to !=0, so the system processes everything in the
  //while loop bellow.
  struct poptOption optionsTable[] = {
    { "db", 'd', POPT_ARG_STRING, &db, 'd',
      "location of the database to split", "path" },
    { "eta", 'e', POPT_ARG_DOUBLE, &eta, 'e',
      "where to split the database", "double > 0" },
    { "jet-location", 'j', POPT_ARG_STRING, &j_loc, 'j',
      "file containing the location for jets",
      "path: default is jet-location.txt" },
    { "electron-location", 'k', POPT_ARG_STRING, &e_loc, 'k',
      "file containing the location for electrons",
      "path: default is electron-location.txt" },
    POPT_AUTOHELP
    { 0, 0, 0, 0, 0 }
  };

  poptContext optCon = poptGetContext(NULL, argc, (const char**)argv,
				      optionsTable, 0);

  if (argc == 1) {
    poptPrintUsage(optCon, stderr, 0);
    return false;
  }

  char c;
  while ((c = poptGetNextOpt(optCon)) > 0) {
    switch (c) {
    case 'd': //db
      RINGER_DEBUG1("Database name is " << db);
      if (!sys::exists(db)) {
	RINGER_DEBUG1("Database file " << db << " doesn't exist.");
	throw RINGER_EXCEPTION("Database file doesn't exist");
      }
      break;
    case 'e': //eta threshold
      if (eta < 0) {
	RINGER_DEBUG1("Setting the eta threshold to less than zero is"
		      << " prohibted. Exception thrown.");
	throw RINGER_EXCEPTION("Forbidden eta threshold");
      }
      RINGER_DEBUG1("Eta threshold set to " << eta);
      break;
    case 'j': //jet locations
      RINGER_DEBUG1("Jet locations will be fetched from " << j_loc);
      break;
    case 'k': //electron locations
      RINGER_DEBUG1("Electron locations will be fetched from " << e_loc);
      break;
    }
  }

  if (c < -1) {
    /* an error occurred during option processing */
    RINGER_FATAL(reporter, "Error during option processing with popt! "
		 << poptBadOption(optCon, POPT_BADOPTION_NOALIAS) << ": "
		 << poptStrerror(c));
  }

  //checks
  if (!db) {
    RINGER_DEBUG1("I cannot work without a database file. Exception thrown.");
    throw RINGER_EXCEPTION("No database file specified");
  } else p.db = db;
  if (!eta) {
    RINGER_DEBUG1("I cannot work with an eta threshold of ZERO."
		  << " Exception thrown.");
    throw RINGER_EXCEPTION("Valid eta threshold specified?");
  } else p.eta = eta;
  std::string prefix = stripname(db, ".xml");
  std::ostringstream eta_str;
  eta_str << eta;
  p.less_db = prefix + ".eta-lt-" + eta_str.str() + ".xml";
  p.greater_db = prefix + ".eta-ge-" + eta_str.str() + ".xml";
  if (!e_loc) {
    p.e_loc = "electron-location.txt";
  } else p.e_loc = e_loc;
  p.e_loc_less = stripname(p.e_loc, ".txt") + ".eta-lt-" + 
    eta_str.str() + ".txt";
  p.e_loc_greater = stripname(p.e_loc, ".txt") + ".eta-ge-" + 
    eta_str.str() + ".txt";
  if (!j_loc) {
    p.j_loc = "jet-location.txt";
  } else p.j_loc = j_loc;
  p.j_loc_less = stripname(p.j_loc, ".txt") + ".eta-lt-" + 
    eta_str.str() + ".txt";
  p.j_loc_greater = stripname(p.j_loc, ".txt") + ".eta-ge-" +
    eta_str.str() + ".txt";
  poptFreeContext(optCon);

  RINGER_DEBUG1("Command line options have been read.");
  return true;
}

typedef struct loc_t {
  double eta; ///< the eta location of an object
  double phi; ///< the phi location of an object
  std::string original; ///< the name of the originating zebra file
} loc_t;

/**
 * Loads a set of RoI locations from a file, preserving the order and the eta
 * values for each entry
 *
 * @param fname The filename containing the eta locations
 * @param m The map where to put the order and the locations
 */
void load_locations (const std::string& fname, 
		     std::map<unsigned int, loc_t>& m)
{
  sys::File in(fname);
  unsigned int order=0;
  while (!in.eof() && in.good()) {
    loc_t loc;
    in >> loc.eta;
    if (in.eof()) break;
    in >> loc.phi >> loc.original;
    m[order] = loc;
    ++order;
  }
}

/**
 * Saves a set of locations in a new File
 *
 * @param fname The filename where the locations will be saved to
 * @param m The map to dump into that file
 */
void save_locations (const std::string& fname,
		     const std::map<unsigned int, loc_t>& m)
{
  sys::File out(fname, std::ios_base::out|std::ios_base::trunc, ' ');
  std::string sep;
  sep += out.separator();
  for (std::map<unsigned int, loc_t>::const_iterator it = m.begin();
       it != m.end(); ++it) {
    out << it->second.eta << sep
	<< it->second.phi << sep
	<< it->second.original << "\n";
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

  try {
    //loads the DB
    data::Database db(par.db, reporter);
    
    //checks db size
    if (db.size() < 2) {
      RINGER_FATAL(reporter, "The database you loaded contains only 1 class of"
		   " events. Please, reconsider your input file.");
    }
    
    //loads the electron locations
    std::map<unsigned int, loc_t> electron_loc;
    load_locations(par.e_loc, electron_loc);
    RINGER_REPORT(reporter, "Loaded " << electron_loc.size() << " electron "
		  << "locations from file " << par.e_loc);
    
    //get indexes for electrons
    std::vector<unsigned int> e_less;
    std::vector<unsigned int> e_greater;
    std::map<unsigned int, loc_t> electron_loc_less;
    std::map<unsigned int, loc_t> electron_loc_greater;
    for (std::map<unsigned int, loc_t>::const_iterator
	   it = electron_loc.begin(); it != electron_loc.end(); ++it) {
      if (std::fabs(it->second.eta) < par.eta) {
	e_less.push_back(it->first);
	electron_loc_less[it->first] = it->second;
      }
      else {
	e_greater.push_back(it->first);
	electron_loc_greater[it->first] = it->second;
      }
    }
    RINGER_REPORT(reporter, "There are " << e_less.size() << " electrons"
		  << " with |eta| less than " << par.eta << " and "
		  << e_greater.size() << " electrons with eta greater or"
		  << " equal to that value.");
    
    //loads the jet locations
    std::map<unsigned int, loc_t> jet_loc;
    load_locations(par.j_loc, jet_loc);
    RINGER_REPORT(reporter, "Loaded " << jet_loc.size() << " jet "
		  << "locations from file " << par.j_loc);
    
    //get indexes for jets
    std::vector<unsigned int> j_less;
    std::vector<unsigned int> j_greater;
    std::map<unsigned int, loc_t> jet_loc_less;
    std::map<unsigned int, loc_t> jet_loc_greater;
    for (std::map<unsigned int, loc_t>::const_iterator
	   it = jet_loc.begin(); it != jet_loc.end(); ++it) {
      if (std::fabs(it->second.eta) < par.eta) {
	j_less.push_back(it->first);
	jet_loc_less[it->first] = it->second;
      }
      else {
	j_greater.push_back(it->first);
	jet_loc_greater[it->first] = it->second;
      }
    }
    RINGER_REPORT(reporter, "There are " << j_less.size() << " jets"
		  << " with |eta| less than " << par.eta << " and "
		  << j_greater.size() << " jets with eta greater or"
		  << " equal to that value.");
    
    //build output data sets for electrons
    const data::PatternSet* electrons = db.data("electron");
    if (electrons->size() > e_less.size() + e_greater.size()) {
      RINGER_WARN(reporter, "(electrons) The sizes of final DB's ("
		  << e_less.size() + e_greater.size() << ") will be smaller"
		  << " than the original DB size (" << electrons->size() 
		  << ").");
    }
    if (electrons->size() < e_less.size() + e_greater.size()) {
      RINGER_FATAL(reporter, "(electrons) The sizes of final DB's ("
		   << e_less.size() + e_greater.size() << ") will be bigger"
		   << " than the original DB size (" << electrons->size() 
		   << ")? Impossible! Aborting.");
    }
    data::PatternSet e_less_ps(*electrons, e_less);
    data::PatternSet e_greater_ps(*electrons, e_greater);
    
    //build output data sets for jets
    const data::PatternSet* jets = db.data("jet");
    if (jets->size() > j_less.size() + j_greater.size()) {
      RINGER_WARN(reporter, "(jets) The sizes of final DB's ("
		  << j_less.size() + j_greater.size() << ") will be smaller"
		  << " than the original DB size (" << jets->size() 
		  << ").");
    }
    if (jets->size() < j_less.size() + j_greater.size()) {
      RINGER_WARN(reporter, "(jets) The sizes of final DB's ("
		  << j_less.size() + j_greater.size() << ") will be greater"
		  << " than the original DB size (" << jets->size() 
		  << ")? Impossible! Aborting.");
    }
    data::PatternSet j_less_ps(*jets, j_less);
    data::PatternSet j_greater_ps(*jets, j_greater);
    
    //dump new databases at specified locations
    std::map<std::string, data::PatternSet*> less_db_data;
    less_db_data["electron"] = &e_less_ps;
    less_db_data["jet"] = &j_less_ps;
    std::map<std::string, data::PatternSet*> greater_db_data;
    greater_db_data["electron"] = &e_greater_ps;
    greater_db_data["jet"] = &j_greater_ps;
    std::ostringstream less_comment;
    less_comment << "DB for objects with eta less than " << par.eta << ".";
    data::Header less_header("Andre DOS ANJOS", "Filtered database on eta",
			     "1.0", time(0), less_comment.str());
    data::Database less_db(&less_header, less_db_data, reporter);
    less_db.save(par.less_db);
    RINGER_REPORT(reporter, "Saved new database " << par.less_db);
    std::ostringstream greater_comment;
    less_comment << "DB for objects with eta greater or equal to " 
		 << par.eta << ".";
    data::Header greater_header("Andre DOS ANJOS", "Filtered database on eta",
				"1.0", time(0), greater_comment.str());
    data::Database greater_db(&greater_header, greater_db_data, reporter);
    greater_db.save(par.greater_db);
    RINGER_REPORT(reporter, "Saved new database " << par.greater_db);

    save_locations(par.e_loc_less, electron_loc_less);
    RINGER_REPORT(reporter, "Saved location file " << par.e_loc_less);
    save_locations(par.e_loc_greater, electron_loc_greater);
    RINGER_REPORT(reporter, "Saved location file " << par.e_loc_greater);
    save_locations(par.j_loc_less, jet_loc_less);
    RINGER_REPORT(reporter, "Saved location file " << par.j_loc_less);
    save_locations(par.j_loc_greater, jet_loc_greater);
    RINGER_REPORT(reporter, "Saved location file " << par.j_loc_greater);
  }
  catch (const sys::Exception& ex) {
    RINGER_EXCEPT(reporter, ex.what());
    RINGER_FATAL(reporter, 
		 "This was a top-level catch for a RINGER exception, "
	       << "I have to exit, bye.");
  }
  catch (const std::exception& ex) {
    RINGER_EXCEPT(reporter, ex.what());
    RINGER_FATAL(reporter, "This was a top-level catch for a std exception, "
	       << "I have to exit, bye.");
  }
  catch (...) {
    RINGER_FATAL(reporter, 
		 "This was a top-level catch for a unknown exception, "
		 << "I have to exit, bye.");
  }
}
