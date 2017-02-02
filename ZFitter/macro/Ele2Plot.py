import sys, os
import argparse

category = "absEta_0_1-gold-EtLeading_32-EtSubLeading_20-noPF-isEle-eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose"

parser = argparse.ArgumentParser(description='Standard Data MC plots', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("binning", help="(nbins,low,hi)")
parser.add_argument("file", help="specify data file")
parser.add_argument("name", help="outfile base name")
parser.add_argument("-c", "--category", help="category string", default=category)
#specify the branch for electron [0], second axis will replace [0] with [1]
parser.add_argument("-b", "--branch", help="branch to plot (yaxis [0] -> [1])", default="energy_ECAL_ele[0]/cosh(etaSCEle[0])")
parser.add_argument("--isMC", action="store_true", help="isMC for GetTH1()", default=False)

args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

#plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

binning = args.binning

chain = ROOT.TFile.Open(args.file).Get("selected")
branchname = args.branch + ":" + args.branch.replace("[0]", "[1]")
isMC = False

h = plot.GetTH1(chain, branchname, isMC, binning, args.category)

f = ROOT.TFile.Open("tmp/" + args.name + ".root", "RECREATE")
h.Write()
f.Close()

