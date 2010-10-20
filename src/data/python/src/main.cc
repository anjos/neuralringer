/**
 * @file main.cc 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Combines all modules to make up the complete bindings
 */

#include <boost/python.hpp>

using namespace boost::python;

void bind_data_database();
void bind_data_operator();

BOOST_PYTHON_MODULE(libpynlab_data) {
  scope().attr("__doc__") = "Neural Lab data classes and sub-classes";
  bind_data_database();
  bind_data_operator();
}
