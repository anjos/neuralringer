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
  parser.add_option('--train', dest="trainset", default="train.xml",
      help="the input dataset containing the training data (defaults to %default)", metavar="FILE")
  parser.add_option('--devel', dest="develset", default="devel.xml",
      help="the input dataset containing the development data (defaults to %default)", metavar="FILE")
  parser.add_option('--test', dest="testset", default="test.xml",
      help="the input dataset containing the final test data (defaults to %default)", metavar="FILE")
  parser.add_option('--hidden', dest="nhidden", default=5,
      help="number of neurons on the MLP hidden layer (defaults to %default)", metavar="INT")
  parser.add_option('--batch-size', dest="batch_size", default=5,
      help="how many patterns to pick in each class for a single train step (defaults to %default)", metavar="INT")
  parser.add_option('--stop-after', dest="stop", default=10000,
      help="how many training cycles should go by w/o improvements to the classification, before I stop training (defaults to %default)", metavar="INT")
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
  mean, stddev = [nlab.data.Pattern(k) for k in nlab.data.extractors.standard(train)]

  #creates the network
  actfun = nlab.config.NeuronBackProp.ActivationFunction.TANH
  net = nlab.network.MLP(train.pattern_size(), [options.nhidden],
      1, [True, True],
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #hidden layer
      nlab.config.NeuronBackProp(actfun),
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #output layer
      nlab.config.NeuronBackProp(actfun),
      nlab.config.SynapseStrategyType.SYNAPSE_RPROP,
      nlab.config.SynapseRProp(0.05), mean, stddev, reporter)
  
  step = 1 #current training step
  observer = nlab.error.Observer(net, train, devel, test)
  while True: #trains until net statibilizes
    data, target = train.random_sample(options.batch_size)
    net.train(data, target)
    observer.evaluate()
    if not step%100: print observer.statistics()
    if options.stop < observer.stalled: break
    step += 1
  
  #Finalize all statistics using the best network saved so far.
  observer.save_best('trained-network.xml')
  analyzer = nlab.error.Analyzer(observer)
  analyzer.pdf_all('trained-network.pdf')
  #print analyzer.error()

if __name__ == '__main__': main()
