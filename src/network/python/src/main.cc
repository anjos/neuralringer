/**
 * @file main.cc 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Combines all modules to make up the complete bindings
 */

#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(libpynlab_network) {
  scope().attr("__doc__") = "Neural Lab network classes and sub-classes";
}
