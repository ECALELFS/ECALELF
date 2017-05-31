import sys, os
import argparse

commonCut = "Et_25-isEle-eleID_loose25nsRun22016Moriond"

parser = argparse.ArgumentParser(description='Standard Data MC plots', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("name", help="outfile base name")
parser.add_argument("--binning", help="(nbins,low,hi)")
parser.add_argument("--file", help="specify chain file")
parser.add_argument("-c", "--category", help="special category", default="")
parser.add_argument("--commonCut", help="commonCut", default=commonCut)
#specify the branch for electron [0], second axis will replace [0] with [1]
parser.add_argument("-b", "--branch", help="branch to plot (yaxis [0] -> [1])", default="energy_ECAL_ele[0]/cosh(etaSCEle[0])")
parser.add_argument("--isMC", action="store_true", help="isMC for GetTH1()", default=False)
parser.add_argument("--fromFile", action="store_true", help="ignore above options and read TH2 from tmp/name.root", default=False)
parser.add_argument("--nevents", help="number of events in diagonal bins", type=float, default=0)
parser.add_argument("-l","--label", help="axis label", default="")
parser.add_argument("--makeDiagBins", action="store_true", help="draw and calculate the Diagonal bins using nevents", default=False)
parser.add_argument("--noSym", action="store_true", help="fold histogram over x=y", default=False)
parser.add_argument("--noWeights", action="store_true", help="don't apply anyweights", default=False)

args = parser.parse_args()

category = args.commonCut
if args.category:
	category += "-" + args.category

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
	if not args.noSym:
		branchpair = args.branch + "," + args.branch.replace("[0]", "[1]") 
		branchname = "max({0}):min({0})".format(branchpair)
	else:
		branchname = "{0}:{1}".format(args.branch, args.branch.replace("[0]", "[1]") )

	h = plot.GetTH1(chain, branchname, args.isMC, binning, category, noWeights=args.noWeights)

	f = ROOT.TFile.Open("tmp/" + args.name + ".root", "RECREATE")
	h.Write()
	f.Close()

if args.makeDiagBins:
	values, integrals = plot.makeDiagBins(h, args.nevents)
	with open("data/regions/" +args.name + ".dat","w") as datfile:
		for low,hi,integral in zip(values[:-1], values[1:], integrals):
			datfile.write(args.category + "-ET_%.2f_%.2f # nevents %d\n" % (low, hi, integral))
else:
	values = None
plot.Draw2D(h, args.label, args.name, diagBins = values, logy=True, logx=True, logz=True)
