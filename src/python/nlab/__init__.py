"""Neural Lab is a toolkit of ANN utilities.
"""

# This is a fix to the problem of using templates/static/exceptions/dynamic
# casts with boost.python. It makes all symbols loaded by python from this
# point onwards global
import sys as pysys
import ctypes
default_flags = pysys.getdlopenflags()
pysys.setdlopenflags(default_flags|ctypes.RTLD_GLOBAL)

import nlsys as sys
import data
import config
import network

pysys.setdlopenflags(default_flags)
del default_flags

__all__ = ['sys', 'data', 'config', 'network']
