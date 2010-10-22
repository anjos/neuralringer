/**
 * @file database.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds the Database-related classes into python
 */

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "data/Pattern.h"
#include "data/PatternSet.h"
#include "data/Database.h"
#include "data/Header.h"
#include "data/util.h"

using namespace boost::python;

boost::shared_ptr<data::Pattern> make_pattern(object feat) {
  //the input object "feat" has to be iterable!
  boost::shared_ptr<data::Pattern> retval(new data::Pattern(len(feat), 0.0));
  boost::python::stl_input_iterator<data::Feature> i(feat);
  for(boost::python::ssize_t p=0; p<len(feat); ++p) (*retval)[p] = *(i++);
  return retval;
}

boost::shared_ptr<data::PatternSet> create_patternset(object data) {
  std::vector<boost::shared_ptr<data::Pattern> > v; 
  std::vector<data::Pattern*> vp; 
  boost::python::stl_input_iterator<object> it(data);
  for (boost::python::ssize_t p=0; p<len(data); ++p) {
    extract<data::Pattern> pat_extractor(*it);
    if (pat_extractor.check()) {
      data::Pattern* pointer = const_cast<data::Pattern*>(&pat_extractor());
      vp.push_back(pointer);
    }
    else { //second case: iterable with doubles
      boost::shared_ptr<data::Pattern> sp(make_pattern(*it));
      v.push_back(sp);
      vp.push_back(sp.get());
    }
  }
  return boost::shared_ptr<data::PatternSet>(new data::PatternSet(vp));
}

boost::shared_ptr<data::Database> create_database(data::Header& h, dict data, 
    sys::Reporter& r) {
  std::map<std::string, data::PatternSet*> d;
  for (object i = data.iterkeys(); i; i = i.attr("next")()) {
    const char* name = extract<const char*>(i[0]);
    data::PatternSet& ps = extract<data::PatternSet&>(i[1]);
    d[name] = &ps;
  }
  return boost::shared_ptr<data::Database>(new data::Database(&h, d, r));
}

dict get_data(data::Database& db) {
  dict retval;
  std::vector<std::string> class_names;
  db.class_names(class_names);
  for (size_t i=0; i<class_names.size(); ++i) {
    retval[class_names[i]] = db.data(class_names[i]);
  }
  return retval;
}

list get_class_names(const data::Database& db) {
  list retval;
  std::vector<std::string> class_names;
  db.class_names(class_names);
  for (size_t i=0; i<class_names.size(); ++i) retval.append(class_names[i]);
  return retval;
}

void db_set_targets(data::Database& db, dict targets) {
  std::vector<boost::shared_ptr<data::Pattern> > pats;
  std::map<std::string, data::Pattern*> tmap;
  for (object i = targets.iterkeys(); i; i = i.attr("next")()) {
    const char* name = extract<const char*>(i[0]);
    pats.push_back(make_pattern(i[1]));
    tmap[name] = pats[pats.size()-1].get();
  }
  db.set_target(tmap);
}

boost::shared_ptr<data::PatternSet> db_merge(const data::Database& db) {
  boost::shared_ptr<data::PatternSet> retval(new data::PatternSet(1, 1));
  db.merge(*retval.get());
  return retval;
}

boost::shared_ptr<data::PatternSet> db_merge_target (const data::Database& db) {
  boost::shared_ptr<data::PatternSet> retval(new data::PatternSet(1, 1));
  db.merge_target(*retval.get());
  return retval;
}

data::Feature pattern_get(const data::Pattern& p, size_t i) {
  return p[i];
}

void bind_data_database()
{
	class_<data::Header, boost::shared_ptr<data::Header> >("Header", "Defines and reades information from XML headers", init<const std::string&, const std::string&, const std::string&, const time_t&, const std::string&>((arg("author"), arg("name"), arg("version"), arg("created"), arg("comment"))))
    .add_property("author", &data::Header::author) 
    .add_property("name", &data::Header::name) 
    .add_property("version", &data::Header::version) 
    .add_property("created", &data::Header::created) 
    .add_property("lastSaved", &data::Header::lastSaved) 
    .add_property("comment", &data::Header::comment) 
		;

  class_<data::Pattern, boost::shared_ptr<data::Pattern> >("Pattern", "The Pattern is defined in terms of Feature's.\n\n In Neural Network's jargon, the <b>pattern</b> is an entity that contains a set of features that represent an event. Being the <b>pattern</b> just an ensemble, its C++ representation cannot be far from a simple vector. The required functionality is pretty much the same, although its implementation relies on GSL matrix views of a data set.\n\nIf a Pattern is created from a set of Feature's, a new block of data is allocated.", init<const size_t&, const data::Feature>())
    .def("__init__", make_constructor(make_pattern, default_call_policies(), (arg("features"))))
    .def("size", &data::Pattern::size, (arg("self")), "The pattern size")
    .def("__len__", &data::Pattern::size, (arg("self")), "The pattern size")
    .def("__add__", data::operator+)
    .def("__sub__", data::operator-)
    .def("__mul__", data::operator*)
    .def("__div__", data::operator/)
    .def("__iadd__", (data::Pattern& (data::Pattern::*)(const data::Feature&))&data::Pattern::operator+=, return_self<>())
    .def("__isub__", (data::Pattern& (data::Pattern::*)(const data::Feature&))&data::Pattern::operator-=, return_self<>())
    .def("__imul__", (data::Pattern& (data::Pattern::*)(const data::Feature&))&data::Pattern::operator*=, return_self<>())
    .def("__idiv__", (data::Pattern& (data::Pattern::*)(const data::Feature&))&data::Pattern::operator/=, return_self<>())
    .def("__iadd__", (data::Pattern& (data::Pattern::*)(const data::Pattern&))&data::Pattern::operator+=, return_self<>())
    .def("__isub__", (data::Pattern& (data::Pattern::*)(const data::Pattern&))&data::Pattern::operator-=, return_self<>())
    .def("__imul__", (data::Pattern& (data::Pattern::*)(const data::Pattern&))&data::Pattern::operator*=, return_self<>())
    .def("__idiv__", (data::Pattern& (data::Pattern::*)(const data::Pattern&))&data::Pattern::operator/=, return_self<>())
    .def("__getitem__", &pattern_get)
    .def("append", &data::Pattern::append, (arg("self"), arg("pattern")))
    ;

  class_<data::PatternSet, boost::shared_ptr<data::PatternSet> >("PatternSet", "This class represents a set of data::Pattern's.\n\nA <b>data::Pattern set</b> is an entity that holds, virtually, any number of data::Pattern's. The set can be queried for special methods, allows random access with a reasonable speed and data::Pattern's to be inserted and removed from it. Some sort of normalisation strategies are also possible. A PatternSet can also be used to produce smaller <b>sets</b> that match a certain criteria. This class and related methods are strongly based on the GNU Scientific Library (GSL) vector/matrix/block objects. Please, refer to the GSL manual to understand better its limitations and virtudes (try at your shell <code>info gsl</code> or <code>info gsl-ref</code>).\n\n While data::Pattern's represent each event available in a PatternSet, a data::Ensemble represents a given Feature for every event in a PatternSet. Through this abstraction, it is possible to also manipulate a PatternSet with respect to its 'columns', erasing or setting them.\n\nAll types developed to be used in conjunction with this class may be derived types through the use of GSL's views of blocks, matrices and vectors.", init<const size_t&, const size_t&, optional<const data::Feature&> >((arg("set_size"), arg("pattern_size"), arg("init_value")), "Creates a brand new PatternSet"))
    .def("__init__", make_constructor(create_patternset, default_call_policies(), (arg("data"))))
    .def("__len__", &data::PatternSet::size, (arg("self")), "Returns the size of the set")
    .def("size", &data::PatternSet::size, (arg("self")), "Returns the size of the set")
    .def("pattern_size", &data::PatternSet::size, (arg("self")), "Returns the size of each Pattern in the set")
    .def("pattern", &data::PatternSet::pattern, (arg("self"), arg("pos")), "Returns a specific pattern from the set")
    .def("__getitem__", &data::PatternSet::pattern, (arg("self"), arg("pos")), "Returns a specific pattern from the set")
    .def("ensemble", &data::PatternSet::ensemble, (arg("self"), arg("pos")), "Returns a specific ensemble (column) from the set")
    .def("set_pattern", &data::PatternSet::set_pattern, (arg("self"), arg("pos"), arg("pattern")), "Sets a particular Pattern to the given value")
    .def("__setitem__", &data::PatternSet::set_pattern, (arg("self"), arg("pos"), arg("pattern")), "Sets a particular Pattern to the given value")
    .def("set_ensemble", &data::PatternSet::set_ensemble, (arg("self"), arg("pos"), arg("pattern")), "Sets a particular Ensemble to the given value")
    .def("erase_pattern", &data::PatternSet::erase_pattern, (arg("self"), arg("pos")), "Removes a particular Pattern from the set")
    .def("erase_ensemble", &data::PatternSet::erase_pattern, (arg("self"), arg("pos")), "Removes a particular Ensemble from the set")
    .def("shuffle", &data::PatternSet::shuffle, (arg("self")), "Shuffles the order of data inside this PatternSet.")
    .def("merge", &data::PatternSet::merge, (arg("self")), "This method will copy the given PatternSet Pattern's into the current set, enlarging it. We check if the Pattern sizes are the same previous to the copying. This method returns a reference to the current set being manipulated.", return_self<>()) 
    .def("__isub__", &data::PatternSet::operator-=, return_self<>())
    .def("mean_square", &data::mean_square, (arg("self")), "Calculates the Mean Square (MS) of this PatternSet, taking in consideration both of its dimensions. The formulae for this calculation is given by:\n\\frac{{\\sum_i}^M{{\\sum_j}^N{{e_{ij}}^{2}}}}{M \\times N}\n Where <b>M</b> describes the number of patterns the set contains and <b>N</b>, the number of ensembles there is.")
    .def("root_mean_square", &data::root_mean_square, (arg("self")), "This is simply the sqrt(mean_square(self)). Read the help of `mean_square` to understand what it is.")
    .def("abs_mean", &data::abs_mean, (arg("self")), "Calculates the mean of a sum of absolute values of this PatternSet, taking in consideration both of its dimensions. The formula for this calculation is given by:\nE = \\frac{{\\sum_i}^M{{\\sum_j}^N{{|e_{ij}|}}}}{M \\times N}\nWhere <b>M</b> describes the number of patterns the set contains and <b>N</b>, the number of ensembles there is.")
    .def("mse", &data::mse, (arg("self"), arg("target")), "Calculates the Mean-Square Root Error for a certain classifier output, given its target values.")
    ;

  class_<data::Database, boost::shared_ptr<data::Database> >("LegacyDatabase", "Loads a database in memory. The database file consists of a header description and a set of entries each of which, contains one or more data sets classified.", init<const std::string&, sys::Reporter&>((arg("filename"), arg("reporter"))))
    .def("__init__", make_constructor(create_database, default_call_policies(), (arg("header"), arg("data"), arg("reporter"))))
    .def("header", &data::Database::header, (arg("self")), "Returns the DB header", return_internal_reference<>())
    .def("size", &data::Database::size, (arg("self")), "The current DB size")
    .def("pattern_size", &data::Database::pattern_size, (arg("self")), "The size of each pattern inside this DB")
    .def("set_target", &db_set_targets, (arg("self"), arg("targets")), "Sets the targets for each class in the database")
    .def("data", (const data::PatternSet*(data::Database::*)(const std::string&))&data::Database::data, (arg("self"), arg("class_id")), "Returns a handle to the pattern set pointed by the class id input", return_internal_reference<>())
    .def("data", &get_data, (arg("self")), "Returns a handle to a database dictionary", with_custodian_and_ward_postcall<0, 1>())
    .def("class_names", &get_class_names, (arg("self")), "Returns a handle to a database dictionary")
    .def("save", &data::Database::save, (arg("self"), arg("filename")), "Saves into a file.")
    .def("merge", &db_merge, (arg("self")), "Merges the whole database into a single PatternSet.")
    .def("merge_target", &db_merge_target, (arg("self")), "Returns a PatternSet which express the class of each 'merged' Pattern returned by merge().")
    .def("normalise", &data::Database::normalise, (arg("self")), "Normalises the database contents with respect to its classes. This process will calculate the number of Patterns in each class and will concatenate each PatternSet (class) so each class has the same amount of Pattern's.")
    .def("shuffle", &data::Database::shuffle, (arg("self")), "This will shuffle all PatternSet's inside this database, randomly.")
		;
}
