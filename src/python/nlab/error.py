#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Thu 21 Oct 2010 09:14:17 AM CEST 

"""Calculates error curves and the such.
"""

__all__ = ['Observer', 'Analyzer']

import os, sys, math, tempfile

class Observer(object):
  """Objects of this type observe what happens to a classifier while its being
  trained. It can provide clues on which states are worth saving or when to
  stop training a classifier."""

  def __init__(self, classifier, train, devel, test, reporter):
    def prepare_buffers(database):
      retval = {}
      retval['database'] = database
      retval['input'] = database.cat()
      retval['target'] = database.cat_target()
      retval['output'] = database.cat_target()

    self.data = {}
    self.data['train'] = prepare_buffers(train)
    self.data['devel'] = prepare_buffers(devel)
    self.data['test'] = prepare_buffers(test)
    self.classifier = classifier
    self.best_classifier_seen = (0, classifier.clone(reporter))
    self.reporter = reporter
    self.mse = []

  def rundb(self, classifier, dbname):
    classifier.run(self.data[dbname]['input'], self.data[dbname]['output'])

  def evaluate(self):
    """Evalulates the current classifier performance"""
    
    def eval_once(classifier, dbname): 
      self.rundb(classifier, dbname)
      return self.data[dbname]['output'].mse(self.data[dbname]['target'])

    result = {}
    for k, v in self.data.iteritems(): result[k] = eval_once(self.classifier, v)
    
    if result['devel'] < min([k['devel'] for k in self.mse]): #save best state
      self.best_classifier_seen = (len(self.mse), 
          self.classifier.clone(self.reporter))
    
    self.mse.append(result)

  def _stalled(self):
    """Determines how many cycles w/o improvement have been observed."""
    return len(self.mse) - self.best_classifier_seen[0]
  
  stalled = property(_stalled)

  def save_best(self, filename):
    self.best_classifier_seen[1].save(filename)

def far(noise, threshold):
  """Calculates the FAR given a certain threshold."""
  return 100.0*sum([int(k>=threshold) for k in noise])/float(len(noise))

def frr(signal, threshold):
  """Calculates the FRR given a certain threshold."""
  return 100.0*sum([int(k<threshold) for k in signal])/float(len(signal))

def plot_roc(data, name):
  import matplotlib.pyplot as mpl

  for k in data.iterkeys(): 
    mpl.plot(data[k]['far'], data[k]['frr'], **Analyzer.STYLE[k])
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.xlabel('FAR (%)')
  mpl.ylabel('FRR (%)')
  mpl.legend()

def plot_det(data, name):
  import matplotlib.pyplot as mpl

  for k in data.iterkeys(): 
    mpl.loglog(data[k]['far'], data[k]['frr'], **Analyzer.STYLE[k])
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
        ((1-omega) * data['devel']['frr'][i])
    if wer <= minwer:
      minwer = wer
      minhter = (data['test'][i] + data['test'][i])/2
  return minhter

def plot_epc(data, name):
  import matplotlib.pyplot as mpl

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

def plot_mse(data, name):
  import matplotlib.pyplot as mpl

  for k in data.keys():
    mpl.plot(range(len(data[k]['mse']), data[k]['mse'], **Analyzer.STYLE[k]))
  mpl.grid(True)
  mpl.Title('MSE evolution')
  mpl.ylabel('Mean Square Error')
  mpl.xlable('Training steps')
  mpl.legend()

class Analyzer(object):
  """Objects of this type can analyze classifiers and nicely plot results."""

  STYLE  = {
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

  def __init__(self, observer):
    self.observer = observer
    self.signal_class = signal
    self.noise_class = noise

    #gets the MSE
    self.data = {'train': {}, 'devel': {}, 'test': {},}
    for k in self.data.keys():
      self.data[k]['mse'] = [i[k] for i in self.observer.mse]

    #estimates the FAR and FFR on all sets
    if len(self.observer.evaluator.train.target.values()) != 2:
      raise RuntimeError, "Can only make plots if we have only 2 targets"

    #WARNING: -1 => noise, +1 => signal
    points = 20
    thresholds = [k/float(points) for k in range(-1*points+1, points)]
    for k in self.data.keys():
      self.data[k]['far'] = []
      self.data[k]['frr'] = []
      self.data[k]['thresholds'] = thresholds
      self.observer.rundb(self.observer.best_classifier_seen[1], k)
      output = self.observer.data[k]['output']
      target = self.observer.data[k]['target']
      noise = [output[j] for j in range(len(output)) if target[j] < 0]
      signal = [output[j] for j in range(len(output)) if target[j] > 0]
      for t in thresholds: 
        self.data[k]['far'].append(far(noise, t))
        self.data[k]['frr'].append(frr(signal, t))

  def pdf_all(self, filename, hint=''):
    """Records a full run analyzis to PDF"""
    from matplotlib.backends.backend_pdf import PdfPages
    pp = PdfPages(filename)
    for k in (plot_mse, plot_roc, plot_det, plot_epc):
      fig = mpl.figure()
      k(data, hint)
      pp.savefig(fig)
    pp.close()

  def error(self):
    """Calculates the min HTER and EER and print thresholds."""
    pass 
