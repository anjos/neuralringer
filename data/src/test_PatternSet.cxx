//Dear emacs, this is -*- c++ -*-

/**
 * @file data/src/test_PatternSet.cxx
 *
 * @brief Tests the PatternSet class and some of its
 * functionality. The purpose of this test is to make sure that the
 * PatternSet is responding as expected. Obvioulsy, other classes are
 * also a bit tested in the mean time.
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "data/Database.h"
#include "data/RandomInteger.h"
#include "sys/Reporter.h"
#include "sys/Exception.h"

/**
 * Returns an error message and exit.
 * 
 * @param m1 The initial part of the error message
 * @param m2 The second part of the error message
 */
void error (const char* m1, const char* m2)
{
  std::cerr << "[test_PatternSet::main()] "
	    << "ERROR: " << m1 << ", " << m2 
	    << std::endl;
  std::exit(1);
}

/**
 * Prints the contents of the input dababase on the output file
 * @param os The output stream to use
 * @param pat The PatternSet where to get the data from
 */
void print (std::ostream& os, const data::PatternSet& pat)
{
  os << "****************************" << std::endl
     << "* THE PATTERNSET CONTAINS: *" << std::endl
     << "****************************" << std::endl
     << pat;
}

/**
 * Gets a number of random entries at the pattern and return and print
 * them.
 * @param os The output stream to use
 * @param pat The PatternSet where to get the data from
 * @param nentry The number of entries to get from the set
 */
void rget (std::ostream& os, const data::PatternSet& pat, 
	   const size_t& nentry)
{ 
  os << "The " << nentry << " random entries are:" << std::endl;
  data::RandomInteger rnd;
  std::vector<size_t> v(nentry);
  rnd.draw(pat.size(), v);
  data::PatternSet otherpat(pat, v);
  std::cout << otherpat << std::endl;
}

/**
 * Sets and prints a given Pattern of the set.
 * @param os The output stream where to print the set
 * @param pset The PatternSet to change
 * @param entry The entry to change
 * @param pat The value of the new entry
 */
void setnprint (std::ostream& os, data::PatternSet& pset,
		const size_t& entry, const data::Pattern& pat)
{
  os << "**************************" << std::endl
     << "* SET AND PRINT RESULTS: *" << std::endl
     << "**************************" << std::endl;
  os << "[" << entry << "] " << "BEFORE " << pset.pattern(entry)
     << std::endl;
  pset.set_pattern(entry,pat);
  os << "[" << entry << "] " << "AFTER  " << pset.pattern(entry)
     << std::endl;
}

/**
 * Erases the given entry on the Pattern File.
 * @param os The output stream where to dump the results
 * @param pset The PatternSet from where erase the Pattern
 * @param entry The entry to erase from the set
 */
void erase (std::ostream& os, data::PatternSet& pset,
	    const size_t& entry)
{
  os << "****************************" << std::endl
     << "* ERASE AND PRINT RESULTS: *" << std::endl
     << "****************************" << std::endl;
  os << "BEFORE: " << std::endl;
  os << pset;
  pset.erase_pattern(entry);
  os << "ERASED Pattern " << entry << std::endl;
  os << pset << std::endl;
}

/**
 * Merges the second PatternSet into the first.
 * @param os The stream where to print results
 * @param f The first PatternSet (result)
 * @param s The second PatternSet 
 */
void merge (std::ostream& os, data::PatternSet& f,
	    const data::PatternSet& s)
{
  os << "****************************" << std::endl
     << "* MERGE AND PRINT RESULTS: *" << std::endl
     << "****************************" << std::endl;  
  os << " BEFORE, we had " << f.size() << " patterns." << std::endl;
  f.merge(s);
  os << " MERGED PatternSets." << std::endl;
  os << " AFTER, we have " << f.size() << " patterns." << std::endl;
  print(os, f);
}

int main (int argc, char** argv)
{
  sys::Reporter reporter("local");
  if (argc < 3 || argc > 4) {
    RINGER_REPORT(reporter, "usage: " << argv[0]
		<< " <db filename> <command>" << std::endl
		<< "\tcommand may be:" << std::endl
		<< "\t print - print the database" << std::endl
		<< "\t rget X - get X random entries and print" << std::endl
		<< "\t setp X - set pattern X to a value and print" 
		<< std::endl
		<< "\t erase X - erase entry X on the Pattern file" 
		<< std::endl
		<< "\t merge - merges the two pattern sets in DB");
    RINGER_FATAL(reporter, "Insuficiente number of parameters! Bye...");
  }
  try {
    data::Database db(argv[1], reporter);
    const data::PatternSet* mypat = db.data().begin()->second;
    std::string command(argv[2]);
    if ( command == "print" ) {
      print(std::cout, *mypat);
    }
    else if ( command == "rget" ) {
      if (argc != 4) RINGER_FATAL(reporter, "rget: missing argument!");
      std::istringstream iss(argv[3]);
      size_t n;
      iss >> n;
      rget(std::cout, *mypat, n);
    }
    else if ( command == "setp" ) {
      if (argc != 4) RINGER_FATAL(reporter, "setp: missing argument!");
      std::istringstream iss(argv[3]);
      size_t n;
      iss >> n;
      data::PatternSet copy(*mypat);
      setnprint(std::cout, copy, n,
		data::Pattern(copy.pattern_size(),3.141547));
    }
    else if ( command == "erase" ) {
      if (argc != 4) RINGER_FATAL(reporter, "erase: missing argument!");
      std::istringstream iss(argv[3]);
      size_t n;
      iss >> n;
      data::PatternSet copy(*mypat);
      erase(std::cout, copy, n);
    }
    else if ( command == "merge" ) {
      const data::PatternSet* otherpat = (++db.data().begin())->second;
      data::PatternSet copy1(*mypat);
      data::PatternSet copy2(*otherpat);
      merge(std::cout, copy1, copy2);
      std::map <std::string, data::PatternSet*> newdata;
      newdata["merged"] = &copy1;
      newdata["not-merged"] = &copy2;
      data::Database newdb(db.header(), newdata, reporter);
      newdb.save("test.xml");
    }
    else RINGER_FATAL(reporter, "Command \"" << command << "\" not identified.");
  }
  catch (sys::Exception& e) {
    std::string mesg = e.what();
    RINGER_EXCEPT(reporter, mesg);
    RINGER_FATAL(reporter, "Exception caught, exiting...");
  }
}

