#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Fri 22 Oct 2010 09:12:18 AM CEST 

"""A pure python implementation of a neural lab Database.
"""

__all__ = ['Database']

from xml.dom.minidom import parse as xml_parse
from . import PatternSet
from math import ceil
from random import sample, shuffle

def db_from_xml(source):
  """Loads the whole of the Database data from an XML file."""

  def load_features(c):
    """Loads all feature of a class, returns a list."""
    l = []
    for f in c.getElementsByTagName('feature'):
      l.append(tuple([float(k) for k in f.childNodes[0].wholeText.split()]))
    lengths = [len(k) for k in l]
    if max(lengths) != min(lengths):
      raise RuntimeError, "Feature lengths should be the same in class %s (min: %d; max: %d)"  % (c.getAttribute('name'), min(lengths), max(lengths))
    return (lengths[0], l)

  dom = xml_parse(source)
  retval = {}
  pattern_length = None
  for c in dom.getElementsByTagName('class'):
    length, retval[c.getAttribute('name')] = load_features(c)
    retval[c.getAttribute('name')] = tuple(retval[c.getAttribute('name')])
    if not pattern_length: pattern_length = length
    elif length != pattern_length:
      raise RuntimeError, "Features of class %s do not seem to follow the standard for the database (normal: %d; %s: %d)" % (c.getAttribute('name'), pattern_length, c.getAttribute('name'), length)

  return retval

class Database(object):
  """A database contains the data for different classes."""

  def __init__(self, *args):
    self.data = {}
    self.target = {}

    if len(args) == 1:
      if isinstance(args[0], Database):
        other = args[0]
        for k, v in other.data.iteritems(): self.data[k] = v
        for k, v in other.target.iteritems(): self.target[k] = v 
      elif isinstance(args[0], str): #load from file
        f = open(args[0], 'rt')
        self.data = db_from_xml(f)
        f.close()
      else:
        raise RuntimeError, "Cannot construct a new Databse from %s" % \
            type(args[0])

    else:
      raise RuntimeError, "Can only have 1 arg. for Database __init__()"

  def __str__(self):
    retval = ''
    for k in self.data.iterkeys():
      retval += 'Class %s\n' % k
      for v in self.data[k]:
        retval += '  %s => %s\n' % (v, self.target[k])
    return retval 

  def size(self):
    return len(self.data)

  def pattern_size(self):
    return len(self.data[self.data.keys()[0]][0])

  def cat(self):
    """Converts the whole database into a data::PatternSet by concatenating
    the data from its classes in an orderly manner."""
    l = []
    for k in sorted(self.data.keys()): l += self.data[k]
    return PatternSet(l)

  def set_target(self, target):
    """Sets the targets for the current classes."""

    if len(target) != len(self.data):
      raise RuntimeError, "Input target dictionary has not the same length as self data"
  
    newtargets = {}
    for k in sorted(self.data.keys()): newtargets[k] = tuple(target[k])
    self.target = newtargets

  def cat_target(self):
    """Returns a pattern set containing the targets for the inputs defined by
    cat() above."""

    l = []
    for k in sorted(self.data.keys()):
      for i in self.data[k]: l.append(self.target[k])
    return PatternSet(l)

  def random_sample(self, n):
    """Randomly pick n elements from each class. Returns two pattern sets for
    input and target."""

    d = []
    for k, v in self.data.iteritems():
      d += [(i, self.target[k]) for i in sample(v, n)]
    shuffle(d)
    return PatternSet([k[0] for k in d]), PatternSet([k[1] for k in d]) 
