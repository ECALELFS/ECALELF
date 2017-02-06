import sys, os
import argparse

parser = argparse.ArgumentParser(description='Standard Data MC plots', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
#parser.add_argument("binning", help="(nbins,low,hi)")
#parser.add_argument("file", help="specify data file")
#parser.add_argument("name", help="outfile base name")
#parser.add_argument("-c", "--category", help="category string", default=category)
##specify the branch for electron [0], second axis will replace [0] with [1]
#parser.add_argument("-b", "--branch", help="branch to plot (yaxis [0] -> [1])", default="energy_ECAL_ele[0]/cosh(etaSCEle[0])")
#parser.add_argument("--isMC", action="store_true", help="isMC for GetTH1()", default=False)

args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

infile = ROOT.TFile.Open("LT_LT_hist.root")
stack = [ key.ReadObj() for key in infile.GetListOfKeys() ] 

stack,_ = plot.NormalizeStack(stack)
plot.ColorMCs(stack)

incfile = ROOT.TFile.Open("LT_inc.root")
mc_inc = incfile.Get("hist0")

plot.Normalize([],mc_inc)
plot.ColorMCs(mc_inc)

mc_inc.SetLineColor(ROOT.kBlack)

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
plot.PlotDataMC([], [mc_inc] +  list(stack), "plots/validation/LTBinned/",  "LT_stack", xlabel="L_{T}", ylabel="Events", ylabel_unit="GeV", logy = False, ratio=True)
