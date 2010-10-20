# Andre Anjos <andre.anjos@idiap.ch>
# 4/August/2010

# These are a few handy macros for Torch compilation.

# Builds and installs a shared library with dependencies
macro(nlab_shlib libname sources dependencies externals installdir)
  add_library(${libname} SHARED ${sources})
  if(NOT ("${dependencies}" STREQUAL ""))
    foreach(dep ${dependencies})
      target_link_libraries(${libname} nlab_${dep})
    endforeach(dep ${dependencies})
  endif(NOT ("${dependencies}" STREQUAL ""))
  if(NOT ("${externals}" STREQUAL ""))
    foreach(ext ${externals})
      target_link_libraries(${libname} ${ext})
    endforeach(ext ${externals})
  endif(NOT ("${externals}" STREQUAL ""))
  install(TARGETS ${libname} EXPORT nlab LIBRARY DESTINATION ${installdir})
endmacro(nlab_shlib libname sources dependencies)

# Builds and installs a shared library with dependencies
macro(nlab_archive libname sources dependencies installdir)
  add_library(${libname}-static STATIC ${sources})
  if(NOT ("${dependencies}" STREQUAL ""))
    foreach(dep ${dependencies})
      target_link_libraries(${libname}-static nlab_${dep}-static)
    endforeach(dep ${dependencies})
  endif(NOT ("${dependencies}" STREQUAL ""))
  set_target_properties(${libname}-static PROPERTIES OUTPUT_NAME ${libname})
  set_target_properties(${libname}-static PROPERTIES PREFIX "lib")
  install(TARGETS ${libname}-static EXPORT nlab ARCHIVE DESTINATION ${installdir})
endmacro(nlab_archive sources dependencies)

# Builds libraries for a subproject and installs headers. Wraps every of those
# items in an exported CMake module to be used by other libraries in or outside
# the project.
# 
# The parameters:
# nlab_library -- This macro's name
# package -- The base name of this package, so everything besides "nlab_",
# which will get automatically prefixed
# src -- The sources for the libraries generated
# deps -- This is a list of other subprojects that this project depends on.
# shared -- This is a list of shared libraries to which shared libraries
# generated on this project must link against.
macro(nlab_library package src deps shared)
  # We set this so we don't need to become repetitive.
  set(libname nlab_${package})
  set(libdir lib)
  set(incdir include/nlab)

  include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

  # This adds target (library) nlab_<package>, exports into "nlab"
  nlab_shlib(${libname} "${src}" "${deps}" "${shared}" ${libdir})

  if ("${TORCH_BUILD_STATIC_LIBS}")
    # This adds target (library) nlab_<package>-static, exports into "nlab"
    nlab_archive(${libname} "${src}" "${deps}" ${libdir})
  endif ("${TORCH_BUILD_STATIC_LIBS}")

  # This installs all headers to the destination directory
  add_custom_command(TARGET ${libname} POST_BUILD COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/${incdir} COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${package} ${CMAKE_INSTALL_PREFIX}/${incdir} COMMENT "Installing ${package} headers...")
endmacro(nlab_library)

# Creates a standard Torch test.
macro(nlab_test package name src)
  set(testname nlabtest_${package}_${name})

  # Please note we don't install test executables
  add_executable(${testname} ${src})
  target_link_libraries(${testname} nlab_${package})
  target_link_libraries(${testname} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_test(cxx-${package}-${name} ${testname} --log_level=test_suite)
endmacro(nlab_test package src)

# Creates a standard Torch benchmark.
macro(nlab_benchmark package name src)
  set(bindir bin)
  set(progname nlabbench_${package}_${name})

  add_executable(${progname} ${src})
  target_link_libraries(${progname} nlab_${package})
  install(TARGETS ${progname} RUNTIME DESTINATION ${bindir})
endmacro(nlab_benchmark package name src)

macro(nlab_python_bindings package src)
  if (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
    # Some preparatory work
    set(libname pynlab_${package})
    set(libdir lib)
    include_directories(${PYTHON_INCLUDE_DIRS})

    # Building the library itself
    add_library(${libname} SHARED ${src})
    set_target_properties(${libname} PROPERTIES SUFFIX ".so")
    target_link_libraries(${libname} nlab_${package})
    target_link_libraries(${libname} ${Boost_PYTHON_LIBRARY})
    target_link_libraries(${libname} ${PYTHON_LIBRARIES})

    # And its installation
    install(TARGETS ${libname} LIBRARY DESTINATION ${libdir})
  else (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
    message("Boost::Python bindings for ${package} are DISABLED: externals NOT FOUND!")
  endif (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
endmacro(nlab_python_bindings package name src)

# Installs python files and compile them
macro(nlab_python_install package)
  if (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
    set(pydir ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION}/nlab)
    add_custom_target(${package}-python-install cp -r ${CMAKE_CURRENT_SOURCE_DIR}/python/${package} ${pydir} COMMENT "Installing ${package} python files...")
    add_dependencies(${package}-python-install pynlab_${package})
    add_dependencies(${package}-python-install nlab-python-install)
    add_dependencies(python-compilation ${package}-python-install)
  endif (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
endmacro(nlab_python_install package)
