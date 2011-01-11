#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 06 Dec 2010 13:54:41 CET 

"""Discriminates the types of videos that fail in the anti-spoofing counter
measure.
"""


import os, sys, time
import nlab

def get_options():
  import optparse

  parser = optparse.OptionParser(description=__doc__)
  parser.add_option('--train', dest="trainset", default="train.xml",
      help="the input dataset containing the training data (defaults to %default)", metavar="FILE")
  parser.add_option('--devel', dest="develset", default="devel.xml",
      help="the input dataset containing the development data (defaults to %default)", metavar="FILE")
  parser.add_option('--test', dest="testset", default="test.xml",
      help="the input dataset containing the final test data (defaults to %default)", metavar="FILE")
  parser.add_option('--network', dest="network", default=None,
      help="file containing the network to test (defaults to %default)",
      metavar="FILE")
  
  options, args = parser.parse_args()

  if len(args) != 0:
    parser.error("this program does not accept positional arguments")

  return options

def main():
  options = get_options()
  reporter = nlab.sys.Reporter()

  #some hard-coded targets
  targets = {'Real-access': (+1,), 'Attack': (-1,)}

  #loads the DBs
  train = nlab.data.Database(options.trainset)
  train.name = 'train'
  train.set_target(targets)
  devel = nlab.data.Database(options.develset)
  devel.set_target(targets)
  devel.name = 'devel'
  test  = nlab.data.Database(options.testset)
  test.set_target(targets)
  test.name = 'test'

  #loads the network
  net = nlab.network.Network(options.network, reporter)

  def inline_dict(d):
    to_print = []
    for k, v in d.iteritems():
      to_print.append('%s=%s' % (k,v))
    return ' '.join(to_print)

  #passes all the data through the network and record in txt files.
  for db in (train, devel, test):
    output = {}
    for cls in targets.keys():
      ips = nlab.data.PatternSet(db.data[cls])
      output[cls] = nlab.data.PatternSet(len(db.data[cls]), 1, 0.0)
      net.run(ips, output[cls])
    dir = os.path.dirname(os.path.realpath(options.network))
    f = open(os.path.join(dir, db.name + '.txt'), 'wt')
    for cls in targets.keys():
      for patidx in range(output[cls].size()):
        f.write(cls + ' ' + inline_dict(db.properties[cls][patidx]) + \
            ' output=%.6e' % (output[cls][patidx][0],) + '\n')
    f.close()

if __name__ == '__main__': main()

