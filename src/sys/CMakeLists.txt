project(sys)
cmake_minimum_required(VERSION 2.6)

# This defines the dependencies of this package
set(deps "") #other nlab subprojects
set(shared "${LIBXML2_LIBRARIES}") #shared externals to link against (link)
add_definitions(-D__PACKAGE__="sys")

# If we have google-perftools installed, enable the HAS_GOOGLE_PERFTOOLS flag,
# so profiling extensions can be compiled in.
if(GOOGLE_PERFTOOLS_FOUND)
  set(shared "${shared};${GOOGLE_PERFTOOLS_LIBRARIES}")
endif(GOOGLE_PERFTOOLS_FOUND)

# This defines the list of source files inside this package.
set(src
   "src/debug.cxx"
   "src/Exception.cxx"
   "src/File.cxx"
   "src/libxml2_Codec.cxx"
   "src/LocalReporter.cxx"
   #"src/OptParser.cxx"
   "src/Plain.cxx"
   "src/Reporter.cxx"
   "src/util.cxx"
   "src/XMLProcessor.cxx"
   "src/xmlutil.cxx"
   )

include(../cmake/macros.cmake)
nlab_library(sys "${src}" "${deps}" "${shared}")

# Defines tests for this package
#nlab_test(core tensor test/tensor.cc)

# Defines benchmarks we make available
#nlab_benchmark(core tensor benchmark/tensor.cc)

# Python bindings
set(pysrc
   "python/src/reporter.cc"
   "python/src/main.cc"
   )
 
nlab_python_bindings(sys "${pysrc}")
nlab_python_install(nlsys)
