#!/usr/bin/env python

from stability  import ParseTable as pt
from shutil     import copyfile
from optparse   import OptionParser
from pprint     import pprint

import os, re, sys, getopt

def get_options():
    parser = OptionParser()
    parser.add_option("-r", "--run-range",
                      dest="run_range", default='',
                      help=" run ranges ")
    parser.add_option("-d", "--datasetpath",
                      dest="datasetpath", default='',
                      help=" dataset path ")
    parser.add_option("-n", "--datasetname",
                      dest="datasetname", default='',
                      help="dataset name")
    parser.add_option("--store",
                      dest="store", default='',
                      help="storage directory")
    parser.add_option("--remote_dir",
                      dest="remote_dir", default='',
                      help="remotre directory of the origin files")
                    
    
    
def 
    
