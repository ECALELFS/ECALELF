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
category = "Et_5-eleID_loose25nsRun22016Moriond"

lt_names = [ "5 - 75", "75 - 80", "80 - 85", "85 - 90", "90 - 95", "95 - 100",
				 "100 - 200", "200 - 400", "400 - 800", "800 - 2000", ]

inc_filename = "tmp/Moriond17_oldRegr/s1_chain.root"
inc_chain = ROOT.TFile.Open(inc_filename).Get("selected")

lt_binned_chains = []
for i in range(1,11):
	lt_binned_filename = "tmp/LTregrOld/s%d_chain.root" % i
	lt_binned_chain = ROOT.TFile.Open(lt_binned_filename).Get("selected")
	lt_binned_chains.append(lt_binned_chain)

#inclusive vs binned stack
#invmass

def comp(branchname, filename, binning, xlabel):
	inclusive = plot.GetTH1(inc_chain, branchname, isMC, binning, category, label="Inclusive Madgraph", usePU = False)

	lt_hists = []
	lt_sum = 0
	for name, chain in zip(lt_names,lt_binned_chains):
		print "Getting", name
		h = plot.GetTH1(chain, branchname, isMC, binning, category, label= "L_{T} " + name + " GeV", usePU = False)
		lt_hists.append(h)
		lt_sum += h.Integral()


	plot.ColorData(inclusive)
	plot.ColorMCs(lt_hists)
	inclusive.Scale(1./inclusive.Integral())
	for h in lt_hists:
		h.Scale(1./lt_sum)

	lt_hists.reverse()
	plot.PlotDataMC(inclusive, lt_hists, "plots/validation/LTBinned/", "LT_comparison_" + filename, xlabel=xlabel + " [GeV]", ylabel="Fraction", ylabel_unit="GeV", stack_mc=True, ratio=True)

branchname = "invMass_ECAL_ele"
filename = "invMass_ECAL_ele"
binning = "(100,80,100)"
xlabel  = "M_{ee}"
#comp(branchname, filename, binning, xlabel)

branchname = "energy_ECAL_ele/cosh(etaSCEle)"
filename = "ET"
binning = "(100,1,81)"
xlabel  = "E_{T}"
#comp(branchname, filename, binning, xlabel)

branchname = "etaSCEle"
filename = "eta"
binning = "(100,-2.5,2.5)"
xlabel  = "#eta_{SC}"
#comp(branchname, filename, binning, xlabel)

branchname = "R9Ele"
filename = "R9"
binning = "(100,.3,1)"
xlabel  = "R_{9}"
#comp(branchname, filename, binning, xlabel)


# 2d ET data
