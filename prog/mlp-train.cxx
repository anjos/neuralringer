//Dear emacs, this is -*- c++ -*-

/**
 * @file mlp-train.cxx
 *
 * Builds an unbiased MLP from scratch, based on a database (number of
 * features and classes to separate) and train the neural network to recognize
 * the patterns in that database.
 */

#include "data/RoIPatternSet.h"
#include "data/Database.h"
#include "data/NormalizationOperator.h"
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
  std::string db; ///< database to use for training and testing
  std::string startnet; ///< name of the starting neural net file
  std::string endnet; ///< name of the ending neural net file
  std::string mseevo; ///< where to save the neural network MSE evolution
  std::string spevo; ///< where to save the neural network SP evolution
  std::string output; ///< where to save the last output after training
  unsigned int nhidden; ///< number of hidden neurons
  unsigned int epoch; ///< each epoch size
  data::Feature lrate; ///< learning rate to use
  data::Feature lrdecay; ///< learning rate decay
  data::Feature momentum; ///< momentum
  data::Feature trainperc; ///< default amount of data to use for tranining
  bool spstop; ///< use SP product stop criteria instead of MSE stabilisation
  bool compress; ///< if I should use compressed or extended output 
  unsigned int stopiter; ///< number of iterations w/o variance to stop
  data::Feature stopthres; ///< the threshold to consider for stopping
  unsigned int sample; ///< the sample interval for MSE or SP
  unsigned int hardstop; ///< where to hard stop the training
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
  char* startnet=0;
  char* endnet=0;
  char* mseevo=0;
  char* spevo=0;
  char* output=0;
  unsigned int nhidden=0;
  unsigned int epoch=1;
  data::Feature lrate=0.1;
  data::Feature lrdecay=1.0;
  data::Feature momentum=0.0;
  data::Feature trainperc=0.5;
  bool spstop = true;
  bool compress = false;
  unsigned int stopiter=100;
  data::Feature stopthres=0.001; //0.1%
  unsigned int sample=5;
  unsigned int hardstop=0;

  //return `arg' is set to !=0, so the system processes everything in the
  //while loop bellow.
  struct poptOption optionsTable[] = {
    { "train-percentage", 'a', POPT_ARG_DOUBLE, &trainperc, 'a',
      "how much of the database to use for traning the network",
      "double, -1.0 < x < 1.0: default is 0.5"}, 
    { "hard-stop", 'b', POPT_ARG_INT, &hardstop, 'b',
      "number of epochs after which to hard stop the training session",
      "uint > 0: no defaults" },
    { "epoch", 'c', POPT_ARG_INT, &epoch, 'c',
      "how many entries per training step should I use",
      "uint > 0: default is 1 (online)" },
    { "db", 'd', POPT_ARG_STRING, &db, 'd',
      "location of the database to use for training and testing", "path" },
    { "end-net", 'e', POPT_ARG_STRING, &endnet, 'e',
      "where to write the last network", "path: default is db-name.end.xml" },
    { "momentum", 'g', POPT_ARG_DOUBLE, &momentum, 'g',
      "the momentum I should use for training",  
      "double, 0 < x <= 1: default is 0.0 (no momentum)" }, 
    { "stop-iteration", 'i', POPT_ARG_INT, &stopiter, 'i',
      "how many times to wait for non-variation to be considered a stop sign",
      "unsigned int: default is 100" },
    { "learn-rate", 'l', POPT_ARG_DOUBLE, &lrate, 'l',
      "the learning rate I should use for training",
      "double, 0 < x <= 2: default is 0.1" },
    { "mse-evolution", 'm', POPT_ARG_STRING, &mseevo, 'm',
      "where to write the MSE evolution data, during training",
      "path: default is db-name.mse.xml" },
    { "sample-interval", 'n', POPT_ARG_INT, &sample, 'n',
      "when to sample the training process for the MSE evolution or SP",
      "int: default is 5" },
    { "output", 'o', POPT_ARG_STRING, &output, 'o',
      "where to write the output of the last network", 
      "path: default is db-name.out.xml" },
    { "sp-evolution", 'p', POPT_ARG_STRING, &spevo, 'p',
      "where to write the SP evolution data, during training", 
      "path: default is db-name.sp.xml" },
    { "hidden", 'r', POPT_ARG_INT, &nhidden, 'r',
      "how many hidden neurons should I use for the network",
      "uint > 0: no default" },
    { "start-net", 's', POPT_ARG_STRING, &startnet, 's',
      "where to write the start status of the network",
      "path: default is db-name.start.xml" },
    { "mse-stop", 't', POPT_ARG_NONE, 0, 't',
      "If I should use MSE stop criteria instead of SP product",
      "default is false" },
    { "stop-threshold", 'w', POPT_ARG_DOUBLE, &stopthres, 'w',
      "the stop threshold to consider for flagging a potential stop",
      "double, x > 0: default is 0.001" },
    { "learn-rate-decay", 'y', POPT_ARG_DOUBLE, &lrdecay, 'y',
      "the learning rate decay I should use for training",
      "double, 0 < x <= 1: default is 1.0 (no decay)" },
    { "compress-output", 'z', POPT_ARG_NONE, 0, 'z',
      "should compress the output, e.g. 2 classes -> 1 output for the network",
      "default is false" },
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
	throw RINGER_EXCEPTION("This value should be between (-1,1)");
      }
      RINGER_DEBUG1("Training percentage set to " << trainperc);
      break;
    case 'b': //hardstop
      RINGER_DEBUG1("I'll hardstop the training after " << hardstop
		    << " epochs, if nothing happens before");
      break;
    case 'c': //epoch
      RINGER_DEBUG1("Training epoch is " << epoch);
      break;
    case 'd': //db
      RINGER_DEBUG1("Database name is " << db);
      if (!sys::exists(db)) {
	RINGER_DEBUG1("Database file " << db << " doesn't exist.");
	throw RINGER_EXCEPTION("Database file doesn't exist");
      }
      break;
    case 'e': //end-net name
      RINGER_DEBUG1("End Network name is " << endnet);
      break;
    case 'g': //momentum
      if (momentum < 0 || momentum >= 1.0) {
	RINGER_DEBUG1("Trying to set the momentum to " << momentum);
	throw RINGER_EXCEPTION("The momentum should be between [0,1)");
      }
      RINGER_DEBUG1("Momentum set to " << momentum);
      break;
    case 'i': //number of iterations before stop training
      RINGER_DEBUG1("Stop iteration is to " << stopiter);
      break;
    case 'l': //learn-rate
      if (lrate <= 0 || lrate > 2) {
	RINGER_DEBUG1("Trying to set the learning rate to " << lrate);
	throw RINGER_EXCEPTION("Learning rate should be between (0,2]");
      }
      RINGER_DEBUG1("Learning rate set to " << lrate);
      break;
    case 'm': //MSE evolution file
      RINGER_DEBUG1("MSE evolution file set to " << mseevo);
      break;
    case 'n': //sample interval
      if (sample <= 0) {
	RINGER_DEBUG1("Trying to set the sampling interval to " << sample);
	throw RINGER_EXCEPTION("The sampling interval should be > 0");
      }
      RINGER_DEBUG1("Sampling interval set to " << sample);
      break;
    case 'o': //output file name
      RINGER_DEBUG1("Output file set to " << output);
      break;
    case 'p': //SP evolution file
      RINGER_DEBUG1("SP evolution file set to " << spevo);
      break;
    case 'r': //number of hidden neurons
      RINGER_DEBUG1("Number of hidden neurons set to " << nhidden);
      break;
    case 's': //start-net name
      RINGER_DEBUG1("Start network file set to " << startnet);
      break;
    case 't': //use MSE instead of default SP stop criteria
      RINGER_DEBUG1("I'll be using the MSE as stopping criteria");
      spstop = false;
      break;
    case 'y': //learn-rate decay
      if (lrdecay <= 0 || lrdecay > 1) {
	RINGER_DEBUG1("Trying to set the learning rate decay to " << lrdecay);
	throw RINGER_EXCEPTION("Learning rate decay should be between (0,1]");
      }
      RINGER_DEBUG1("Learning rate decay set to " << lrdecay);
      break;
    case 'w': //stop threshold
      if (stopthres <= 0) {
	RINGER_DEBUG1("I cannot use a stop threshold less than zero");
	throw RINGER_EXCEPTION("Stop threshold is less than zero");
      }
      RINGER_DEBUG1("Setting stop threshold to " << stopthres);
    case 'z': //compress the network output
      RINGER_DEBUG1("I'll be compressing the network output");
      compress = true;
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
  if (!startnet) {
    p.startnet = stripname(p.db) + ".start.xml";
    RINGER_DEBUG1("Setting start file name to " << p.startnet);
  } else p.startnet = startnet;
  if (!endnet) {
    p.endnet = stripname(p.db) + ".end.xml";
    RINGER_DEBUG1("Setting end file name to " << p.endnet);
  } else p.endnet = endnet;
  if (!mseevo) {
    p.mseevo = stripname(p.db) + ".mse.txt";
    RINGER_DEBUG1("Setting MSE evolution file name to " << p.mseevo);
  } else p.mseevo = mseevo;
  if (!spevo) {
    p.spevo = stripname(p.db) + ".sp.txt";
    RINGER_DEBUG1("Setting SP evolution file name to " << p.spevo);
  } else p.spevo = spevo;
  if (!output) {
    p.output = stripname(p.db) + ".out.xml";
    RINGER_DEBUG1("Setting output file name to " << p.output);
  } else p.output = output;
  if (!nhidden) {
    RINGER_DEBUG1("I cannot work with 0 hidden neurons. Exception thrown.");
    throw RINGER_EXCEPTION("No number of hidden neurons specified");
  } else p.nhidden = nhidden;
  if (stopiter == 0) {
    RINGER_DEBUG1("I cannot wait 0 cycles to stop, please use a number"
		  << " between 1 and infinity.");
    throw RINGER_EXCEPTION("Cycles to stop cannot be zero");
  }
  if (!hardstop) {
    RINGER_DEBUG1("I cannot work with an unbound network training."
		  << " Please provide me a hardstop.");
    throw RINGER_EXCEPTION("No hardstop parameter specified.");
  } else p.hardstop = hardstop;
  p.epoch = epoch;
  p.lrate = lrate;
  p.lrdecay = lrdecay;
  p.momentum = momentum;
  p.trainperc = trainperc;
  p.spstop = spstop;
  p.compress = compress;
  p.stopiter = stopiter;
  p.stopthres = stopthres;
  p.sample = sample;
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
  //splits the database in train and test
  data::Database<data::RoIPatternSet>* traindb;
  data::Database<data::RoIPatternSet>* testdb;
  std::vector<std::string> cnames;
  db.class_names(cnames);
  db.split(par.trainperc, traindb, testdb);
  //calculate the normalization factor based on the train set only
  data::NormalizationOperator norm_op(*traindb);
  RINGER_DEBUG1("Train set size is " << traindb->size());
  RINGER_DEBUG1("Test set size is " << testdb->size());
  
  //checks db size
  if (db.size() < 2) {
    RINGER_FATAL(reporter, "The database you loaded contains only 1 class of"
		 " events. Please, reconsider your input file.");
  }

  //checks one more thing
  if (db.size() > 2 && par.spstop) {
    RINGER_FATAL(reporter, "I cannot use the SP product with in a multi"
		 "-class scenario. Please, either re-program me or reconsider"
		 " your options.");
  }

  //Builds the MLP network, 1 input for each feature, a compressed output
  //schema for each class (2 classes -> 1 output)
  std::vector<size_t> hlayer(1, par.nhidden);
  config::NeuronStrategyType nstrat = config::NEURON_BACKPROP;
  config::NeuronBackProp::ActivationFunction actfun = 
    config::NeuronBackProp::TANH;
  config::Parameter* nsparam = new config::NeuronBackProp(actfun);
  config::SynapseStrategyType sstrat = config::SYNAPSE_BACKPROP;
  config::Parameter* ssparam =
    new config::SynapseBackProp(par.lrate, par.momentum, par.lrdecay);
  std::vector<bool> biaslayer(2, true);
  unsigned int nout = db.size();
  if (par.compress) nout = lrint(std::ceil(log2(db.size())));
  network::MLP net(db.pattern_size(), hlayer, nout,
		   biaslayer, nstrat, nsparam, nstrat, nsparam,
		   sstrat, ssparam, norm_op.mean(), norm_op.stddev(), 
		   reporter);

  //tune input DB's for size/randomness
  data::Database<data::RoIPatternSet> 
    traindb2(*traindb); //save original train database
  traindb->normalise();
  data::RoIPatternSet train(1, 1);
  traindb->merge(train);
  RINGER_REPORT(reporter, "Normalised train set size is " << train.size());
  data::RoIPatternSet train2(1, 1);
  traindb2.merge(train2);
  RINGER_REPORT(reporter, "Original train set size is " << train2.size());
  RINGER_DEBUG1("Train set size is " << train.size());
  data::RoIPatternSet target(1, 1);
  traindb->merge_target(par.compress, -1, +1, target);
  RINGER_REPORT(reporter, "Normalized train target set size is " 
		<< target.size());
  data::RoIPatternSet target2(1, 1);
  traindb2.merge_target(par.compress, -1, +1, target2);
  RINGER_REPORT(reporter, "Original train target set size is "
		<< target2.size());
  RINGER_DEBUG1("Train target set size is " << target.size());
  data::RoIPatternSet test(1, 1);
  testdb->merge(test);
  RINGER_DEBUG1("Test set size is " << test.size());
  data::RoIPatternSet test_target(1, 1);
  testdb->merge_target(par.compress, -1, +1, test_target);
  RINGER_DEBUG1("Test target set size is " << test_target.size());
  
  try {
    sys::File mseevo(par.mseevo, std::ios_base::trunc|std::ios_base::out);
    mseevo << "epoch test-mse train-mse" << "\n";
    sys::File spevo(par.spevo, std::ios_base::trunc|std::ios_base::out);
    spevo << "epoch test-sp train-sp" << "\n";
    data::SimplePatternSet output(target2.simple());
    config::Header net_header("Andre DOS ANJOS", par.output, "1.0", time(0),
			      "Start set");
    net.save(par.startnet, &net_header);
    unsigned int stopnow = par.stopiter;

    //Trains until the MSE or SP product stabilizes
    double val = 1;
    double var = 1;

    double prev = 0; //previous
    size_t i = 0;
    double best_val = val;
    while (stopnow) {
      net.train(train.simple(), target.simple(), par.epoch);
      --par.hardstop;
      if (!par.hardstop) {
	RINGER_REPORT(reporter, "Hard-stop limit has been reached. Stopping"
		      << " the training session by force.");
	stopnow = 0;
	continue;
      }

      if (i%par.sample == 0) { //time to check performance
	double eff1, eff2, thres;
	{
	  //test set analysis
	  output = test_target.simple();
	  net.run(test.simple(), output);
	  double sp_val;
	  if (db.size() == 2)
	    sp_val = data::sp(output, test_target, eff1, eff2, thres);
	  double mse_val = data::mse(output, test_target.simple());
	  mseevo << i << " " << mse_val;
	  spevo << i << " " << sp_val;
	  prev = val;
	  if (par.spstop) val = sp_val;
	  else val = mse_val;
	  var = std::fabs(val-prev)/prev;
	  bool savenet = false;
	  if (par.spstop) { //the greater, the better
	    if (val > best_val) {
	      best_val = val;
	      savenet = true;
	    }
	  }
	  else {
	    if (var < best_val) {
	      best_val = val;
	      savenet = true;
	    }
	  }
	  if (savenet) { //save the current network because it is the best
	    //save result
	    RINGER_REPORT(reporter, "Saving best network so far at \""
			  << par.endnet << "\"...");
	    config::Header end_header("Andre DOS ANJOS", par.output, 
				      "1.0", time(0), "Trained network");
	    net.save(par.endnet, &end_header);
	  }
	}
	{
	  //train set analysis
	  net.run(train2.simple(), output);
	  double sp_val;
	  if (db.size() == 2) 
	    sp_val = data::sp(output, target2, eff1, eff2, thres);
	  double mse_val = data::mse(output, target2.simple());
	  mseevo << " " << mse_val << "\n";
	  spevo << " " << sp_val << "\n";
	}
	if (var < par.stopthres) {
	  --stopnow;
	  RINGER_REPORT(reporter, "Detected possible stop in " << stopnow
			<< " more iterations.");
	}
	else stopnow = par.stopiter;
	if (par.spstop) {
	  RINGER_REPORT(reporter, "[epoch " << i << "] SP = " << val
			<< " (variation = " << var << ") (for threshold=" 
			<< thres << " -> " << cnames[0]
			<< " eff=" << eff1*100 << "% and " << cnames[1]
			<< " eff=" << eff2*100 << "%)");
	}
	else RINGER_REPORT(reporter, "[epoch " << i << "] MSE = " << val
			   << " (variation = " << var << ")");
      }

      ++i; //go to next epoch
    }

    RINGER_REPORT(reporter,
		  "Saving training and testing outputs and targets.");
    
    //reload the best network saved so far.
    network::Network bestnet(par.endnet, reporter);
    data::SimplePatternSet simple_train_output(target2.simple());
    bestnet.run(train2.simple(), simple_train_output);
    data::RoIPatternSet train_output(simple_train_output, train2.attributes());
    double mse_train = data::mse(train_output, target2);
    double sp_train = 0;
    double train_eff1 = 0;
    double train_eff2 = 0;
    double train_thres = 0;
    if (db.size() == 2) 
      sp_train = data::sp(train_output, target2,train_eff1, train_eff2, 
			  train_thres);
    data::SimplePatternSet simple_test_output(target.simple());
    bestnet.run(test.simple(), simple_test_output);
    data::RoIPatternSet test_output(simple_test_output, test.attributes());
    double mse_test = data::mse(test_output, test_target);
    double sp_test = 0;
    double test_eff1 = 0;
    double test_eff2 = 0;
    double test_thres = 0;
    if (db.size() == 2) sp_test = data::sp(test_output, test_target,
					   test_eff1, test_eff2, test_thres);
    std::map<std::string, data::RoIPatternSet*> data;
    data["train-output"] = &train_output;
    data["train-target"] = &target2;
    data["test-output"] = &test_output;
    data["test-target"] = &test_target;
    std::ostringstream comment;
    comment << "Outputs from training session based on " << par.db << ". "
	    << "Train set MSE = " << mse_train;
    if (db.size() == 2) comment << " and SP = " << sp_train;
    comment << ". Test set MSE = " << mse_test;
    if (db.size() == 2) comment << " and SP = " << sp_test;
    comment << ".";
    data::Header header("Andre DOS ANJOS", par.output, "1.0", time(0),
			comment.str());
    data::Database<data::RoIPatternSet> output_db(&header, data, reporter);
    output_db.save(par.output);
    RINGER_REPORT(reporter, "Network output saved to \"" << par.output
		  << "\".");

    //print summary:
    if (db.size() == 2) {
      RINGER_REPORT(reporter, "Summary for " << cnames[0] << "/" << cnames[1]
		    << " discrimination with an MLP network:");
    }
    RINGER_REPORT(reporter, comment.str());
    if (db.size() == 2) { //print SP statistics
      RINGER_REPORT(reporter, "Train set gives, for threshold="
		    << train_thres << " -> " << cnames[0]
		    << " eff=" << train_eff1*100 << "% and " << cnames[1]
		    << " eff=" << train_eff2*100 << "%.");
      RINGER_REPORT(reporter, "Test set gives, for threshold="
		    << test_thres << " -> " << cnames[0]
		    << " eff=" << test_eff1*100 << "% and " << cnames[1]
		    << " eff=" << test_eff2*100 << "%.");
    }

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



