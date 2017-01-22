import ROOT
from ElectronCategory import cutter

colors = [ROOT.kRed, ROOT.kGreen, ROOT.kBlue, ROOT.kCyan]

hist_index = 0

def GetTH1(chain, branchname, isMC, binning="", category="", selection="", label="",
		 usePU=True, smear=False, scale=False, useR9Weight=False):
	global hist_index
	#enable only used branches
	chain.SetBranchStatus("*",0)
	branchlist = cutter.GetBranchNameNtupleVec(category)
	for b in branchlist:
		print "[Status] Enabling branch:", b
		chain.SetBranchStatus(b.Data(), 1)
	chain.SetBranchStatus(branchname, 1)

	selection = ROOT.TCut("")
	if(category):
		selection = cutter.GetCut(category, False, 0 , scale)
	
	if(smear and isMC):
		#print "Apply smear to the MC"
		print "[WARNING] smearing not implemented"
	if(scale and not isMC):
		print "[WARNING] scale not implemented"
	if(useR9Weight):
		print "[WARNING] R9weight not implemented"



	weights = ROOT.TCut("")
	if(isMC):
		chain.SetBranchStatus("mcGenWeight", 1)
		weights *= "mcGenWeight"
		if(usePU):
			chain.SetBranchStatus("puWeight", 1)
			weights *= "puWeight"
	

	histname = "hist%d" % hist_index
	chain.Draw(branchname + ">>" + histname + binning, selection * weights, "")
	h = ROOT.gDirectory.Get(histname)
	h.SetTitle(label)
	hist_index += 1
	return h



def PlotDataMC(data, mc, file_path, file_basename, xlabel="", ylabel="",
		ylabel_unit="", logy=False, ratio=True):
	global color

	if(ratio):
		print "[WARNING] ratio not implemented"

	data.SetXTitle(xlabel)
	data.SetYTitle(ylabel + " /(%.2f %s)" %(data.GetBinWidth(2), ylabel_unit))

	data.SetMarkerStyle(20)
	data.SetMarkerSize(1)


	mc.SetMarkerStyle(20)
	mc.SetMarkerSize(1)
	mc.SetMarkerColor(colors[0])
	mc.SetFillStyle(0)
	mc.SetFillColor(colors[0])
	mc.SetLineColor(colors[0])
	mc.SetLineWidth(2)

	dataintegral = data.Integral()
	data.Scale(1/dataintegral)
	mc.Scale(1/dataintegral)

	c = ROOT.TCanvas("c","")

	maximum = max(data.GetMaximum(), mc.GetMaximum())
	if(logy):
		minimum = 0.001
		maximum *= 5
		c.SetLogy()
	else:
		minimum = 0.0
		maximum *= 1.2
	
	print minimum, maximum
	data.GetYaxis().SetRangeUser(minimum, maximum)
	data.Draw("p")
	mc.DrawNormalized("hist same", data.Integral())

	x0 = .60
	y0 = .7
	xw = .24
	yw = .15
	leg = ROOT.TLegend(x0, y0, x0+xw, y0+yw)
	leg.AddEntry(data, data.GetTitle(), "p")
	leg.AddEntry(mc, mc.GetTitle(), "lf")
	leg.Draw()

	pv = ROOT.TPaveText(0.25,0.92,0.65,.97,"NDC")
	pv.AddText("CMS Preliminary 2016")
	pv.SetFillColor(0)
	pv.SetBorderSize(0)
	pv.Draw()

	c.SaveAs(file_path + '/' + file_basename + ".png")
	c.SaveAs(file_path + '/' + file_basename + ".pdf")
	c.SaveAs(file_path + '/' + file_basename + ".eps")
	c.SaveAs(file_path + '/' + file_basename + ".C")
	

if __name__ == "__main__": 
	import sys
	data = sys.argv[1]
	mc = sys.argv[2]
	branchname = sys.argv[3]

	ROOT.gROOT.SetBatch(True)
	ROOT.gStyle.SetOptStat(0)
	ROOT.gStyle.SetOptTitle(0)

	eleID = "eleID_loose25nsRun22016Moriond"
	category = "Et_25-" + eleID
	binning = "(100,60,120)"

	print "[STATUS] Data"
	isMC = False
	data_chain = ROOT.TFile.Open(data).Get("selected")
	data_h = GetTH1(data_chain, branchname, isMC, binning, category, label="Data" )

	print "[STATUS] MC"
	isMC = True
	mc_chain = ROOT.TFile.Open(mc).Get("selected")
	mc_h = GetTH1(mc_chain, branchname, isMC, binning, category, label="DYJets_madgraph")

	PlotDataMC(data_h, mc_h, "plots/", branchname, xlabel=branchname, ylabel="Events", ylabel_unit="GeV", logy = True)
	print "[STATUS] Done"
	
