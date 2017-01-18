import sys, os
import argparse

category = "absEta_0_1-gold-EtLeading_32-EtSubLeading_20-noPF-isEle-eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose"

parser = argparse.ArgumentParser(description='Standard Data MC plots')
parser.add_argument("binning", help="(nbins,low,hi)")
parser.add_argument("file", help="specify data file")
parser.add_argument("name", help="outfile base name")
parser.add_argument("-c", "--category", help="category string", default=category)

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
branchname = "energy_ECAL_ele[0]/cosh(etaSCEle[0]):energy_ECAL_ele[1]/cosh(etaSCEle[1])"
isMC = False

et_h = plot.GetTH1(chain, branchname, isMC, binning, args.category)

f = ROOT.TFile.Open("tmp/" + args.name + ".root", "RECREATE")
et_h.Write()
f.Close()

