#!/usr/bin/python2.7

import os
import sys
import signal
import subprocess
import resource

def handler(signum, frame):
   raise Exception("timeout")
signal.signal(signal.SIGALRM, handler)

def write_MPDR_dofile(filename, dofilename):
   dofile = open(dofilename, "w")
   dofile.write("read aig %s\n" % filename)
   dofile.write("set safe 0\n")
   dofile.write("set report -endline\n")
   dofile.write("ver pdr p1\n")
   dofile.write("usage\n")
   dofile.write("q -f\n")
   dofile.close()

def write_SimplePDR_dofile(filename, dofilename):
   dofile = open(dofilename, "w")
   dofile.write("read aig %s\n" % filename)
   dofile.write("satv pdr -o 0\n")
   dofile.write("usage\n")
   dofile.write("q -f\n")
   dofile.close()

if len(sys.argv) != 3:
   print "Usage: %s <case> <out>" % sys.argv[0]
   exit(0)

v3_path = "../../pdrv"
timelimit = 900
casefilename = sys.argv[1]
outfilename = sys.argv[2]
dofilename = "run.dofile"

# set memory limit 16GB
memlimit = 16*1024*1024*1024
_, hard = resource.getrlimit(resource.RLIMIT_AS)
resource.setrlimit(resource.RLIMIT_AS, (memlimit, hard))

cases = open(casefilename, "r")
if os.path.isfile(outfilename):
   os.system("rm %s" % outfilename)
for filename in cases:
   filename = filename.strip()
   print "running %s" % filename
   # write_MPDR_dofile(filename, dofilename)
   write_SimplePDR_dofile(filename, dofilename)
   outfile = open(outfilename, "a")
   p = subprocess.Popen([v3_path, "-f", dofilename], stdout = outfile, stderr = outfile)
   signal.alarm(timelimit)
   try:
      ret = p.wait()
      if (ret != 0): outfile.write("\nMEMERROR\n")
      signal.alarm(0)
   except Exception, msg:
      p.kill()
      outfile.write("\nTIMEOUT\n")
      print msg
   outfile.close()

cases.close()

