from libpynlab_network import *
__all__ = dir()

def _clone(self, reporter):
  """Clones this network by saving it and reloading from disk."""
  from tempfile import NamedTemporaryFile
  from os import unlink

  f = NamedTemporaryFile()
  f.close()
  name = f.name
  del f
  self.save(name)
  retval = Network(name, reporter)
  os.unlink(name)
  return retval

Network.clone = _clone
