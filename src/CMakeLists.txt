# Configures and builds nlab
# Andre Anjos - 13.august.2010

project(neurallab)
cmake_minimum_required(VERSION 2.6)

# Set the compiler flags and build type(release or debug)
set(COMMON_FLAGS "-pthread -g -Wall")
add_definitions(-DNR_VERSION="1.0.0")

# Cmake does not support include_directories(SYSTEM) correctly on OSX, so the
# compiler issues warnings/errors on Python and ffmpeg includes that do not 
# respect a strict standard. To make it compile, we do not use "-pedantic" on
# OSX.
if(NOT (${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
  set(COMMON_FLAGS "${COMMON_FLAGS} -pedantic")
endif(NOT (${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))

set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} -O2 -march=generic")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLAGS} -DRINGER_DEBUG=3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

# This is our RPATH policy
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# This is where we install cmake files for external projects
set(cmakedir share/nlab/cmake)

# Sets up externals and install the central dependency file
include(cmake/nlab-external.cmake)
foreach(ext ${nlab_DEPENDENCIES}) 
  find_package(${ext} REQUIRED PATHS external)
  install(FILES external/${ext}Config.cmake DESTINATION ${cmakedir})
endforeach(ext ${nlab_DEPENDENCIES}) 
foreach(ext ${nlab_OPTIONALS}) 
  find_package(${ext} PATHS external)
  install(FILES external/${ext}Config.cmake DESTINATION ${cmakedir})
endforeach(ext ${nlab_OPTIONALS}) 
install(FILES cmake/nlab-external.cmake DESTINATION ${cmakedir})

# This will install the schema files
set(schemadir share/nlab/schema)
install(FILES schema/network.xsd;schema/database.xsd DESTINATION ${schemadir})

# Enables the testing framework
enable_testing()

# This makes sure we can reach all external headers
if(NOT ("$ENV{CMAKE_PREFIX_PATH}" STREQUAL ""))
  string(REGEX MATCHALL "[^:]+" prefix_path $ENV{CMAKE_PREFIX_PATH})
  list(REMOVE_DUPLICATES prefix_path)
  foreach(prefix ${prefix_path})
    include_directories(SYSTEM "${prefix}/include")
    link_directories("${prefix}/lib")
  endforeach(prefix ${prefix_path})
endif(NOT ("$ENV{CMAKE_PREFIX_PATH}" STREQUAL ""))

# This makes sure we have all headers within reach before starting to compile
include_directories(${CMAKE_INSTALL_PREFIX}/include/nlab)

add_subdirectory(python)
add_subdirectory(sys)
add_subdirectory(config)
add_subdirectory(data)
add_subdirectory(network)

install(EXPORT nlab DESTINATION ${cmakedir})
install(FILES cmake/nlabConfig.cmake DESTINATION ${cmakedir})
