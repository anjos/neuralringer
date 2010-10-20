/**
 * @file reporter.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds the Reporter class into python
 */

#include <boost/python.hpp>
#include "sys/Reporter.h"

using namespace boost::python;

void bind_sys_reporter()
{
	class_<sys::Reporter, boost::shared_ptr<sys::Reporter>, boost::noncopyable>("Reporter", "The reporter should, among other things, report debugging messages correctly. This is controlled by a small set of macros that correctly address the debugging facilities of the Reporter. For using those, you have to set a compilation macro accordingly: <code>DEBUG</code>=LOW, NORMAL or HIGH. If this macro is undefined for the time of compilation, no compilation options are coded in.", init<optional<const std::string&> >((arg("type")="local")))
    .def("report", &sys::Reporter::report, (arg("self"), arg("message")), "Reports something to the user") 
    .def("warn", &sys::Reporter::warn, (arg("self"), arg("message")), "Reports a warning message to the user") 
    .def("exception", &sys::Reporter::except, (arg("self"), arg("message")), "Reports something exceptional to the user, raises an exception") 
		;
}
