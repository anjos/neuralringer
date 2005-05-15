//Dear emacs, this is -*- c++ -*-

/**
 * @file mlp-run.cxx
 *
 * Runs one or more databases through a neural network.
 */

#include "data/RoIPatternSet.h"
#include "data/Database.h"
#include "data/util.h"
#include "network/MLP.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/debug.h"
#include "sys/util.h"
#include "config/NeuronBackProp.h"
#include "config/SynapseBackProp.h"
#include "config/type.h"
#include "config/Header.h"
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <popt.h>
#include <sstream>

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
  size_t end = fullname.rfind(".xml");
  return fullname.substr(start, end-start);
}

typedef struct param_t {
  std::string db; ///< database to use for testing
  std::string net; ///< name of the neural net file
  std::string output; ///< where to save the output 
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
  char* net=0;
  char* output=0;

  //return `arg' is set to !=0, so the system processes everything in the
  //while loop bellow.
  struct poptOption optionsTable[] = {
    { "db", 'd', POPT_ARG_STRING, &db, 'd',
      "location of the database to use for training and testing", "path" },
    { "net", 'n', POPT_ARG_STRING, &net, 'n',
      "where to read the network", "path: no default" },
    { "output", 'o', POPT_ARG_STRING, &output, 'o',
      "where to write the output of the MLP neural-network", 
      "path: default is db-name.out.xml" },
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
    case 'n': //net name
      RINGER_DEBUG1("Network file set to " << net);
      break;
    case 'o': //output file name
      RINGER_DEBUG1("Output file set to " << output);
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
  } 
  p.db = db;
  if (!net) {
    RINGER_DEBUG1("I cannot work without a network file. Exception thrown.");
    throw RINGER_EXCEPTION("No network file specified");
  } 
  p.net = net;
  if (!output) {
    p.output = stripname(p.db) + ".out.xml";
    RINGER_DEBUG1("Setting output file name to " << p.output);
  } 
  else p.output = output;
  poptFreeContext(optCon);

  RINGER_DEBUG1("Command line options have been read.");
  return true;
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

  //loads the DB
  data::Database<data::RoIPatternSet> db(par.db, reporter);

  //loads the Network
  RINGER_REPORT(reporter, "Loading network \"" << par.net << "\"...");
  network::Network net(par.net, reporter);
  
  try {
    std::map<std::string, data::RoIPatternSet*> outdb_data;
    std::vector<std::string> input_class_names;
    db.class_names(input_class_names);
    for (std::vector<std::string>::const_iterator it = 
	   input_class_names.begin(); it != input_class_names.end(); ++it) {
      RINGER_REPORT(reporter, "Processing DB class \"" << *it << "\"...");
      data::SimplePatternSet output(db.data(*it)->size(), net.output_size());
      net.run(db.data(*it)->simple(), output);
      data::RoIPatternSet* roi_output = 
	new data::RoIPatternSet(output, db.data(*it)->attributes());
      std::ostringstream oss;
      oss << *it << "-output";
      outdb_data[oss.str()] = roi_output;
      RINGER_REPORT(reporter, "DB class \"" << *it 
		    << "\" output was saved was \"" << oss.str() << "\".");
    }
    std::ostringstream comment;
    comment << "Outputs from " << par.db << " ran through " << par.net;
    data::Header header("Andre DOS ANJOS", par.output, "1.0", time(0),
			comment.str());
    data::Database<data::RoIPatternSet> 
      output_db(&header, outdb_data, reporter);
    output_db.save(par.output);
    RINGER_REPORT(reporter, "All network outputs were saved to \"" 
		  << par.output << "\".");
    //print summary:
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



