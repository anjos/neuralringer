/**
 * @file main.cc 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Combines all modules to make up the complete bindings
 */

#include <boost/python.hpp>

using namespace boost::python;

void bind_sys_reporter();

BOOST_PYTHON_MODULE(libpynlab_sys) {
  scope().attr("__doc__") = "Neural Lab sys classes and sub-classes";
  bind_sys_reporter();
}
