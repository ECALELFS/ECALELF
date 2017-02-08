import sys, os
import argparse

#parser = argparse.ArgumentParser(description='Validation plots')
#parser.add_argument("dat", help="dat file (with unique basename)")
#parser.add_argument("names", nargs="+", help="comma separted pairs for labels and names (e.g. s1,\"Madgraph MC\"")
#parser.add_argument("--plotdir", help="outdir for plots", default="plots/")
#
#args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

#plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

isMC = True
category = "Et_25-isEle-eleID_loose25nsRun22016Moriond"
#category = "Et_5-eleID_loose25nsRun22016Moriond"

inc_filename = "tmp/Moriond17_oldRegr/s1_chain.root"
inc_chain = ROOT.TFile.Open(inc_filename).Get("selected")

lt_binned_filename = "tmp/LTregrOld/s_chain.root"
lt_binned_all = ROOT.TFile.Open(lt_binned_filename).Get("selected")


def comp(branchname, filename, binning, xlabel, nele, ratio):
	inclusive_weights   = plot.GetTH1(inc_chain, branchname, isMC, binning, category, label="Inclusive w/ SF Weights", useEleIDSF=nele)
	inclusive_noweights = plot.GetTH1(inc_chain, branchname, isMC, binning, category, label="Inclusive w/o SF Weights", useEleIDSF=False)

	hists = [inclusive_weights, inclusive_noweights]
	plot.ColorMCs(hists)

	plot.PlotDataMC(inclusive_weights, inclusive_noweights, "plots/validation/EleIDSF/", "oldRegr_" + filename, xlabel=xlabel + " [GeV]", ylabel="Events", ylabel_unit="GeV", ratio=ratio)
	plot.Normalize([], hists)
	plot.PlotDataMC(inclusive_weights, inclusive_noweights, "plots/validation/EleIDSF/", "oldRegr_norm_" + filename, xlabel=xlabel + " [GeV]", ylabel="Fraction", ylabel_unit="GeV", ratio=ratio)


branchname = "invMass_ECAL_ele"
filename = "invMass_ECAL_ele"
binning = "(100,80,100)"
xlabel  = "M_{ee}"
comp(branchname, filename, binning, xlabel, 2, True)

branchname = "energy_ECAL_ele/cosh(etaSCEle)"
filename = "ET"
binning = "(100,20,81)"
xlabel  = "E_{T}"
comp(branchname, filename, binning, xlabel, 1, (.9, 1.1))

branchname = "etaSCEle"
filename = "eta"
binning = "(100,-2.5,2.5)"
xlabel  = "#eta_{SC}"
comp(branchname, filename, binning, xlabel, 1, True)

branchname = "R9Ele"
filename = "R9"
binning = "(100,.3,1)"
xlabel  = "R_{9}"
comp(branchname, filename, binning, xlabel, 1, True)
