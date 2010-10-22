#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Thu 21 Oct 2010 09:14:17 AM CEST 

"""Calculates error curves and the such.
"""

import os, sys, math, tempfile
import matplotlib.pyplot as mpl
from matplotlib.backends.backend_pdf import PdfPages
from data import SimplePatternSet

class Evaluator(object):
  """Expresses the performance of a certain classifier."""

  def __init__(self, train, devel, test):
    """Initializes the Performance object with links to the train input and
    targets as well as the ones for the development set and the test sets."""

    self.train = train
    self.devel = devel
    self.test  = test

    #some buffers
    self.output['train'] = SimplePatternSet(train['target'])
    self.output['devel'] = SimplePatternSet(devel['target'])
    self.output['test']  = SimplePatternSet(test['target'])

  def __call__(self, classifier):
    """Evaluates the performance for the classifier given."""

    classifier.run(self.train['input'], self.output['train'])
    train_mse = self.output['train'].mse(self.train['target'])
    classifier.run(self.devel['input'], self.output['devel'])
    devel_mse = self.output['devel'].mse(self.devel['target'])
    classifier.run(self.test['input'], self.output['test'])
    devel_mse = self.output['test'].mse(self.test['target'])

    return (train_mse, devel_mse, test_mse)

class Observer(object):
  """Objects of this type observe what happens to a classifier while its being
  trained. It can provide clues on which states are worth saving or when to
  stop training a classifier."""

  def __init__(self, classifier, train, devel, test, reporter):
    self.evaluator = Evaluator(train, devel, test)
    self.classifier = classifier
    self.best_classifier_seen = (0, self.classifier.clone(reporter))
    self.mse = []

  def evalulate(self):
    """Evalulates the current classifier performance"""
    train_mse, devel_mse, test_mse = self.evaluator(self.classifier)
    self.mse.append({
      'train': train_mse,
      'devel': devel_mse,
      'test': test_mse,
      })

    if devel_mse < min([k['devel'] for k in self.mse]): #save best state
      self.best_classifier_seen = (len(self.mse),
          self.classifier.clone(self.return))

  def _stalled(self):
    """Determines how many cycles w/o improvement have been observed."""
    return len(self.mse) - self.best_classifier_seen[0]
  
  stalled = property(_stalled)

def far(noise, threshold):
  """Calculates the FAR given a certain threshold."""
  return 100.0*sum([int(k>=threshold) for k in noise])/float(len(noise))

def frr(signal, threshold):
  """Calculates the FRR given a certain threshold."""
  return 100.0*sum([int(k<threshold) for k in signal])/float(len(signal))

def plot_roc(data, name):
  for k in data.iterkeys(): 
    mpl.plot(data[k]['far'], data[k]['frr'], **data[k]['style'])
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.xlabel('FAR (%)')
  mpl.ylabel('FRR (%)')
  mpl.legend()

def plot_det(data, name)
  for k in data.iterkeys(): 
    mpl.loglog(data[k]['far'], data[k]['frr'], **data[k]['style'])
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.xlabel('FAR (%)')
  mpl.ylabel('FRR (%)')
  mpl.legend()

def hter_min_wer(data, omega):
  N = len(data['test']['far']) - 1
  threshold = [2.0*k/N for k in range(-1*(N/2), (N/2) + 1)]
  minwer = 100
  minhter = 100
  for i, k in enumerate(threshold):
    wer = (omega * data['devel']['far'][i]) + \
        ((1-omega) * data['devel']['ffr'][i])
    if wer <= minwer:
      minwer = wer
      minhter = (data['test'][i] + data['test'][i])/2
  return minhter

def plot_epc(data, name):
  N = len(data['test']['far'])
  omega = [k/float(N-1) for k in range(N)]
  test_hter = []
  for k in omega:
    test_hter.append(hter_min_wer(data, k))
  mpl.plot(omega, test_hter, color='black', linestyle='-', label='test', 
           linewidth=2)
  mpl.grid(True)
  mpl.title('EPC $\mathit{a\ priori}$ (%s)' % name)
  mpl.ylabel('HTER')
  mpl.xlabel('$\omega$')
  mpl.legend()

class Analyzer(object):
  """Objects of this type can analyze classifiers and nicely plot results."""

  def __init__(self, classifier, train, devel, test):
    buff = SimplePatternSet()
    classifier.run(self.train['input'], buff)
    train_mse = self.output['train'].mse(self.train['target'])
    classifier.run(self.devel['input'], self.output['devel'])
    devel_mse = self.output['devel'].mse(self.devel['target'])
    classifier.run(self.test['input'], self.output['test'])
    devel_mse = self.output['test'].mse(self.test['target'])

    self.data = {
        'train': {
          'far': 
          }
    self.style  = {
        'train': {
          'color': '0.7', 
          'linestyle': '.-', 
          'linewidth': 1,
          },
        'devel': {
          'color': '0.7', 
          'linestyle': '-', 
          'linewidth': 1,
          },
        'test': {
          'color': 'black', 
          'linestyle': '-', 
          'linewidth': 2,
          },
        }

  def plot_roc(self):
    """"""
