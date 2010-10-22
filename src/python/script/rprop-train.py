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
  parser.add_option('--hidden', dest="nhidden",
      help="number of neurons on the MLP hidden layer", metavar="INT")
  parser.add_option('--epoch', dest="epoch",
      help="the size of each training epoch", metavar="INT")
  parser.add_option('--stop-after', dest="stop",
      help="how many training cycles should go by w/o improvements to the classification, before I stop training", metavar="INT")
  options, args = parser.parse_args()

  if len(args) != 0:
    parser.error("this program does not accept positional arguments")

  return options

def prepare_inputs(database):
  """Creates the input dictionaries, with the respective targets"""
  retval = {
      'input': nlab.data.PatternSet(1, 1),
      'target': nlab.data.PatternSet(1, 1),
      }
  database.merge(retval['input'])
  database.merge_target(False, -1, +1, retval['target'])
  return retval

def main():
  options = get_options()
  reporter = nlab.sys.Reporter()

  #loads the DBs
  traindb = nlab.data.Database(options.trainset, reporter)
  develdb = nlab.data.Database(options.develset, reporter)
  testdb  = nlab.data.Database(options.testset,  reporter)

  #tune input DBs for size/randomness
  traindb_normalized = nlab.data.Database(traindb)
  traindb_normalized.normalise()
  #calculates the normalization factor based on the normalized trainset
  norm_op = nlab.data.NormalizationOperator(traindb_normalized)

  #creates the network
  net = nlab.network.MLP(traindb.pattern_size(), [options.nhidden],
      traindb.size(), [True, True],
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #hidden layer
      nlab.config.NeuronBackProp(nlab.config.NeuronBackProp.TANH),
      nlab.config.NeuronStrategyType.NEURON_BACKPROP, #output layer
      nlab.config.NeuronBackProp(nlab.config.NeuronBackProp.TANH),
      nlab.config.SynapseStrategyType.SYNAPSE_RPROP,
      nlab.config.SynapseRProp(0.1),
      norm_op.mean(), norm_op.stddev(),
      reporter)
  
  #prepares all pattern sets for training the network and for testing
  train_normalized = prepare_inputs(traindb_normalized)
  train = prepare_inputs(traindb)
  devel = prepare_inputs(develdb)
  test  = prepare_inputs(testdb)

  #some configuration stuff
  header = nlab.config.Header('Andre Anjos', 'Bla', '1.0', int(time.time()),
      'MLP - RProp-trained - Network')

  #TODO: capture MSE and save to disk
  step = 0 #current training step
  observer = nlab.perf.Observer(net, train, devel, test, reporter)
  while True: #trains until net statibilizes
    net.train(train_normalized['input'], train_normalized['target'], 
        options.epoch)
    observer.evaluate()
    print observer.statistics()
    if options.stop < observer.stalled: break
  
  #Finalize all statistics using the best network saved so far.
  bestnet = observer.best_classifier_seen[1]
