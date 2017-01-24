import sys, os
import argparse

parser = argparse.ArgumentParser(description='Standard Data MC plots')
parser.add_argument("branch",nargs="+", help="Branch names to plot")
parser.add_argument("-d", "--data", action="append", help="specify data files")
parser.add_argument("-s", "--mc",   action="append", help="specify mc files")

args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

eleID = "eleID_loose25nsRun22016Moriond"
category = "Et_25-" + eleID
binning = "(100,60,120)"

for branchname in args.branch:
	print "[STATUS] Data: " + branchname
	isMC = False
	data_chains = [ROOT.TFile.Open(data).Get("selected") for data in args.data]
	data_hs = [plot.GetTH1(data_chain, branchname, isMC, binning, category, label="Data" ) for data_chain in data_chains]
	plot.ColorData(data_hs)

	print "[STATUS] MC: " +  branchname
	isMC = True
	mc_chains = [ROOT.TFile.Open(mc).Get("selected") for mc in args.mc]
	mc_hs = [plot.GetTH1(mc_chain, branchname, isMC, binning, category, label="DYJets_madgraph") for mc_chain in mc_chains]
	plot.ColorMCs(mc_hs)

	plot.Normalize(data_hs, mc_hs)
	plot.PlotDataMC(data_hs, mc_hs, "plots/", branchname, xlabel=branchname, ylabel="Events", ylabel_unit="GeV", logy = False)
	print "[STATUS] Done: " + branchname
