import ROOT
from ElectronCategory import cutter

colors = [ROOT.kRed, ROOT.kGreen, ROOT.kBlue, ROOT.kCyan]
ndraw_entries = 1000000000

hist_index = 0

def GetTH1(chain, branchname, isMC, binning="", category="", selection="", label="",
		 usePU=True, smear=False, scale=False, useR9Weight=False):
	global hist_index
	#enable only used branches
	chain.SetBranchStatus("*", 0)
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
	print "[DEBUG] Getting TH1F of " + branchname + binning + " with " + str(selection) + " and weights" + str(weights)
	chain.Draw(branchname + ">>" + histname + binning, selection * weights, "", ndraw_entries)
	h = ROOT.gDirectory.Get(histname)
	h.SetTitle(label)
	hist_index += 1
	return h

def AsList(arg):
	try:
		ROOT.TObject(arg)
		return [arg,]
	except:
		return arg

def NormHists(data, signal):

	try:
		ndatas = len(data)
		data_hists = data
	except TypeError:
		data_hists = [data]

def ColorMCs(mcs):
	mc_hists = AsList(mcs)

	for i, h in enumerate(mc_hists):
		h.SetMarkerStyle(20)
		h.SetMarkerSize(1)
		h.SetMarkerColor(colors[i])
		h.SetFillStyle(0)
		h.SetFillColor(colors[i])
		h.SetLineColor(colors[i])
		h.SetLineWidth(2)

def ColorData(data):
	d_hists = AsList(data)

	data_color = [ROOT.kBlack] + colors

	for i, h in enumerate(d_hists):
		h.SetMarkerStyle(20)
		h.SetMarkerSize(1)
		h.SetMarkerColor(data_color[i])

def Normalize(data, mc):
	"""
	Normalize MC histograms to data.Integral()
	if there is no data histogram, normalize to 1
	if there is more than one data histogram
	"""
	data_list = AsList(data)
	mc_list = AsList(mc)
	ndata = len(data_list)
	nmc = len(mc_list)

	if nmc > 0:
		if ndata:
			for h in mc_list:
				h.Scale(data_list[0].Integral()/h.Integral())
			return
		else:
			for h in mc_list:
				h.Scale(1./h.Integral())
			return
	if ndata > 1 and mc == 0:
		for h in data_list:
			h.Scale(1./h.Integral())
	else:
		print "[WARNING] No normalization defind for (ndata=%d, nmc%d)" % (ndata, nmc)
		
	


def PlotDataMC(data, mc, file_path, file_basename, xlabel="", ylabel="",
		ylabel_unit="", logy=False, ratio=True):

	mc_list = AsList(mc)
	data_list = AsList(data)

	if(ratio):
		print "[WARNING] ratio not implemented"

	c = ROOT.TCanvas("c", "c", 600, 600)

	maximum = max( max([h.GetMaximum() for h in  data_list]), max([h.GetMaximum() for h in mc_list]))
	if(logy):
		minimum = 0.001
		maximum *= 5
		c.SetLogy()
	else:
		minimum = 0.0
		maximum *= 1.2
	
	data0 = data_list[0]
	data0.SetXTitle(xlabel)
	data0.SetYTitle(ylabel + " /(%.2f %s)" %(data0.GetBinWidth(2), ylabel_unit))
	data0.GetYaxis().SetRangeUser(minimum, maximum)

	same = ""
	for h in data_list:
		h.Draw("p" + same)
		if not same: same = "same" 
	for h in mc_list:
		h.Draw("hist" + same)
		if not same: same = "same" 

	x0 = .60
	y0 = .7
	xw = .24
	yw = .15
	leg = ROOT.TLegend(x0, y0, x0+xw, y0+yw)
	for h in data_list:
		leg.AddEntry(h, h.GetTitle(), "p")
	for h in mc_list:
		leg.AddEntry(h, h.GetTitle(), "lf")
	leg.Draw()

	pv = ROOT.TPaveText(0.25, 0.92, 0.65, .97, "NDC")
	pv.AddText("CMS Preliminary 2016")
	pv.SetFillColor(0)
	pv.SetBorderSize(0)
	pv.Draw()

	c.SaveAs(file_path + '/' + file_basename + ".png")
	c.SaveAs(file_path + '/' + file_basename + ".pdf")
	c.SaveAs(file_path + '/' + file_basename + ".eps")
	c.SaveAs(file_path + '/' + file_basename + ".C")
