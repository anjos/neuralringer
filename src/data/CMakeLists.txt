project(data)
cmake_minimum_required(VERSION 2.6)

# This defines the dependencies of this package
set(deps sys) #other nlab subprojects
set(shared "${GSL_LIBRARIES}") #shared externals to link against (link)
add_definitions(-D__PACKAGE__="data")

# If we have google-perftools installed, enable the HAS_GOOGLE_PERFTOOLS flag,
# so profiling extensions can be compiled in.
if(googlePerfTools_FOUND)
  set(shared "${shared};${googlePerfTools_LIBRARIES}")
endif(googlePerfTools_FOUND)

# This defines the list of source files inside this package.
set(src
   "src/Database.cxx"
   "src/EnergyNormaliseOperator.cxx"
   "src/Header.cxx"
   "src/MaxExtractor.cxx"
   "src/MeanExtractor.cxx"
   "src/MinExtractor.cxx"
   "src/NormaliseOperator.cxx"
   "src/NormalizationOperator.cxx"
   "src/Pattern.cxx"
   "src/PatternSet.cxx"
   "src/RandomInteger.cxx"
   "src/RemoveDBMeanOperator.cxx"
   "src/RemoveMeanOperator.cxx"
   "src/SumExtractor.cxx"
   "src/util.cxx"
   "src/VarianceExtractor.cxx"
   )

include(../cmake/macros.cmake)
nlab_library(data "${src}" "${deps}" "${shared}")

# Defines tests for this package
#nlab_test(core tensor test/tensor.cc)

# Defines benchmarks we make available
#nlab_benchmark(core tensor benchmark/tensor.cc)

# Python bindings
set(pysrc
  "python/src/database.cc"
  "python/src/operator.cc"
  "python/src/main.cc"
   )
 
nlab_python_bindings(data "${pysrc}")
nlab_python_install(data)
