# Configures a remote nlab installation
# Andre Anjos - 13.august.2010

# This file is installed on every nlab build so *external* code can compile in
# a few easy steps. If you want to change the way nlab itself is compiled,
# this is *not* the place.

# Locates and loads all nlab exported dependencies
find_file(nlab_BUILD_INFO nlab.cmake)
include(${nlab_BUILD_INFO})

# Defines the includes
get_filename_component(nlab_CMAKE_DIR ${nlab_BUILD_INFO} PATH)
get_filename_component(nlab_SHARE_DIR ${nlab_CMAKE_DIR} PATH)
get_filename_component(nlab_PREFIX ${nlab_SHARE_DIR} PATH)

# Loads all externals
find_file(nlab_DEPENDENCIES_FILE nlab-external.cmake)
include("${nlab_DEPENDENCIES_FILE}")

set(nlab_INCLUDE_DIRS ${nlab_PREFIX}/include/nlab)
set(nlab_LIBRARY_DIRS ${nlab_PREFIX}/lib)
foreach(dep ${nlab_DEPENDENCIES})
  find_package(${dep})
  set(nlab_INCLUDE_DIRS "${nlab_INCLUDE_DIRS};${${dep}_INCLUDE_DIRS}")
  set(nlab_LIBRARY_DIRS "${nlab_LIBRARY_DIRS};${${dep}_LIBRARY_DIRS}")
endforeach(dep ${nlab_DEPENDENCIES})
list(REMOVE_DUPLICATES nlab_INCLUDE_DIRS)
list(REMOVE_DUPLICATES nlab_LIBRARY_DIRS)

# This macro helps users to build nlab-based executables
macro(nlab_add_executable name sources dependencies)
  include_directories(${nlab_INCLUDE_DIRS})
  link_directories(${nlab_LIBRARY_DIRS})
  add_executable(${name} "${sources}")
  foreach(dep ${dependencies})
    target_link_libraries(${name} nlab_${dep})
  endforeach(dep ${dependencies})
endmacro(nlab_add_executable name sources dependencies)
