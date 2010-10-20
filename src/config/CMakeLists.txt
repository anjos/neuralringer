project(config)
cmake_minimum_required(VERSION 2.6)

# This defines the dependencies of this package
set(deps sys data) #other nlab subprojects
set(shared "") #shared externals to link against (link)
add_definitions(-D__PACKAGE__="config")

# If we have google-perftools installed, enable the HAS_GOOGLE_PERFTOOLS flag,
# so profiling extensions can be compiled in.
if(googlePerfTools_FOUND)
  set(shared "${shared};${googlePerfTools_LIBRARIES}")
  add_definitions(-DHAS_GOOGLE_PERFTOOLS)
endif(googlePerfTools_FOUND)

# This defines the list of source files inside this package.
set(src
   "src/Header.cxx"
   "src/Network.cxx"
   "src/NeuronBackProp.cxx"
   "src/Neuron.cxx"
   "src/SynapseBackProp.cxx"
   "src/Synapse.cxx"
   "src/SynapseRProp.cxx"
   )

include(../cmake/macros.cmake)
nlab_library(config "${src}" "${deps}" "${shared}")

# Defines tests for this package
#nlab_test(core tensor test/tensor.cc)

# Defines benchmarks we make available
#nlab_benchmark(core tensor benchmark/tensor.cc)

# Python bindings
set(pysrc
   "python/src/main.cc"
   )
 
nlab_python_bindings(config "${pysrc}")
nlab_python_install(config)
