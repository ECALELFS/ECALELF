import math
import ROOT
from ROOT import *
import sys,os

if len(sys.argv) > 1:
	folder = sys.argv[1]
else:
	folder = "."

file=folder + '/ntuple.root'
fileEleID=folder+'/eleIDTree.root'

tfile = TFile.Open(file)
tree  = tfile.Get("selected")

tfileEleID = TFile.Open(fileEleID)
eleIDTree = tfileEleID.Get("eleIDTree")

tree.Print()
eleIDTree.Print()

tree.SetScanField(0)
eleIDTree.SetScanField(0)

tree.Show(0)
tree.Show(1)
tree.Show(2)
tree.Show(3)
eleIDTree.Show(0)
eleIDTree.Show(1)
eleIDTree.Show(2)
eleIDTree.Show(3)

tree.Scan("*","", "colsize=30", 100)
tree.Scan("slewRateDeltaESeed:etaEle:energySeedSC","slewRateDeltaESeed>0", "")
eleIDTree.Scan("*","","colsize=30",100)
