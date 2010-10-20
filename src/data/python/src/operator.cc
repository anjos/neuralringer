/**
 * @file operator.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds A few of the operator classes into python
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "data/NormalizationOperator.h"
#include "data/NormaliseOperator.h"
#include "data/RemoveDBMeanOperator.h"
#include "data/RemoveMeanOperator.h"
#include "data/EnergyNormaliseOperator.h"

using namespace boost::python;

void bind_data_operator()
{
	class_<data::PatternOperator, boost::shared_ptr<data::PatternOperator>, boost::noncopyable>("PatternOperator", "This will transform the input Pattern into a new output Pattern. The result of this transformation is <b>not</b> necesarily bi-directional.", no_init)
    .def("__call__", &data::PatternOperator::operator())
    ;

	class_<data::NormalizationOperator, boost::shared_ptr<data::NormalizationOperator>, bases<data::PatternOperator>, boost::noncopyable>("NormalizationOperator", "Generates a Pattern with the same length as the previous, but with the mean removed and divided by the variance from each Pattern feature.", init<const data::Database<data::SimplePatternSet>&>((arg("pattern_set"))))
    .def("mean", &data::NormalizationOperator::mean, (arg("self")), "Returns the Pattern containing the set mean", return_internal_reference<>())
    .def("stddev", &data::NormalizationOperator::stddev, (arg("self")), "Returns the Pattern containing the set variance", return_internal_reference<>())
		;
	
  class_<data::NormaliseOperator, boost::shared_ptr<data::NormaliseOperator>, bases<data::PatternOperator>, boost::noncopyable>("NormaliseOperator", "Generates a Pattern with the same length as the previous, but with the mean removed from each Pattern feature.", init<const data::Database<data::SimplePatternSet>&>((arg("pattern_set"))))
		;

  class_<data::RemoveDBMeanOperator, boost::shared_ptr<data::RemoveDBMeanOperator>, bases<data::PatternOperator>, boost::noncopyable>("RemoveDBMeanOperator", "Generates a Pattern with the same length as the previous, but with the mean removed from each Pattern feature.", init<const data::Database<data::SimplePatternSet>&>((arg("pattern_set"))))
		;

  class_<data::RemoveMeanOperator, boost::shared_ptr<data::RemoveMeanOperator>, bases<data::PatternOperator>, boost::noncopyable>("RemoveMeanOperator", "Generates a Pattern with the same length as the previous, but with the mean removed from each Pattern feature.", init<>()) 
		;

  class_<data::EnergyNormaliseOperator, boost::shared_ptr<data::EnergyNormaliseOperator>, bases<data::PatternOperator>, boost::noncopyable>("EnergyNormaliseOperator", "Generates a Pattern with the same length as the previous, but with its energy normalised to the square sum of all features is 1.", init<>())
    ;
}
