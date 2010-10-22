#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Fri 22 Oct 2010 11:46:12 AM CEST 

"""This module contains normalization operators for databases and pattern sets.
"""

def standard(db):
  """Calculates the Mean and Variance of a Database taking into consideration
  the number of classes available and considering them equally. I.e., classes
  with more feature sets will not contribute more to the mean or variance
  calculation. 

  M = 1/N * sum(Mc), with Mc = Mean of class1, class2, ... classN

  SD = 1/N * sqrt(sum(Vc)), with Vc = Variance of class1, class2, ... classN
  
  """

  def mean(v): 
    sum(v)/float(len(v))
  def var(v):
    m = mean(v)
    (1.0/(len(v)-1)) * sum([(k-m)**2.0 for k in v])

  #calculates the intra-class averages
  averages = []
  for k, features in db.data.iteritems():
    s = []
    for i in range(db.pattern_size()): s.append(mean([f[i] for f in features]))
    averages.append(s)

  #calculates the overall average
  overall_average = []
  for k in range(db.pattern_size()): 
    overall_average.append(mean([i[k] for i in averages]))
  overall_average = tuple(overall_average)

  #now we compute the overall deviation from the mean
  variances = {} 
  for k, features in db.data.iteritems():
    s = []
    for i in range(db.pattern_size()): s.append(var([f[i] for f in features]))
    variances[k] = s

  #calculates the overall variance
  overall_variance = []
  factor = 1.0/(db.size()**2.0)
  for k in range(db.pattern_size()): 
    overall_variance.append(factor*sum([i[k] for i in variances]))
  overall_stddev = tuple([k**0.5 for k in overall_variance])

  return (overall_mean, overall_stddev)
