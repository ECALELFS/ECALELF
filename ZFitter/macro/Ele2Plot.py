import sys, os
import argparse

category = "Et_25-isEle-eleID_loose25nsRun22016Moriond"

parser = argparse.ArgumentParser(description='Standard Data MC plots', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("name", help="outfile base name")
parser.add_argument("--binning", help="(nbins,low,hi)")
parser.add_argument("--file", help="specify data file")
parser.add_argument("-c", "--category", help="category string", default=category)
#specify the branch for electron [0], second axis will replace [0] with [1]
parser.add_argument("-b", "--branch", help="branch to plot (yaxis [0] -> [1])", default="energy_ECAL_ele[0]/cosh(etaSCEle[0])")
parser.add_argument("--isMC", action="store_true", help="isMC for GetTH1()", default=False)
parser.add_argument("--fromFile", action="store_true", help="ignore above options and read TH2 from tmp/name.root", default=False)
parser.add_argument("--nevents", help="number of events in diagonal bins", type=float, default=0)
parser.add_argument("-l","--label", help="axis label", default="")
parser.add_argument("--makeDiagBins", action="store_true", help="draw and calculate the Diagonal bins using nevents", default=False)
parser.add_argument("--noSym", action="store_true", help="fold histogram over x=y", default=False)

args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

#plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

h = None
if args.fromFile:
	f = ROOT.TFile.Open("tmp/" + args.name + ".root")
	if f:
		h = f.Get("hist0")

if not args.fromFile or not h:
	nbins, low, hi = eval(args.binning)
	binning = "({n},{l},{h},{n},{l},{h})".format(n=nbins, l=low, h=hi)

	chain = ROOT.TFile.Open(args.file).Get("selected")
	branchname = args.branch + ":" + args.branch.replace("[0]", "[1]")

	h = plot.GetTH1(chain, branchname, args.isMC, binning, args.category)

	f = ROOT.TFile.Open("tmp/" + args.name + ".root", "RECREATE")
	h.Write()
	f.Close()

if not args.noSym:
	print "folding"
	plot.FoldTH2(h)

plot.Draw2D(h, args.label, args.name, args.makeDiagBins, args.nevents, logy=True, logx=True)

