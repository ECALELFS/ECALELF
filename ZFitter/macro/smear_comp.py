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
from ElectronCategory import cutter

#plot.ndraw_entries = 10000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

tmpfile = ROOT.TFile.Open("tmp/tmp.root","RECREATE")
category = "Et_25-isEle-eleID_loose25nsRun22016Moriond"
selection = str(cutter.GetCut(category, False, 0 , False) + ROOT.TCut("smearerCat[0] >= 1"))

data_step7_filename = "tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned_step4/d_chain.root"
data_step7_chain = ROOT.TFile.Open(data_step7_filename).Get("selected")

data_step2_filename = "tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned_HggRunEtaR9/d_chain.root"
data_step2_chain = ROOT.TFile.Open(data_step2_filename).Get("selected")

mc_filename = "tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned/s_chain.root"
mc_chain = ROOT.TFile.Open(mc_filename).Get("selected")

mc_step4_filename = "tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned_step4/s_chain.root"
mc_step4_chain = ROOT.TFile.Open(mc_step4_filename).Get("selected")
mc_step7_filename = "tmp/Moriond17_oldRegr_Hgg_reMiniAOD_withLTbinned_step7/s_chain.root"
mc_step7_chain = ROOT.TFile.Open(mc_step7_filename).Get("selected")

#inclusive vs binned stack
#invmass
hist_index = 0

isMC = True

cat_branch =  [x.GetName() for x in data_step7_chain.GetListOfFriends() if x.GetName().startswith("smearerCat")][0]

#open regions file, taking care of comments
region_name = cat_branch.replace("smearerCat_","")
with open("data/regions/" + region_name + ".dat", "r") as regionfile:
	categories = [x.strip().split("#")[0] for x in regionfile]

#strip out empty lines and extra whitespace
categories = [x.strip() for x in categories if x]

ncat = len(categories)

from itertools import combinations_with_replacement as cwr
dicat = cwr(range(ncat), 2)
dicat_names = ["%s-%s" % (a,b) for a,b in cwr(categories, 2)]

def comp(branchname, filename, binning, xlabel, ratio, nele, logx=False, remake=False):
	global hist_index

	branchname = "smearerCat[0]:" + branchname
	nbins,low,hi = eval(binning)
	binning = "({nbins},{low},{hi},{ncat},0,{ncat})".format(nbins=nbins,low=low,hi=hi,ncat=ncat*(ncat+1)/2)
	if not remake:
		f = ROOT.TFile.Open("tmp/smear_comp_%s.root" % filename, "READ")
		histos = [ f.Get("hist%d" % i) for i in range(hist_index, 5 + hist_index)]
		data_step7_hist, data_step2_hist, mc_h, mc_step4_h, mc_step7_h = histos
		data_hists = [data_step7_hist, data_step2_hist]
		mc_hists = [mc_h, mc_step4_h, mc_step7_h]
		hist_index += 5
	else:
		data_step7_hist = plot.GetTH1(data_step7_chain, branchname, not isMC, binning, category, label="Data HggRunEtaR9Et",          usePU = False, useEleIDSF=nele, scale=True)
		data_step2_hist = plot.GetTH1(data_step2_chain, branchname, not isMC, binning, category, label="Data HggRunEtaR9",          usePU = False, useEleIDSF=nele, scale=True)
		data_hists = [data_step7_hist, data_step2_hist]

		mc_h       = plot.GetTH1(mc_chain,       branchname, isMC,  binning, category, label="no Correction", usePU = False, useEleIDSF=nele, smear=False)
		mc_step4_h = plot.GetTH1(mc_step4_chain, branchname, isMC,  binning, category, label="Step4 Smear",   usePU = False, useEleIDSF=nele, smear=True)
		mc_step7_h = plot.GetTH1(mc_step7_chain, branchname, isMC,  binning, category, label="Step7 Smear",   usePU = False, useEleIDSF=nele, smear=True)
		mc_hists = [mc_h, mc_step4_h, mc_step7_h]

	if remake:
		f = ROOT.TFile.Open("tmp/smear_comp_%s.root" % filename, "RECREATE")
		for h in data_hists + mc_hists:
			h.Write()
		f.Close()

	c = ROOT.TCanvas()
	names = ["data_HggRunEtaR9Et","data_HggRunEtaR9","uncorrected","step4","step7"]
	for name, h in zip(names, data_hists + mc_hists):
		h.Draw("colz")
		c.SaveAs("plots/scale_smear/%s.png" % name)

	data_step7_hs = ROOT.THStack(data_step7_hist, "x")
	data_step2_hs = ROOT.THStack(data_step2_hist, "x")
	mc_hs         = ROOT.THStack(mc_h,            "x")
	mc_step4_hs   = ROOT.THStack(mc_step4_h,      "x")
	mc_step7_hs   = ROOT.THStack(mc_step7_h,      "x")


	#print "CHI2 Category", " ".join(names[2:])
	for i,dicat_name in enumerate(dicat_names):
		ds = [data_step7_hs.GetHists()[i], data_step2_hs.GetHists()[i]]
		plot.ColorData(ds)
		mcs = [mc_hs.GetHists()[i], mc_step4_hs.GetHists()[i], mc_step7_hs.GetHists()[i]]
		plot.ColorMCs(mcs)
		plot.Normalize(ds, mcs)

		ds[0].GetXaxis().SetRangeUser(80,100)

		#print "CHI2", dicat_name,
		#for h in mcs:
			#ch2 = ds[0].Chi2Test(h, "UW CHI2/NDF")
			#print ch2,
		#for h in mcs:
			#print h.GetEntries(),
		#print
		plot.PlotDataMC(ds[0], mcs, "plots/smear_comp/", filename + "_" + dicat_name,
				x_range=(80,100), xlabel=xlabel + " [GeV]", ylabel="Events", ylabel_unit="GeV", ratio=ratio, logx=logx)
		mcs[1].SetFillStyle(1001)
		mcs[1].SetFillColor(ROOT.kAzure + 6)
		mcs[1].SetLineColor(ROOT.kAzure + 6)
		plot.PlotDataMC(ds, mcs[1], "plots/scale_comp/", filename + "_" + dicat_name,
				x_range=(80,100), xlabel=xlabel + " [GeV]", ylabel="Events", ylabel_unit="GeV", ratio=False, logx=logx)

branchname = "invMass_ECAL_pho"
filename = "invMass_ECAL_pho"
binning = "(100,60,120)"
xlabel  = "M_{ee}"
comp(branchname, filename, binning, xlabel, (.9,1.1), 2, remake = False)
