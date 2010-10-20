/**
 * @file database.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds the Database-related classes into python
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "data/Pattern.h"
#include "data/SimplePatternSet.h"
#include "data/Database.h"
#include "data/Header.h"

using namespace boost::python;

typedef data::Database<data::SimplePatternSet> db_t;

boost::shared_ptr<data::SimplePatternSet> create_patternset(list data) {
  std::vector<data::Pattern*> v; 
  for (size_t i = 0; i < data.attr("__len__")(); ++i) {
    data::Pattern& p = extract<data::Pattern&>(data[i]);
    v.push_back(&p);
  }
  return boost::shared_ptr<data::SimplePatternSet>(new data::SimplePatternSet(v));
}

boost::shared_ptr<db_t> create_database(data::Header& h, dict data, 
    sys::Reporter& r) {
  std::map<std::string, data::SimplePatternSet*> d;
  for (object i = data.iterkeys(); i; i = i.attr("next")()) {
    const char* name = extract<const char*>(i);
    data::SimplePatternSet& ps = extract<data::SimplePatternSet&>(data[i]);
    d[name] = &ps;
  }
  return boost::shared_ptr<db_t>(new db_t(&h, d, r));
}

dict get_data(db_t& db) {
  dict retval;
  std::vector<std::string> class_names;
  db.class_names(class_names);
  for (size_t i=0; i<class_names.size(); ++i) {
    retval[class_names[i]] = db.data(class_names[i]);
  }
  return retval;
}

list get_class_names(const db_t& db) {
  list retval;
  std::vector<std::string> class_names;
  db.class_names(class_names);
  for (size_t i=0; i<class_names.size(); ++i) retval.append(class_names[i]);
  return retval;
}

boost::shared_ptr<data::SimplePatternSet> db_merge(const db_t& db) {
  boost::shared_ptr<data::SimplePatternSet> retval(new data::SimplePatternSet(1, 1));
  db.merge(*retval.get());
  return retval;
}

boost::shared_ptr<data::SimplePatternSet> db_merge_target
(const db_t& db, const bool minimal, const double& min, const double& max) {
  boost::shared_ptr<data::SimplePatternSet> retval(new data::SimplePatternSet(1, 1));
  db.merge_target(minimal, min, max, *retval.get());
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
    .def("size", &data::Pattern::size, (arg("self")), "The pattern size")
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

  class_<data::SimplePatternSet, boost::shared_ptr<data::SimplePatternSet> >("SimplePatternSet", "This class represents a set of data::Pattern's.\n\nA <b>data::Pattern set</b> is an entity that holds, virtually, any number of data::Pattern's. The set can be queried for special methods, allows random access with a reasonable speed and data::Pattern's to be inserted and removed from it. Some sort of normalisation strategies are also possible. A SimplePatternSet can also be used to produce smaller <b>sets</b> that match a certain criteria. This class and related methods are strongly based on the GNU Scientific Library (GSL) vector/matrix/block objects. Please, refer to the GSL manual to understand better its limitations and virtudes (try at your shell <code>info gsl</code> or <code>info gsl-ref</code>).\n\n While data::Pattern's represent each event available in a SimplePatternSet, a data::Ensemble represents a given Feature for every event in a SimplePatternSet. Through this abstraction, it is possible to also manipulate a SimplePatternSet with respect to its 'columns', erasing or setting them.\n\nAll types developed to be used in conjunction with this class may be derived types through the use of GSL's views of blocks, matrices and vectors.", init<const size_t&, const size_t&, optional<const data::Feature&> >((arg("set_size"), arg("pattern_size"), arg("init_value")), "Creates a brand new PatternSet"))
    .def("__init__", make_constructor(create_patternset, default_call_policies(), (arg("data"))))
    .def("size", &data::SimplePatternSet::size, (arg("self")), "Returns the size of the set")
    .def("pattern_size", &data::SimplePatternSet::size, (arg("self")), "Returns the size of each Pattern in the set")
    .def("pattern", &data::SimplePatternSet::pattern, (arg("self"), arg("pos")), "Returns a specific pattern from the set")
    .def("ensemble", &data::SimplePatternSet::ensemble, (arg("self"), arg("pos")), "Returns a specific ensemble (column) from the set")
    .def("set_pattern", &data::SimplePatternSet::set_pattern, (arg("self"), arg("pos"), arg("pattern")), "Sets a particular Pattern to the given value")
    .def("set_ensemble", &data::SimplePatternSet::set_ensemble, (arg("self"), arg("pos"), arg("pattern")), "Sets a particular Ensemble to the given value")
    .def("erase_pattern", &data::SimplePatternSet::erase_pattern, (arg("self"), arg("pos")), "Removes a particular Pattern from the set")
    .def("erase_ensemble", &data::SimplePatternSet::erase_pattern, (arg("self"), arg("pos")), "Removes a particular Ensemble from the set")
    .def("shuffle", &data::SimplePatternSet::shuffle, (arg("self")), "Shuffles the order of data inside this SimplePatternSet.")
    .def("merge", &data::SimplePatternSet::merge, (arg("self")), "This method will copy the given SimplePatternSet Pattern's into the current set, enlarging it. We check if the Pattern sizes are the same previous to the copying. This method returns a reference to the current set being manipulated.", return_self<>()) 
    .def("__isub__", &data::SimplePatternSet::operator-=, return_self<>())
    ;

  class_<db_t, boost::shared_ptr<db_t> >("Database", "Loads a database in memory. The database file consists of a header description and a set of entries each of which, contains one or more data sets classified.", init<const std::string&, sys::Reporter&>((arg("filename"), arg("reporter"))))
    .def("__init__", make_constructor(create_database, default_call_policies(), (arg("header"), arg("data"), arg("reporter"))))
    .def("header", &db_t::header, (arg("self")), "Returns the DB header", return_internal_reference<>())
    .def("size", &db_t::size, (arg("self")), "The current DB size")
    .def("pattern_size", &db_t::pattern_size, (arg("self")), "The size of each pattern inside this DB")
    .def("data", (const data::SimplePatternSet*(db_t::*)(const std::string&))&db_t::data, (arg("self"), arg("class_id")), "Returns a handle to the pattern set pointed by the class id input", return_internal_reference<>())
    .def("data", &get_data, (arg("self")), "Returns a handle to a database dictionary", with_custodian_and_ward_postcall<0, 1>())
    .def("class_names", &get_class_names, (arg("self")), "Returns a handle to a database dictionary")
    .def("save", &db_t::save, (arg("self"), arg("filename")), "Saves into a file.")
    .def("merge", &db_merge, (arg("self")), "Merges the whole database into a single PatternSet.")
    .def("merge_target", &db_merge_target, (arg("self"), arg("minimal"), arg("min"), arg("max")), "Returns a PatternSet which express the class of each 'merged' Pattern returned by merge(). There are two possible options 'minimal' representation, in which the number of ensembles will be minimised to the closest power of two possibility. For example, if I have two classes, only one ensemble will exist (<code>min</code> indicates the value for the first class of Pattern's while <code>max</code> indicates the value for the second class of Pattern's). The second possibility is 'normal' where each output will represent one and only one class. In this case, looking at the targets, the maximum happening in ensemble 0, indicates the Pattern's are from class 0, when the maximum happens at ensemble 2, the Pattern's related belong to class 2, and on.")
    .def("normalise", &db_t::normalise, (arg("self")), "Normalises the database contents with respect to its classes. This process will calculate the number of Patterns in each class and will concatenate each PatternSet (class) so each class has the same amount of Pattern's.")
    .def("shuffle", &db_t::shuffle, (arg("self")), "This will shuffle all PatternSet's inside this database, randomly.")
		;
}
