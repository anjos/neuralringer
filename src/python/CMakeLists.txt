project(python)

cmake_minimum_required(VERSION 2.6)

# Installs python top-level wrapper
if (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
  add_custom_target(nlab-python-install ALL mkdir -p ${PYTHON_INSTALL_DIRECTORY} COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/nlab ${PYTHON_INSTALL_DIRECTORY} COMMENT "Installing nlab python files...")
  # Installs all python scripts
  file(COPY script/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin FILES_MATCHING PATTERN "*.py")

  # Compiles all installed python files
  add_custom_target(python-compilation ALL ${PYTHON_EXECUTABLE} -m compileall . 
    WORKING_DIRECTORY ${PYTHON_INSTALL_DIRECTORY} COMMENT "Compiling python files")

  # Finally, adds all python tests
  #nlab_python_add_test(python-video-tests
  #  ${CMAKE_CURRENT_SOURCE_DIR}/test/test_video.py)
endif (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND Boost_FOUND)
