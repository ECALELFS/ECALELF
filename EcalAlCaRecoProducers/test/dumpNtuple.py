import math
import ROOT
from ROOT import *
import sys,os

if len(sys.argv) > 1:
	folder = sys.argv[1]
else:
	folder = "."

file=folder + '/ntuple.root'

tfile = TFile.Open(file)
tree  = tfile.Get("selected")

tree.Print()
tree.SetScanField(0)

tree.Show(0)
tree.Show(1)
tree.Show(2)
tree.Show(3)
tree.Show(4)
tree.Show(5)
tree.Show(6)
tree.Show(7)
tree.Show(8)

tree.Scan("*","", "", 100)
tree.Scan("slewRateDeltaESeed:etaEle:energySeedSC","slewRateDeltaESeed>0", "", 100)

