#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 25 Oct 2010 02:54:15 PM CEST 

"""Catapults testing into SGE.

usage: sge-run.py network.xml [+network.xml]...

Assumes the following structure for your working directories containing
"network.xml":

  .. => parent directory contains "train.xml", "devel.xml", "test.xml"
  .  => contains network.xml, place where I should dump train.txt, devel.txt
  and test.txt (results of the running)

network.xml - Networks to test
"""

SCRIPT = """#!/bin/bash
#$ -N %(prog)s
#$ -cwd
source /idiap/home/aanjos/sw/setup.sh
source ${basedir}/neurallab/setup.sh
cd $(dirname $1)
%(prog)s --train=../train.xml --devel=../devel.xml --test=../test.xml --network=network.xml
"""

import os, sys, subprocess, tempfile

def sge_run(network):
  tmp = tempfile.TemporaryFile()
  tmp.write(SCRIPT % {'prog': 'run.py'})
  tmp.seek(0)
  cmd = ['qsub']
  p = subprocess.Popen(cmd, stdin=tmp, stdout=subprocess.PIPE,
      stderr=subprocess.PIPE)
  (stdout, stderr) = p.communicate()
  tmp.close() #also unlinks
  return not p.returncode

if __name__ == '__main__':
  if len(sys.argv) == 1:
    print __doc__
    sys.exit(1)

  for net in sys.argv[1:]:
    netpath = os.path.realpath(net)
    print 'Qsub\'ing: run %s...' % (net),
    if sge_run(k): print 'OK'
    else: print 'Failed'
