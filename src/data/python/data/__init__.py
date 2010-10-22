from libpynlab_data import *

import extractors
from Database import Database

__all__ = dir()

def pattern_str(self):
  return str([self[k] for k in range(len(self))])

Pattern.__str__ = pattern_str
