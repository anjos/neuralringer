/**
 * @file operator.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds A few of the operator classes into python
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "data/NormalizationOperator.h"

using namespace boost::python;

void bind_data_operator()
{
	class_<data::PatternOperator, boost::shared_ptr<data::PatternOperator>, boost::noncopyable>("PatternOperator", "This will transform the input Pattern into a new output Pattern. The result of this transformation is <b>not</b> necesarily bi-directional.", no_init)
    .def("__call__", &data::PatternOperator::operator())
    ;

	class_<data::NormalizationOperator, boost::shared_ptr<data::NormalizationOperator>, bases<data::PatternOperator>, boost::noncopyable>("NormalizationOperator", "Generates a Pattern with the same length as the previous, but with the mean removed and divided by the variance from each Pattern feature.", init<const data::Database<data::SimplePatternSet>&>((arg("pattern_set"))))
    //implement mean and var?
		;
}
