#!/usr/bin/env python3
"""
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
"""

from __future__ import (absolute_import, division,
                        print_function, unicode_literals)
import gzip
import argparse
import math
import numpy
import os
import subprocess
import sys
import tempfile
import gridstats

def main(args):
  TOLERANCE = 1e-6
  # read file
  filename = args.infile
  opener = gzip.open if filename.endswith('.gz') else open
  with opener(filename, 'rb') as fd:
    lines = fd.readlines()

  # start & end
  s = math.inf
  e = 0
  for line in lines:
    line = line.decode('utf-8').strip()
    if line.find('F') != -1:
      cols = line.split(',')
      send_time = int(cols[2])
      recv_time = int(cols[2])
      if args.scalar:
        send_time *= args.scalar
        recv_time *= args.scalar
      if send_time < s:
        s = send_time
      if recv_time > e:
        e = recv_time
  if args.mintime and args.maxtime:
    assert args.mintime <= args.maxtime, "start must be <= end"
  if args.mintime:
    assert args.mintime < e + TOLERANCE and args.mintime >= s - TOLERANCE,\
    ("Start [" + str(args.mintime) + "] not in sim range: "\
     + str(s) + "-" + str(e))
    s = args.mintime
  if args.maxtime:
    assert args.maxtime > s - TOLERANCE and args.maxtime <= e + TOLERANCE,\
    ("End [" + str(args.maxtime) + "] not in sim range: "\
     + str(s) + "-" + str(e))
    e = args.maxtime
  if s == math.inf or e == 0 :
    outfile = args.outfile
    opener = gzip.open if outfile.endswith('.gz') else open
    open(outfile, 'w').close()
    exit(0)

  # create buckets
  numBins = args.buckets
  binWidth = (e - s) / numBins
  binTimesOrg = numpy.linspace(s, e - binWidth, numBins)
  binTimes = numpy.append(binTimesOrg, e)

  print("Running ssparse for range {0}-{1} with {2} bins"
        .format(s, e, numBins))
  # call bin -> tmp file
  hopFiles = []
  latFiles = []
  error = None
  for idx in range(numBins):
    b_start = binTimes[idx]
    b_end = binTimes[idx+1]

    hopfile_fd, hopfile_name = tempfile.mkstemp(prefix='trans_')
    latfile_fd, latfile_name = tempfile.mkstemp(prefix='trans_')
    os.close(hopfile_fd)
    os.close(latfile_fd)
    hopFiles.append(hopfile_name)
    latFiles.append(latfile_name)

    # ssparse cmd
    cmd = ('{0} -c {1} -l {2} -f +{3}={4}-{5} {6}'
           .format(args.ssparse,
                   hopfile_name,
                   latfile_name,
                   args.time, b_start, b_end,
                   args.infile))
    if args.scalar:
      cmd += (' -s {0}'.format(args.scalar))

    if args.filters:
      for filter in args.filters:
        cmd += ' -f {0}'.format(filter)

    try:
      subprocess.run(cmd, check=True, shell=True)
    except subprocess.CalledProcessError as e:
      error = True
    if error:
      break

  if error:
    assert(len(hopFiles) == len(latFiles))
    for hopfile, latfile in zip(hopFiles,latFiles):
      os.remove(hopfile)
      os.remove(latfile)
    exit(-1)

  # headers
  uColsLat = []
  gridsLat = []
  uColsHop = []
  gridsHop = []
  assert(len(hopFiles) == len(latFiles))
  for hopfile, latfile in zip(hopFiles,latFiles):
    gLat = gridstats.GridStats()
    gLat.read(latfile)
    gridsLat.append(gLat)
    currColsLat = gLat.column_names()

    gHop = gridstats.GridStats()
    gHop.read(hopfile)
    gridsHop.append(gHop)
    currColsHop = gHop.column_names()

    for x in currColsLat:
      if x not in uColsLat:
        uColsLat.append(x)

    for x in currColsHop:
      if x not in uColsHop:
        uColsHop.append(x)

  # sort hops
  PerHops = []
  PerMinHops = []
  PerNonMinHops = []
  for x in uColsHop:
    if "PerHops" in x:
      PerHops.append(x)
    if "PerMinHops" in x:
      PerMinHops.append(x)
    if "PerNonMinHops" in x:
      PerNonMinHops.append(x)

  sColsHop = ['AveHops']
  sColsHop.extend(PerHops)
  sColsHop.append('AveMinHops')
  sColsHop.append('PerMinimal')
  sColsHop.extend(PerMinHops)
  sColsHop.append('AveNonMinHops')
  sColsHop.append('PerNonMinimal')
  sColsHop.extend(PerNonMinHops)

  # delete tmp files
  for hopfile, latfile in zip(hopFiles,latFiles):
    os.remove(hopfile)
    os.remove(latfile)

  # write out.csv
  fGrid = gridstats.GridStats()
  fGrid.create('Time', binTimesOrg, sColsHop + uColsLat)

  assert len(gridsLat) == len(binTimesOrg) and len(gridsHop) == len(binTimesOrg)
  for gridLat, gridHop, binTime in zip(gridsLat, gridsHop, binTimesOrg):
    for colHop in uColsHop:
      value = gridHop.get('Packet', colHop, default='nan')
      fGrid.set(binTime, colHop, value)
    for colLat in uColsLat:
      value = gridLat.get('Packet', colLat, default='nan')
      fGrid.set(binTime, colLat, value)
  fGrid.write(args.outfile)


if __name__ == '__main__':
  ap = argparse.ArgumentParser()
  ap.add_argument('ssparse',
                  help='ssparse executable')
  ap.add_argument('infile',
                  help='input message log')
  ap.add_argument('outfile',
                  help='output csv file')

  ap.add_argument('-b', '--buckets', type=int, default=40,
                  help='number of time buckets')
  ap.add_argument('-m', '--mintime', type=float,
                  help='min send time')
  ap.add_argument('-n', '--maxtime',type=float,
                  help='max send time')
  ap.add_argument('-s', '--scalar', type=float,
                  help='time scalar')
  ap.add_argument('-t', '--time', type=str, default='send',
                  choices=['start', 'send', 'end', 'recv'],
                  help='type of time filter to use')
  ap.add_argument('-f', '--filters', action='append', type=str,
                  help='filters to be used (can\'t be time based)')
  args = ap.parse_args()

  if args.filters:
    for filter in args.filters:
      for banned in ['start', 'send', 'end', 'recv']:
        if filter[1:].startswith(banned):
          sys.exit("Filters can\'t be time based, "\
                   "i.e. 'start', 'send', 'end', 'recv'")

  main(args)
