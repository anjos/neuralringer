# Finds and configures google-perftools if it exists on the system. 
# Andre Anjos - 07.september.2010

# We start by defining GOOGLE_PERFTOOLS_FOUND to false
set(GOOGLE_PERFTOOLS_FOUND "NO" CACHE INTERNAL "package")

# Here we do some variable cleanup and adjustments
find_path(GOOGLE_PERFTOOLS_INCLUDE NAMES google/profiler.h)
set(GOOGLE_PERFTOOLS_INCLUDE_DIRS ${GOOGLE_PERFTOOLS_INCLUDE})

find_library(GOOGLE_PERFTOOLS_LIBRARY NAMES profiler)
set(GOOGLE_PERFTOOLS_LIBRARIES ${GOOGLE_PERFTOOLS_LIBRARY} CACHE INTERNAL "libraries")

if(GOOGLE_PERFTOOLS_INCLUDE AND GOOGLE_PERFTOOLS_LIBRARY)
  set(GOOGLE_PERFTOOLS_FOUND "YES" CACHE INTERNAL "package")
else(GOOGLE_PERFTOOLS_INCLUDE AND GOOGLE_PERFTOOLS_LIBRARY)
  # This will say why we have got to that conclusion
  set(GOOGLE_PERFTOOLS_FOUND "NO" CACHE INTERNAL "package")
  if (NOT GOOGLE_PERFTOOLS_INCLUDE)
    message(STATUS "Google Perftools <google/profiler.h> not found!")
  endif (NOT GOOGLE_PERFTOOLS_INCLUDE)
  if (NOT GOOGLE_PERFTOOLS_LIBRARY)
    message(STATUS "Google Perftools libprofiler.so not found!")
  endif (NOT GOOGLE_PERFTOOLS_LIBRARY)
endif(GOOGLE_PERFTOOLS_INCLUDE AND GOOGLE_PERFTOOLS_LIBRARY)

if(GOOGLE_PERFTOOLS_FOUND)
 include_directories(SYSTEM ${GOOGLE_PERFTOOLS_INCLUDE_DIRS})
 message(STATUS "Found Google Perftools: ${GOOGLE_PERFTOOLS_INCLUDE}")
else(GOOGLE_PERFTOOLS_FOUND)
 message(STATUS "Google Perftools NOT FOUND: Disabled")
endif(GOOGLE_PERFTOOLS_FOUND)
