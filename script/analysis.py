#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Tue 24 Aug 2010 08:49:23 CEST 

"""Analyzes neuralringer output using matplotlib instead of R.
"""

import os, sys, math
import matplotlib.pyplot as mpl
from matplotlib.backends.backend_pdf import PdfPages

def load_values(f):
  """Loads values from the last column of a file"""
  t = open(f, 'rt')
  vals = [float(k.split()[4].strip()) for k in t]
  t.close()
  return vals
  
def load_mse(f):
  """Loads the MSE network output."""
  t = open(f, 'rt')
  lines = [k.split() for k in t]
  t.close()
  epoch = [int(k[0]) for k in lines[1:]]
  test = [float(k[1]) for k in lines[1:]]
  train = [float(k[2]) for k in lines[1:]]
  return (epoch, test, train)

def load_relevance(f):
  """Loads the relevance of each feature."""
  t = open(f, 'rt')
  lines = [k.split() for k in t]
  t.close()
  test = [float(k[1]) for k in lines[1:]]
  train = [float(k[2]) for k in lines[1:]]
  return test, train

def load_outputs(prefix):
  """Loads the outputs of a certain run. Returns a 2-tuple with attacks and
  real client output.
  
  Please note the following arbitrary order: -1 => attack; +1 => real client
  """
  outputs = load_values(prefix + '-output.txt')
  targets = load_values(prefix + '-target.txt')
  attack = []
  real = []
  for i, k in enumerate(targets):
    if k < 0: attack.append(outputs[i])
    else: real.append(outputs[i])
  return (attack, real)

def far(attack, threshold):
  """Calculates the FAR given a certain threshold."""
  return 100.0*sum([int(k>=threshold) for k in attack])/float(len(attack))

def ffr(real, threshold):
  """Calculates the FAR given a certain threshold."""
  return 100.0*sum([int(k<threshold) for k in real])/float(len(real))

def load_ratios(prefix, N=200):
  """Plots the ROC curve for the targets and outputs of train and test data."""
  train_attack, train_real = load_outputs(prefix + '.out.train')
  test_attack, test_real = load_outputs(prefix + '.out.test')
  test_far = []
  test_ffr = []
  train_far = []
  train_ffr = []
  for i in [2.0*k/N for k in range(-1*(N/2), (N/2) + 1)]:
    test_far.append(far(test_attack, i))
    test_ffr.append(ffr(test_real, i))
    train_far.append(far(train_attack, i))
    train_ffr.append(ffr(train_real, i))
  return test_far, test_ffr, train_far, train_ffr

def plot_det(test_far, test_ffr, train_far, train_ffr, name):
  mpl.loglog(test_far, test_ffr, color='black', linestyle='-', label='test', 
           linewidth=2) 
  mpl.loglog(train_far, train_ffr, color='0.7', linestyle='-.', label='train')
  mpl.grid(True)
  mpl.title('DET curve (%s)' % name)
  mpl.ylabel('FR (%)')
  mpl.xlabel('FA (%)')
  mpl.legend()

def plot_roc(test_far, test_ffr, train_far, train_ffr, name):
  mpl.plot(test_far, test_ffr, color='black', linestyle='-', label='test', 
           linewidth=2) 
  mpl.plot(train_far, train_ffr, color='0.7', linestyle='-.', label='train')
  mpl.grid(True)
  mpl.title('ROC curve (%s)' % name)
  mpl.ylabel('FR (%)')
  mpl.xlabel('FA (%)')
  mpl.legend()

def hter_min_wer(test_far, test_ffr, train_far, train_ffr, omega):
  N = len(test_far) - 1
  threshold = [2.0*k/N for k in range(-1*(N/2), (N/2) + 1)]
  minwer = 100
  minhter = 100
  for i, k in enumerate(threshold):
    wer = (omega * train_far[i]) + ((1-omega) * train_ffr[i])
    if wer <= minwer: 
      minwer = wer
      minhter = (test_far[i] + test_ffr[i])/2
  return minhter

def plot_epc(test_far, test_ffr, train_far, train_ffr, name):
  N = len(test_far)
  omega = [k/float(N-1) for k in range(N)]
  test_hter = []
  for k in omega:
    test_hter.append(hter_min_wer(test_far, test_ffr, train_far, train_ffr, k))
  mpl.plot(omega, test_hter, color='black', linestyle='-', label='test', 
           linewidth=2) 
  mpl.grid(True)
  mpl.title('EPC $\mathit{a\ priori}$ (%s)' % name)
  mpl.ylabel('HTER')
  mpl.xlabel('$\omega$')
  mpl.legend()

def plot_relevance(prefix, name):
  test, train = load_relevance(prefix + '.end.relevance.txt')
  labels = ['mean', 'variance', 'min', 'max', 'd.c. ratio']
  xlocations = [k-0.4 for k in range(len(test))]
  mpl.bar(xlocations, test, color='black', label='test')
  mpl.bar(xlocations, train, bottom=test, color='0.7', label='train')
  mpl.xticks([k + 0.4 for k in xlocations], labels)
  mpl.title('Relevance (%s)' % name)
  mpl.legend()
  mpl.grid(True)

def plot_mse(prefix, name):
  epoch, test, train = load_mse(prefix + '.mse.txt')
  
  mpl.plot(epoch, test, color='black', linestyle='-', label='test', linewidth=2)
  mpl.plot(epoch, train, color='0.7', linestyle='-.', label='train')
  mpl.grid(True)
  mpl.title('MSE evolution (%s)' % name)
  mpl.ylabel('MSE')
  mpl.xlabel('Epochs')
  mpl.legend()
  mpl.grid(True)

if __name__ == '__main__':
  if len(sys.argv) != 2:
    print 'usage: %s <testdir/prefix>' % os.path.basename(sys.argv[0])
    sys.exit(1)
 
  dir = os.path.realpath(sys.argv[1])
  name = os.path.basename(os.path.dirname(sys.argv[1])).replace('_', ' - ')
  test_far, test_ffr, train_far, train_ffr = load_ratios(sys.argv[1])
  pp = PdfPages(os.path.join(os.path.dirname(sys.argv[1]), 'ratios.pdf'))
  fig0 = mpl.figure()
  plot_roc(test_far, test_ffr, train_far, train_ffr, name)
  pp.savefig(fig0)
  fig1 = mpl.figure()
  plot_det(test_far, test_ffr, train_far, train_ffr, name)
  pp.savefig(fig1)
  fig2 = mpl.figure()
  plot_epc(test_far, test_ffr, train_far, train_ffr, name)
  pp.savefig(fig2)
  fig3 = mpl.figure()
  plot_relevance(sys.argv[1], name)
  pp.savefig(fig3)
  fig4 = mpl.figure()
  plot_mse(sys.argv[1], name)
  pp.savefig(fig4)
  pp.close()
  sys.exit(0)
