#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Wed 20 Oct 2010 05:45:02 PM CEST 

"""Trains an MLP neural net (RProp technique) using Neural Lab, computes output
scores and analyzes the network performance on the given data sets.
"""

import os, sys, time
import nlab

def get_options():
  import optparse

  parser = optparse.OptionParser(description=__doc__)
  parser.add_option('--train', dest="trainset",
      help="the input dataset containing the training data", metavar="FILE")
  parser.add_option('--devel', dest="develset",
      help="the input dataset containing the development data", metavar="FILE")
  parser.add_option('--test', dest="testset",
      help="the input dataset containing the final test data", metavar="FILE")
  parser.add_option('--hidden', dest="nhidden", default=3,
      help="number of neurons on the MLP hidden layer", metavar="INT")
  parser.add_option('--batch-size', dest="batch_size", default=1,
      help="how many patterns to pick in each class for a single train step", metavar="INT")
  parser.add_option('--stop-after', dest="stop", default=100,
      help="how many training cycles should go by w/o improvements to the classification, before I stop training", metavar="INT")
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
  train.set_target(targets)
  devel = nlab.data.Database(options.develset)
  devel.set_target(targets)
  test  = nlab.data.Database(options.testset)
  test.set_target(targets)

  #calculates the normalization factor 
  mean, stddev = [nl.data.Pattern(k) for k in nlab.data.extractors(train)]

  #creates the network
  net = nlab.network.MLP(train.pattern_size(), [options.nhidden],
      train.size(), [True, True],
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #hidden layer
      nlab.config.NeuronBackProp(nlab.config.NeuronBackProp.TANH),
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #output layer
      nlab.config.NeuronBackProp(nlab.config.NeuronBackProp.TANH),
      nlab.config.SynapseStrategyType.SYNAPSE_RPROP,
      nlab.config.SynapseRProp(0.1), mean, stddev, reporter)
  
  step = 0 #current training step
  observer = nlab.error.Observer(net, train, devel, test, reporter)
  while True: #trains until net statibilizes
    data, target = train.random_sample(options.batch_size)
    net.train(data, target)
    observer.evaluate()
    print observer.statistics()
    if options.stop < observer.stalled: break
  
  #Finalize all statistics using the best network saved so far.
  observer.save_best('trained-network.xml')
  analyzer = nlab.error.Analyzer(observer)
  analyzer.pdf_all('trained-network.pdf')
  #print analyzer.error()
