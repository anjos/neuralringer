/**
 * @file network.cc
 * @author <a href="mailto:andre.anjos@idiap.ch">Andre Anjos</a>
 *
 * @brief Binds the most important bits of the Network class
 */

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "network/MLP.h"

using namespace boost::python;

boost::shared_ptr<network::MLP> make_network_1(const size_t input,
	 list hidden, //const std::vector<size_t>& hidden,
	 const size_t output,
	 const config::NeuronStrategyType& neu_strat_type,
	 const config::Parameter* neu_params,
	 const config::SynapseStrategyType& syn_strat_type,
	 const config::Parameter* syn_params,
	 const data::Pattern& input_subtract,
	 const data::Pattern& input_divide,
	 sys::Reporter& reporter) {
  std::vector<size_t> h;
  for (size_t i=0; i<hidden.attr("__len__")(); ++i) {
    h.push_back(extract<size_t>(hidden[i]));
  }
  return boost::shared_ptr<network::MLP>(new network::MLP(input, h, output, 
        neu_strat_type, neu_params, syn_strat_type, syn_params,
        input_subtract, input_divide, reporter));
}

boost::shared_ptr<network::MLP> make_network_2(const size_t input,
	 list hidden, //const std::vector<size_t>& hidden,
	 const size_t output,
	 list bias, //const std::vector<bool>& bias,
	 const config::NeuronStrategyType& hidneu_strat_type,
	 const config::Parameter* hidneu_params,
	 const config::NeuronStrategyType& outneu_strat_type,
	 const config::Parameter* outneu_params,
	 const config::SynapseStrategyType& syn_strat_type,
	 const config::Parameter* syn_params,
	 const data::Pattern& input_subtract,
	 const data::Pattern& input_divide,
	 sys::Reporter& reporter) {

  std::vector<size_t> h;
  for (size_t i=0; i<hidden.attr("__len__")(); ++i) {
    h.push_back(extract<size_t>(hidden[i]));
  }
  std::vector<bool> b;
  for (size_t i=0; i<bias.attr("__len__")(); ++i) {
    h.push_back(extract<bool>(bias[i]));
  }
  return boost::shared_ptr<network::MLP>(new network::MLP(input, h, output, b,
        hidneu_strat_type, hidneu_params, outneu_strat_type, outneu_params, 
        syn_strat_type, syn_params, input_subtract, input_divide, reporter));
}

void bind_network()
{
  class_<network::Network, boost::shared_ptr<network::Network>, boost::noncopyable>("Network", "Interface to load/save network data from files", init<const std::string&, sys::Reporter&>())
    .def("save", &network::Network::save, (arg("self"), arg("filename")), "Saves the current network into a XML file.")
    .def("dot", &network::Network::dot, (arg("self"), arg("filename")), "Draws using dot, the current network")
    .add_property("input_size", &network::Network::input_size)
    .add_property("output_size", &network::Network::output_size)
    .def("train", (void (network::Network::*)(const data::Pattern&, const data::Pattern&))&network::Network::train, (arg("self"), arg("data"), arg("target")), "Single-step training")
    .def("train", (void (network::Network::*)(const data::PatternSet&, const data::PatternSet&))&network::Network::train, (arg("self"), arg("data"), arg("target")), "Train using all data from the given set, in a single step")
    .def("run", (void (network::Network::*)(const data::Pattern&, data::Pattern&))&network::Network::run, (arg("self"), arg("input"), arg("output")), "Single test")
    .def("run", (void (network::Network::*)(const data::PatternSet&, data::PatternSet&))&network::Network::run, (arg("self"), arg("input"), arg("output")), "Batch test")
    ;

  class_<network::MLP, boost::shared_ptr<network::MLP>, bases<network::Network>, boost::noncopyable>("MLP", "Interface to create, load and safe Multi-Layer Perceptrons", no_init)
    .def("__init__", make_constructor(make_network_1, default_call_policies(), (arg("input"), arg("hidden"), arg("output"), arg("neuron_strategy_type"), arg("neuron_parameters"), arg("synapse_strategy_type"), arg("synapse_parameters"), arg("input_subtract"), arg("input_divide"), arg("reporter"))))
    .def("__init__", make_constructor(make_network_2, default_call_policies(), (arg("input"), arg("hidden"), arg("output"), arg("bias"), arg("hidden_neuron_strategy_type"), arg("hidden_neuron_parameters"), arg("output_neuron_strategy_type"), arg("output_neuron_parameters"), arg("synapse_strategy_type"), arg("synapse_parameters"), arg("input_subtract"), arg("input_divide"), arg("reporter"))))
    ;
}
