//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/test_convert.cxx
 *
 * Gets a file from ftp://ftp.ics.uci.edu/pub/machine-learning-databases and
 * convert it to my internal XML format for databases. This is a very rusty
 * and simple implementation, keep it mind...
 */

#include "sys/Reporter.h"
#include "sys/debug.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

int main (int argc, char** argv) {

  sys::Reporter reporter("local");
  if (argc != 3) {
    RINGER_FATAL(reporter, "usage: " << argv[0] 
	       << " <original-file> <features>");
    std::exit(1);
  }

  std::fstream in(argv[1], std::ios_base::in);
  if (!in) {
    RINGER_FATAL(reporter, "I cannot open " << argv[1]);
    std::exit(1);
  }
  RINGER_REPORT(reporter, "Starting with args => \"" << argv[1] << " " 
	      << argv[2] << "\"");
  const size_t nfeat = std::strtoul(argv[2], 0, 0);

  size_t linecounter = 0;
  std::vector<std::vector<double> > features;
  std::vector<std::string> myclass;
  while (in) {
    RINGER_DEBUG2("Reading line " << linecounter);
    char thisline[1024];
    in.getline(thisline, 1024);
    char* start = thisline;
    bool ok = false;
    for (size_t i=0; i<1024; ++i) {
      if (std::isspace(*start)) ++start;
      else {
	if (!std::iscntrl(*start)) {
	  ok = true;
	  break;
	}
      }
    }
    if (!ok) continue;
    std::string line(start); //copy
    //get features
    size_t next = 0;
    std::vector<double> val;
    for (size_t i=0; i<nfeat; ++i) {
      size_t curr = line.find(',', next);
      RINGER_DEBUG3("Found ',' at position " << curr << ". 'next' is " << next);
      std::string v = line.substr(next, curr-next);
      val.push_back(strtod(v.c_str(), 0));
      RINGER_DEBUG3("Next substring is \"" << v << "\"");
      next = curr+1;
    }
    features.push_back(val);
    //get class
    myclass.push_back(line.substr(next,line.length()-next));
    ++linecounter;
  }

  //calculates the number of classes
  std::vector<std::string> classes;
  classes.push_back(myclass[0]);
  for (size_t i=1; i<myclass.size(); ++i) {
    std::vector<std::string>::iterator 
      it = std::find(classes.begin(), classes.end(), myclass[i]);
    if (it == classes.end()) classes.push_back(myclass[i]);
  }
  RINGER_REPORT(reporter, "Found " << classes.size() << " classes at file \""
	      << argv[1] << "\". They are:");
  for (size_t i=0; i<classes.size(); ++i) 
    RINGER_REPORT(reporter, "[" << i << "] " << classes[i]);
  std::cout << "------------------ CUT HERE ------------------" << std::endl;

  //prints fixed output
  std::cout << " <data>" << std::endl;
  std::cout << "  <classified>" << std::endl;

  //for every class, prints the entries separated
  for (size_t c=0; c<classes.size(); ++c) {
    std::cout << "   <class name=\"" << classes[c] << "\">" << std::endl;

    for (size_t i=0; i<features.size(); ++i) {
      if (classes[c] != myclass[i]) continue; //not from this class
      std::cout << "    <entry id=\"" << i << "\"><feature>";
      for (size_t j=0; j<features[i].size()-1; ++j)
	std::cout << features[i][j] << " ";
      std::cout << features[i][features[i].size()-1] << "</feature></entry>" 
		<< std::endl;
    }
    std::cout << "   </class>" << std::endl;
  }
  std::cout << "  </classified>" << std::endl;
  std::cout << " </data>" << std::endl;
  std::cout << "------------------ CUT HERE ------------------" << std::endl;
}
