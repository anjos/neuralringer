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
  parser.add_option('--batch-size', dest="batch_size", default=20,
      help="how many patterns to pick in each class for a single train step (defaults to %default)", metavar="INT")
  parser.add_option('--epoch-size', dest="epoch_size", default=100,
      help="How many training steps to wait until sample network performance (defaults to %default)", metavar="INT")
  parser.add_option('--stop-after', dest="stop", default=10000,
      help="how many training steps should go by w/o improvements to the classification, before I stop training (defaults to %default)", metavar="INT")
  parser.add_option('--weight-update', dest="weight_update", default=0.1,
      help="The weight update constant for the R-Prop algorithm (defaults to %default)", metavar="FLOAT")
  
  options, args = parser.parse_args()

  if len(args) != 0:
    parser.error("this program does not accept positional arguments")

  #some tweaking
  options.nhidden = int(options.nhidden)
  options.batch_size = int(options.batch_size)
  options.epoch_size = int(options.epoch_size)
  options.stop = int(options.stop)
  options.weight_update = float(options.weight_update)

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
      nlab.config.SynapseRProp(options.weight_update), mean, stddev, reporter)
  
  step = 1 #current training step
  observer = nlab.error.Observer(net, train, devel, test)

  try:
    while True: #trains until net statibilizes
      data, target = train.random_sample(options.batch_size)
      net.train(data, target)
      if not step % options.epoch_size:
        observer.evaluate(step)
        print observer.statistics(step)
        if options.stop < observer.stalled(step): break
      step += 1
  except KeyboardInterrupt: #we still would like to run analysis
    pass
  
  #Finalize all statistics using the best network saved so far.
  observer.save_best('network.xml')
  analyzer = nlab.error.Analyzer(observer)
  hint = os.sep.join(os.path.realpath(os.curdir).split(os.sep)[-3:])
  analyzer.pdf_all('results.pdf', hint)
  print analyzer.error()

if __name__ == '__main__': main()
