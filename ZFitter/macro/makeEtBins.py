import sys, os
import argparse

category = "Et_25-isEle-eleID_loose25nsRun22016Moriond"
common_cut = "eleID_loose25nsRun22016Moriond-EtLeading_32-EtSubLeading_20-noPF-isEle"

parser = argparse.ArgumentParser(description='make Et Bins', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("name", help="outfile base name")
#parser.add_argument("--binning", help="(nbins,low,hi)")
#parser.add_argument("--file", help="specify data file")
#parser.add_argument("-c", "--category", help="category string", default=category)
##specify the branch for electron [0], second axis will replace [0] with [1]
#parser.add_argument("-b", "--branch", help="branch to plot (yaxis [0] -> [1])", default="")
#parser.add_argument("--isMC", action="store_true", help="isMC for GetTH1()", default=False)
#parser.add_argument("--fromFile", action="store_true", help="ignore above options and read TH2 from tmp/name.root", default=False)
#parser.add_argument("--nevents", help="number of events in diagonal bins", type=float, default=0)
#parser.add_argument("-l","--label", help="axis label", default="")
#parser.add_argument("--makeDiagBins", action="store_true", help="draw and calculate the Diagonal bins using nevents", default=False)
#parser.add_argument("--noSym", action="store_true", help="fold histogram over x=y", default=False)

#args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

#plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

regions = [region for region in open("data/regions/eta_r9.dat").read().split("\n") if region]
regions += ["ALL"]

outfile = "tmp/EtBins.root"
f = ROOT.TFile.Open(outfile,"READ")
if not f:
	data_c = ROOT.TFile.Open("tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned/d_chain.root").Get("selected")
	inc_c  = ROOT.TFile.Open("tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned/s1_chain.root").Get("selected")
	lt_c   = ROOT.TFile.Open("tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned/s2_chain.root").Get("selected")

	binwidth = 1
	low = 20
	hi = 500
	nbins = (hi - low) / binwidth
	binning = "({n},{l},{h},{n},{l},{h})".format(n=nbins, l=low, h=hi)

	branch1 = "energy_ECAL_ele[0]/cosh(etaSCEle[0])"
	branch2 = branch1.replace("[0]", "[1]")	
	branchname = "max({0},{1}):min({0},{1})".format(branch1, branch2)

	f = ROOT.TFile.Open("tmp/EtBins.root", "RECREATE")

	for region in regions:
		if region == "ALL": region = ""
		print region
		isMC = False
		print "Data"
		data = plot.GetTH1(data_c, branchname, isMC, binning, region + '-' + common_cut)
		isMC = True
		print "Inc"
		inc = plot.GetTH1(inc_c, branchname, isMC, binning, region + '-' + common_cut, noWeights=True)
		print "LT"
		lt = plot.GetTH1(lt_c, branchname, isMC, binning, region + '-' + common_cut, noWeights=True)
		data.Write()
		inc.Write()
		lt.Write()

	f.Close()
	f = ROOT.TFile.Open(outfile,"READ")

histograms = {}
hist_index = 0
for region in regions:
	r = {}
	r["data"] = f.Get("hist%d" % hist_index)
	hist_index +=1
	r["inc"] = f.Get("hist%d" % hist_index)
	hist_index +=1
	r["lt"] = f.Get("hist%d" % hist_index)
	hist_index +=1
	histograms[region] = r

integrals = {}
results = {}
for region in regions:
	if not region: region = "All"
	values, bins, data_int = plot.makeDiagBins(histograms[region]["data"], 10000, min_value=32.)
	results[region] = values
	r = {}
	r["data_check"] = data_int
	r["data"] = [histograms[region]["data"].Integral(b1, b2-1, b1, b2-1) for b1, b2 in zip(bins, bins[1:]) ]
	r["inc"]  = [histograms[region]["inc"].Integral(b1, b2-1, b1, b2-1)  for b1, b2 in zip(bins, bins[1:]) ]
	r["lt"]   = [histograms[region]["lt"].Integral(b1, b2-1, b1, b2-1)   for b1, b2 in zip(bins, bins[1:]) ]
	integrals[region] = r
	#for b, d1, d2 in zip(bins, r["data_check"], r["data"]):
	#	print region, b, d1, d2

for region in regions:
	for key in ["data", "inc", "lt"]:
		plot.Draw2D(histograms[region][key], "E_{T}", "ET_bins_{}_{}".format(key,region), plotdir="plots/etbins/", diagBins=results[region], logy=True, logx=True)
headers = ["data_check", "data", "inc", "lt"]
print "region " + " ".join(headers) + " bin"
for region in regions:
	if not region: region = "All"
	for i in xrange(len(integrals[region]["data"])):
		print "{:30}".format(region),
		initial = None
		for key in headers:
			if not initial:
				initial = integrals[region][key][i]
			print "{:.3f}".format(integrals[region][key][i]/initial),
		print "{:3.0f} - {:3.0f}".format(results[region][i] , results[region][i+1])
	
