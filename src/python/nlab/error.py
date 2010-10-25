#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Thu 21 Oct 2010 09:14:17 AM CEST 

"""Calculates error curves and the such.
"""

__all__ = ['Observer', 'Analyzer']

import os, sys, tempfile

ERROR_TABLE = """\
          | FAR (%%) | FRR (%%) |  HTER  | Threshold (**)
----------+---------+---------+--------+----------------
Min. HTER | %5s   |  %5s  | %5s  |  %5s
EER       | %5s   |  %5s  | %5s  |  %5s

(**) Test set results, with thresholds chosen a priori on the development set\
"""

class Observer(object):
  """Objects of this type observe what happens to a classifier while its being
  trained. It can provide clues on which states are worth saving or when to
  stop training a classifier."""

  def __init__(self, classifier, train, devel, test):
    def prepare_buffers(database):
      retval = {}
      retval['database'] = database
      retval['input'] = database.cat()
      retval['target'] = database.cat_target()
      retval['output'] = database.cat_target()
      return retval

    self.data = {}
    self.data['train'] = prepare_buffers(train)
    self.data['devel'] = prepare_buffers(devel)
    self.data['test'] = prepare_buffers(test)
    self.classifier = classifier
    self.mse = []
    self.evaluate(0)

  def rundb(self, classifier, dbname):
    classifier.run(self.data[dbname]['input'], self.data[dbname]['output'])

  def evaluate(self, step):
    """Evalulates the current classifier performance"""
    
    def eval_once(classifier, dbname): 
      self.rundb(classifier, dbname)
      return self.data[dbname]['output'].mse(self.data[dbname]['target'])

    result = {'step': step}
    for k in self.data.iterkeys(): result[k] = eval_once(self.classifier, k)
     
    if not len(self.mse) or \
        result['devel'] < self.lowest_mse[1]: #save best state
      #print '[%d] Saving current network, MSE = %.4e < %.4e' % \
      #  (step, result['devel'], self.lowest_mse[1])
      self.lowest_mse = (step, result['devel'])
      self.best_classifier_seen = (step, self.classifier.clone())
    
    self.mse.append(result)

  def statistics(self, step):
    return '[%d] devel: %.4e (stall: %d); train: %.4e; test: %.4e' % \
        (step, self.mse[-1]['devel'], self.stalled(step),
            self.mse[-1]['train'], self.mse[-1]['test'])

  def stalled(self, step):
    """Determines how many cycles w/o improvement have been observed."""
    return step - self.best_classifier_seen[0]
  
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
    mpl.plot(data[k]['far'], data[k]['frr'], label=k, **Analyzer.STYLE[k])
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.xlabel('FAR (%)')
  mpl.ylabel('FRR (%)')
  mpl.legend()

def plot_det(data, name):
  import matplotlib.pyplot as mpl

  for k in data.iterkeys(): 
    mpl.loglog(data[k]['far'], data[k]['frr'], label=k, **Analyzer.STYLE[k])
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.xlabel('FAR (%)')
  mpl.ylabel('FRR (%)')
  mpl.legend()

def hter_min_wer(data, omega):
  minwer = 100
  minhter = 100
  minthres = -1.0
  for i, k in enumerate(data['devel']['threshold']):
    wer = (omega * data['devel']['far'][i]) + \
        ((1-omega) * data['devel']['frr'][i])
    if wer <= minwer:
      minwer = wer
      minhter = (data['test']['far'][i] + data['test']['frr'][i])/2
      minthres = k
  return minhter, minthres

def plot_epc(data, name):
  import matplotlib.pyplot as mpl

  N = len(data['test']['far'])
  omega = [k/float(N-1) for k in range(N)]
  test_hter = []
  for k in omega:
    test_hter.append(hter_min_wer(data, k)[0])
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
    mpl.plot([j[0] for j in data[k]['mse']], [j[1] for j in data[k]['mse']], 
        label=k, **Analyzer.STYLE[k])
  mpl.grid(True)
  mpl.title('MSE evolution (%s)' % name)
  mpl.ylabel('Mean Square Error')
  mpl.xlabel('Training steps')
  mpl.legend()

def plot_output(data, name):
  import matplotlib.pyplot as mpl
  plot = 311
  has_title = False
  for k in data.keys():
    mpl.subplot(plot)
    mpl.hist((data[k]['signal'], data[k]['noise']), bins=10, label=('Real Access',
      'Attack'), alpha=0.5, range=(-1, +1))
    if not has_title: 
      mpl.title('Network Output (%s)' % name)
      has_title = True
    mpl.grid(True)
    mpl.xlabel('%s' % k.capitalize())
    mpl.legend()
    plot += 1

class Analyzer(object):
  """Objects of this type can analyze classifiers and nicely plot results."""

  STYLE  = {
      'train': {
        'color': '0.7', 
        'linestyle': '--', 
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

    #gets the MSE
    self.data = {'train': {}, 'devel': {}, 'test': {},}
    for k in self.data.keys():
      self.data[k]['mse'] = [(i['step'], i[k]) for i in self.observer.mse]

    #WARNING: -1 => noise, +1 => signal
    points = 20
    threshold = [k/float(points) for k in range(-1*points+1, points)]
    for k in self.data.keys():
      self.data[k]['far'] = []
      self.data[k]['frr'] = []
      self.data[k]['threshold'] = threshold
      self.observer.rundb(self.observer.best_classifier_seen[1], k)
      self.data[k]['output'] = [self.observer.data[k]['output'][i][0] for i in range(len(self.observer.data[k]['output']))]
      self.data[k]['target'] = [self.observer.data[k]['target'][i][0] for i in range(len(self.observer.data[k]['target']))]
      self.data[k]['noise'] = [self.data[k]['output'][j] for j in range(len(self.data[k]['output'])) if self.data[k]['target'][j] < 0]
      self.data[k]['signal'] = [self.data[k]['output'][j] for j in range(len(self.data[k]['output'])) if self.data[k]['target'][j] > 0]
      for t in threshold: 
        self.data[k]['far'].append(far(self.data[k]['noise'], t))
        self.data[k]['frr'].append(frr(self.data[k]['signal'], t))

  def pdf_all(self, filename, hint=''):
    """Records a full run analyzis to PDF"""
    import matplotlib.pyplot as mpl
    from matplotlib.backends.backend_pdf import PdfPages

    pp = PdfPages(filename)
    for k in (plot_mse, plot_output, plot_roc, plot_det, plot_epc):
      fig = mpl.figure()
      k(self.data, hint)
      pp.savefig(fig)
    pp.close()

  def error(self):
    """Calculates the min HTER and EER and print thresholds."""
    hter, threshold = hter_min_wer(self.data, 0.5)
    hter_far = far(self.data['test']['noise'], threshold)
    hter_frr = frr(self.data['test']['signal'], threshold)
    strings = ['%.1f' % k for k in (hter_far, hter_frr, (hter_far+hter_frr)/2)]
    strings.append('%1.2f' % threshold)

    #EER calculation
    minabs = 2
    eer_threshold = 0
    for i, k in enumerate(self.data['devel']['threshold']):
      absdiff = abs(self.data['devel']['far'][i] - self.data['devel']['frr'][i])
      if absdiff < minabs:
        minabs = absdiff
        eer_threshold = k
    eer_far = far(self.data['test']['noise'], eer_threshold)
    eer_frr = frr(self.data['test']['signal'], eer_threshold)
    strings += ['%.1f' % k for k in (eer_far, eer_frr, (eer_far+eer_frr)/2)]
    strings.append('%1.2f' % eer_threshold)
    return ERROR_TABLE % tuple(strings)
