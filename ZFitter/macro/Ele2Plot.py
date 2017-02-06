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

if not args.fromFile:
	nbins, low, hi = eval(args.binning)
	binning = "({n},{l},{h},{n},{l},{h})".format(n=nbins, l=low, h=hi)

	chain = ROOT.TFile.Open(args.file).Get("selected")
	branchname = args.branch + ":" + args.branch.replace("[0]", "[1]")

	h = plot.GetTH1(chain, branchname, args.isMC, binning, args.category)

	f = ROOT.TFile.Open("tmp/" + args.name + ".root", "RECREATE")
	h.Write()
	f.Close()

f = ROOT.TFile.Open("tmp/" + args.name + ".root")
h = f.Get("hist0")

if not args.noSym:
	print "folding"
	plot.FoldTH2(h)

c = ROOT.TCanvas("c","c", 800, 500)

c.SetRightMargin(.3)

h.SetStats()
ROOT.gStyle.SetOptStat(1111111)
h.SetYTitle("Ele. 1 " + args.label)
h.SetXTitle("Ele. 2 " + args.label)
#h.SetMaximum(2000)
h.Draw("colz")

print "###", args.name, h.Integral(0, h.GetNbinsX() + 1, 0, h.GetNbinsY()+1), h.GetEntries()

c.Modified()
c.Update()
ps = h.GetListOfFunctions().FindObject("stats")
ps.SetX1NDC(0.8);
c.Modified()
c.Update()

if args.makeDiagBins:
	xbins = h.GetXaxis().GetNbins()
	ybins = h.GetYaxis().GetNbins()

	i = 1
	bins = [i]
	values = [h.GetXaxis().GetBinLowEdge(i)]
	while i <= xbins and i <= ybins:
		integral = h.Integral(bins[-1], i, bins[-1], i)
		if integral > args.nevents:
			bins.append(i)
			values.append(h.GetXaxis().GetBinLowEdge(i))
		i += 1
	bins.append(i)
	values.append(h.GetXaxis().GetBinLowEdge(i))

	print args.label + " bins"
	for low,hi in zip(values[:-1], values[1:]):
		print "%10.3f < %s < %10.3f" % (low, args.label, hi)
	print "[RESULT]", values

	boxes = []
	for i in xrange(len(values)-1):
		boxes.append(ROOT.TBox( values[i], values[i], values[i+1], values[i+1]))

	for b in boxes:
		b.SetFillStyle(0)
		b.SetLineColor(ROOT.kRed)
		b.Draw()

c.SaveAs("plots/" + args.name + ".png")
c.SaveAs("plots/" + args.name + ".pdf")
c.SaveAs("plots/" + args.name + ".eps")


