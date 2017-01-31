import sys, os
import argparse

category = "absEta_0_1-gold-EtLeading_32-EtSubLeading_20-noPF-isEle-eleID_cutBasedElectronID|Spring15|25ns|V1|standalone|loose"

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

chain = ROOT.TFile.Open("tmp/LTregrOld/s1_chain.root").Get("selected")

stack = plot.GetTH1Stack(chain, "LTBinned", "energy_ECAL_ele[0]/cosh(etaSCEle[0])+energy_ECAL_ele[1]/cosh(etaSCEle[1])", True, binning="(200,20,120)", category="eleID_loose25nsRun22016Moriond", label="LT")

outfile = ROOT.TFile.Open("LT_LT_hist.root", "RECREATE")

for h in stack:
	h.Write()
outfile.Close()

