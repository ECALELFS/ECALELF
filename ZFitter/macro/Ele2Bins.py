import sys
import ROOT

import argparse

parser = argparse.ArgumentParser(description='Make 2D plot for deciding diagonal categories', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("file", help="specify data file")
parser.add_argument("nevents", help="number of events in diagonal bins", type=float)
parser.add_argument("-l","--label", help="access label", default="")

args = parser.parse_args()

f = ROOT.TFile.Open(args.file)
h = f.Get("hist0")

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

c = ROOT.TCanvas("c","c")

h.SetStats()
ROOT.gStyle.SetOptStat(1111111)
h.Draw("colz")
h.SetXTitle("Ele. 1 " + args.label)
h.SetYTitle("Ele. 2 " + args.label)

boxes = []
for i in xrange(len(values)-1):
	boxes.append(ROOT.TBox( values[i], values[i], values[i+1], values[i+1]))

for b in boxes:
	b.SetFillStyle(0)
	b.SetLineColor(ROOT.kRed)
	b.Draw()

c.SaveAs("plots/ET_diag.png")
