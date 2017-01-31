import sys, os
import argparse

sys.path.insert(0, os.getcwd() + '/python')
import plot
import ROOT

chain = ROOT.TFile.Open("tmp/Moriond17_oldRegr/s1_chain.root").Get("selected")
f = ROOT.TFile.Open("LT_inc.root","RECREATE")
h = plot.GetTH1(chain, "energy_ECAL_ele[0]/cosh(etaSCEle[0])+energy_ECAL_ele[1]/cosh(etaSCEle[1])",
	True, binning="(4000,0,2000)", category="eleID_loose25nsRun22016Moriond",
	label="LT", usePU=False)

h.Write()
f.Close()
