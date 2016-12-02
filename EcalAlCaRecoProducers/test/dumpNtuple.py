import math
import ROOT
from ROOT import *
import sys,os

if len(sys.argv) > 1:
	folder = sys.argv[1]
else:
	folder = "."

file=folder + '/ntuple_numEvent100.root'

tfile = TFile.Open(file)
tree  = tfile.Get("selected")

tree.SetScanField(0)
tree.Scan("*")
