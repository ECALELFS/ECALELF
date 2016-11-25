import math
import ROOT
from ROOT import *
import sys,os

file='ntuple_numEvent100.root'

tfile = TFile.Open(file)
tree  = tfile.Get("selected")

tree.SetScanField(0)
tree.Scan("*")
