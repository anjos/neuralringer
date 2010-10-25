#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 25 Oct 2010 02:54:15 PM CEST 

"""Catapults training into SGE.

usage: sge-train.py N <dir> [+<dir>]

N - Number of times to repeat each training.
"""

SCRIPT = """#!/bin/bash
#$ -N %(prog)s %(params)
#$ -cwd
basedir=/idiap/home/aanjos/work/replay/shaking
source ${basedir}/neurallab/setup.sh
workdir=${basedir}/%(dir)s
cd ${workdir}
now=$(date +'%y.%m.%d-%H:%M:%S')
while [ -d ${workdir}/${now} ]; do
  sleep 1
  now=$(date +'%y.%m.%d-%H:%M:%S')
done
mkdir ${workdir}/${now}
cd ${workdir}/${now}
%(prog)s --train=../train.xml --devel=../devel.xml --test=../test.xml >& log.out
"""

import os, sys, subprocess, tempfile

def sge_train(directory):
  tmp = tempfile.TemporaryFile()
  tmp.write(SCRIPT % {'dir': directory, 'prog': 'rprop-train.py',
    'params': directory.replace('/', '.')}) 
  tmp.seek(0)
  cmd = ['qsub']
  p = subprocess.Popen(cmd, stdin=tmp, stdout=subprocess.PIPE,
      stderr=subprocess.PIPE)
  (stdout, stderr) = p.communicate()
  tmp.close() #also unlinks
  return not p.returncode

if __name__ == '__main__':
  for N in range(int(sys.argv[1])):
    for k in sys.argv[2:]:
      print 'Qsub\'ing: %s, try %d...' % (k, N),
      if sge_train(k): print 'OK'
      else: print 'Failed'
