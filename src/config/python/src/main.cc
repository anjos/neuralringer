/**
 * @file main.cc 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Combines all modules to make up the complete bindings
 */

#include <boost/python.hpp>

using namespace boost::python;

void bind_config();

BOOST_PYTHON_MODULE(libpynlab_config) {
  scope().attr("__doc__") = "Neural Lab config classes and sub-classes";
  bind_config();
}
