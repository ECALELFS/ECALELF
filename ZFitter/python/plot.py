import ROOT
from ElectronCategory import cutter,defaultEnergyBranch,ReduceFormula

colors = [ROOT.kRed, ROOT.kGreen, ROOT.kBlue, ROOT.kCyan, ROOT.kYellow, ROOT.kBlack, ROOT.kRed, ROOT.kGreen, ROOT.kBlue, ROOT.kCyan, ROOT.kYellow, ROOT.kBlack ]
ndraw_entries = 1000000000
#ndraw_entries = 100000

hist_index = 0

def GetTH1(chain, branchname, isMC, binning="", category="", label="",
		 usePU=True, smear=False, scale=False, useR9Weight=False, energyBranchName = None, norm="1"):
	global hist_index
	normfactor = ROOT.TCut("",str(norm))
	#enable only used branches
	ActivateBranches(chain, branchname, category, energyBranchName)
	selection, weights = GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU)

	histname = "hist%d" % hist_index
	print "[DEBUG] Getting TH1F of " + branchname + binning + " with " + str(selection) + " and weights" + str(weights)
	chain.Draw(branchname + ">>" + histname + binning, selection * weights * normfactor, "", ndraw_entries)
	h = ROOT.gDirectory.Get(histname)
	h.SetTitle(label)
	hist_index += 1
	return h

def GetTH1Stack(chain, splits, branchname, isMC, binning="", category="", label="",
		 usePU=True, smear=False, scale=False, useR9Weight=False, energyBranchName = None):
	global hist_index
	ActivateBranches(chain, branchname, category, energyBranchName)
	selection, weights = GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU)

	#print "[DEBUG] Getting TH1F of " + branchname + binning + " with " + str(selection) + " and weights" + str(weights)
	
	stack_labels = ["LT_5To75", "LT_75To80", "LT_80To85", "LT_85To90", "LT_90To95", "LT_95To100", "LT_100To200", "LT_200To400", "LT_400To800", "LT_800To2000",]

	nbins, low_b, hi_b = eval(binning)
	# Make histograms
	stack = [ ROOT.TH1F(label + stack_labels[i], stack_labels[i], nbins, low_b, hi_b) for i in range(len(stack_labels))]

	oldfilename = ""
	index = -1
	selector = ROOT.TTreeFormula("selector", selection.GetTitle(), chain)
	weighter = ROOT.TTreeFormula("weighter", weights.GetTitle(), chain)
	brancher = ROOT.TTreeFormula("brancher", branchname, chain)
	for event in chain:
		selector.UpdateFormulaLeaves()
		if not selector.EvalInstance(): continue
		filename = event.GetTree().GetName()
		if filename is not oldfilename:
			for nlabel, label in enumerate(stack_labels):
				if label in filename:
					index = nlabel
			oldfilename = filename

		weighter.UpdateFormulaLeaves()
		weight = float(weighter.EvalInstance())

		brancher.UpdateFormulaLeaves()
		value = brancher.EvalInstance()
		stack[index].Fill(value, weight)

	hist_index += 1
	return stack

def GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU):
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
		if(usePU and CheckForBranch(chain, "puWeight")):
			chain.SetBranchStatus("puWeight", 1)
			weights *= "puWeight"

	if CheckForBranch(chain, "LTweight"):
		weights *= "LTweight"

	return selection, weights

def CheckForBranch(chain, branch):
	chain.GetEntry(0)
	try:
		chain.__getattr__(branch)
	except AttributeError:
		return False
	return True


def ActivateBranches(chain, branchnames, category, energyBranchName):
	chain.SetBranchStatus("*", 0)
	if energyBranchName:
		cutter.energyBranchName=energyBranchName
	else:
		cutter.energyBranchName=defaultEnergyBranch
		
	if category:
		branchlist = cutter.GetBranchNameNtupleVec(category)
	else:
		branchlist = []

	if CheckForBranch(chain, "LTweight"):
		branchlist.push_back("LTweight")

	for b in branchlist:
		print "[Status] Enabling branch:", b
		chain.SetBranchStatus(b.Data(), 1)
	for branchname in AsList(branchnames):
		for br in ReduceFormula(branchname):
			if not br: continue
			print "[Status] Enabling branch:", br
			chain.SetBranchStatus(br, 1)


def AsList(arg):
	try:
		ROOT.TObject(arg)
		return [arg,]
	except:
		if(isinstance(arg,str)):
			return [arg]
		else:
			try:
				iter(arg)
				return arg
			except:
				return [arg]

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
		ylabel_unit="", logy=False, ratio=True, stack_data=False, stack_mc=False):

	mc_list = AsList(mc)
	data_list = AsList(data)

	if(ratio):
		print "[WARNING] ratio not implemented"

	c = ROOT.TCanvas("c", "c", 600, 480)

	maximum = max( [h.GetMaximum() for h in  data_list] + [h.GetMaximum() for h in mc_list])
	if(logy):
		minimum = 0.001
		maximum *= 5
		c.SetLogy()
	else:
		minimum = 0.0
		maximum *= 1.2
	
	if data_list: 
		h0 = data_list[0]
	elif mc_list:
		h0 = mc_list[0]
	else:
		raise Exception("no histograms")

	h0.SetXTitle(xlabel)
	h0.SetYTitle(ylabel + " /(%.2f %s)" %(h0.GetBinWidth(2), ylabel_unit))
	h0.GetYaxis().SetRangeUser(minimum, maximum)

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
