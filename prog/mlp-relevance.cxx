//Dear emacs, this is -*- c++ -*-

/**
 * @file mlp-relevance.cxx
 *
 * Loads an MLP built with another instance of neuralringer and calculates the
 * relevance for each variable, dividing the data in train and test.
 */

#include "data/SimplePatternSet.h"
#include "data/Database.h"
#include "data/RemoveDBMeanOperator.h"
#include "data/util.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"
#include "sys/debug.h"
#include "sys/util.h"
#include "network/Network.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <popt.h>

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
  std::string db; ///< database to use for training and testing
  std::string out; ///< where to save the test set relevance
  data::Feature trainperc; ///< default amount of data to use for tranining
  std::string net; ///< the network file
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
  char* out=0;
  char* net=0;
  data::Feature trainperc=0.5;

  //return `arg' is set to !=0, so the system processes everything in the
  //while loop bellow.
  struct poptOption optionsTable[] = {
    { "train-percentage", 'a', POPT_ARG_DOUBLE, &trainperc, 'a',
      "how much of the database to use for traning the network",
      "double, -1.0 < x < 1.0: default is 0.5"}, 
    { "db", 'd', POPT_ARG_STRING, &db, 'd',
      "location of the database to use for training and testing", "path" },
    { "network", 'n', POPT_ARG_STRING, &net, 'n',
      "The network description to use", "path" },
    { "out", 'o', POPT_ARG_STRING, &out, 'o',
      "Where to save the relevance output", 
      "path: default is net-name.relevance.txt" },
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
    case 'a': //train percentage
      if (trainperc <= -1.0 || trainperc >= 1.0) {
	RINGER_DEBUG1("Trying to set the training percentage to "<< trainperc);
	throw RINGER_EXCEPTION("This value should be between (0,1)");
      }
    case 'd': //db
      RINGER_DEBUG1("Database name is " << db);
      if (!sys::exists(db)) {
	RINGER_DEBUG1("Database file " << db << " doesn't exist.");
	throw RINGER_EXCEPTION("Database file doesn't exist");
      }
      break;
    case 'n': //network
      RINGER_DEBUG1("Network name is " << net);
      if (!sys::exists(net)) {
	RINGER_DEBUG1("Network file " << net << " doesn't exist.");
	throw RINGER_EXCEPTION("Network file doesn't exist");
      }
      break;
    case 'o': //where to save the relevance output
      RINGER_DEBUG1("Saving relevance calculations to \"" 
		    << out << "\".");
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
  if (!net) {
    RINGER_DEBUG1("I cannot work without a network file to start from.");
    throw RINGER_EXCEPTION("No network file specified");
  } else p.net = net;
  if (!out) {
    p.out = stripname(p.net) + ".relevance.txt";
    RINGER_DEBUG1("Setting output name to " << p.out);
  } else p.out = out;
  p.trainperc = trainperc;
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
  data::Database<data::SimplePatternSet> db(par.db, reporter);

  //loads the network
  network::Network net(par.net, reporter);
  bool compressed_output = false;
  if (net.output_size() < db.size()) compressed_output = true;
  
  //split the data base in train and test
  data::Database<data::SimplePatternSet>* traindb;
  data::Database<data::SimplePatternSet>* testdb;
  std::vector<std::string> cnames;
  db.class_names(cnames);
  data::RemoveDBMeanOperator rmmean(db);
  db.apply_pattern_op(rmmean);
  db.split(par.trainperc, traindb, testdb);
  RINGER_DEBUG1("Train DB size is " << traindb->size());
  RINGER_DEBUG1("Test DB size is " << testdb->size());
  data::SimplePatternSet train(1, 1);
  traindb->merge(train);
  RINGER_DEBUG1("Train set size is " << train.size());
  data::SimplePatternSet train_target(1, 1);
  traindb->merge_target(compressed_output, -1, +1, train_target);
  data::SimplePatternSet test(1, 1);
  testdb->merge(test);
  RINGER_DEBUG1("Test set size is " << test.size());
  data::SimplePatternSet test_target(1, 1);
  testdb->merge_target(compressed_output, -1, +1, test_target);

  try {
    sys::File out(par.out, std::ios_base::trunc|std::ios_base::out);
    data::SimplePatternSet test_output(test_target);
    data::SimplePatternSet train_output(train_target);
    out << "input test.relevance train.relevance\n";
    //test set analysis
    net.run(test, test_output);
    net.run(train, train_output);
    data::SimplePatternSet test_copy(test);
    data::SimplePatternSet train_copy(train);
    data::SimplePatternSet test_copy_output(test_target);
    data::SimplePatternSet train_copy_output(train_target);
    data::Ensemble test_mean(test.size());
    data::Ensemble train_mean(train.size());
    for (size_t i = 0; i < test.pattern_size(); ++i) {
      RINGER_REPORT(reporter, "Evaluating relevance for feature `" 
		    << i << "'.");
      //for each input variable
      //reset inputs
      test_copy = test; 
      train_copy = train;
      //set desired ensemble to zero: remember, I removed the mean!
      test_copy.set_ensemble(i, test_mean);
      train_copy.set_ensemble(i, train_mean);
      //run changed copies through the network
      net.run(test_copy, test_copy_output);
      net.run(train_copy, train_copy_output);
      out << i << " " << data::mse(test_output, test_copy_output) << " "
	  << data::mse(train_output, train_copy_output) << "\n";
    }
    RINGER_REPORT(reporter, "Cleaning up and exiting...");

    //clear resources
    delete traindb;
    delete testdb;
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



