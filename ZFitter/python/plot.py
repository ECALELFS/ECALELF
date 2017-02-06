import ROOT
from ElectronCategory import cutter,defaultEnergyBranch,ReduceFormula

colors = [ROOT.kRed, ROOT.kGreen, ROOT.kBlue, ROOT.kCyan, ROOT.kMagenta, ROOT.kOrange-6, ROOT.kViolet-6, ROOT.kGray+1, ROOT.kOrange-3, ROOT.kViolet-9]
ndraw_entries = 1000000000
#ndraw_entries = 100000

hist_index = 0

def GetTH1(chain, branchname, isMC, binning="", category="", label="",
		 usePU=True, useEleIDSF=True, smear=False, scale=False, useR9Weight=False, energyBranchName = None):
	global hist_index
	#enable only used branches
	ActivateBranches(chain, branchname, category, energyBranchName)
	selection, weights = GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU, useEleIDSF)

	histname = "hist%d" % hist_index
	print "[DEBUG] Getting TH1F of " + branchname + binning + " with " + str(selection) + " and weights" + str(weights)
	chain.Draw(branchname + ">>" + histname + binning, selection * weights, "", ndraw_entries)
	h = ROOT.gDirectory.Get(histname)
	h.SetTitle(label)
	hist_index += 1
	return h

def GetTH1Stack(chain, splits, branchname, isMC, binning="", category="", label="",
		 usePU=True, useEleIDSF=True,smear=False, scale=False, useR9Weight=False, energyBranchName = None):
	global hist_index
	ActivateBranches(chain, branchname, category, energyBranchName)
	selection, weights = GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU, useEleIDSF)

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
	maxevents = chain.GetEntries()
	i = 0
	for event in chain:
		if not (i % (maxevents/100)): print 100*i/maxevents, "%"
		i += 1
		selector.UpdateFormulaLeaves()
		sv = selector.EvalInstance()
		if not sv > 0: continue
		filename = event.GetFile().GetName()
		if filename != oldfilename:
			for nlabel, label in enumerate(stack_labels):
				if label in filename:
					index = nlabel
			print index, filename
			oldfilename = filename

		weighter.UpdateFormulaLeaves()
		weight = float(weighter.EvalInstance())

		brancher.UpdateFormulaLeaves()
		value = brancher.EvalInstance()
		stack[index].Fill(value, weight)
		
	return stack

def GetSelectionWeights(chain, category, isMC, smear, scale, useR9Weight, usePU, useEleIDSF):
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
		if(useEleIDSF):
			start = category.find("eleID_") + 6
			end = category.find("-",start)
			if end > 0:
				EleIDSF = "EleIDSF_" + category[start:end]
			else: 
				EleIDSF = "EleIDSF_" + category[start:]

			if CheckForBranch(chain, EleIDSF):
				weights *= "{id}[0]*{id}[1]".format(id=EleIDSF)
				chain.SetBranchStatus(EleIDSF, 1)
			else:
				print EleIDSF + " branch not present"

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
		if 'TH1' in arg.ClassName():
			return [arg,]
		elif "Array" in arg.ClassName():
			return arg
		else:
			print "[WARNING] AsList() encountered unhandled ROOT Class" + arg.ClassName()
			raise
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
		h.SetMarkerColor(colors[i % len(colors)])
		h.SetFillStyle(0)
		h.SetFillColor(colors[i % len(colors)])
		h.SetLineColor(colors[i % len(colors)])
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
	c.SetLeftMargin(.14)

	nhists = len(data_list) + len(mc_list)
	ncolumns = min(4,nhists)
	nrows = (nhists+1)/ncolumns
	c.SetTopMargin(.05*nrows)

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
	h0.GetYaxis().SetTitleOffset(1.5)

	same = ""
	for h in data_list:
		h.Draw("p" + same)
		if not same: same = "same" 
	for h in mc_list:
		h.Draw("hist" + same)
		if not same: same = "same" 

	x0 = c.GetLeftMargin()
	y0 = 1 - c.GetTopMargin()
	x1 = 1 - c.GetRightMargin()
	y1 = 1 - c.GetTopMargin()*.1
	leg = ROOT.TLegend(x0, y0, x1, y1)
	for h in data_list:
		leg.AddEntry(h, h.GetTitle(), "p")
	for h in mc_list:
		leg.AddEntry(h, h.GetTitle(), "l")
	leg.SetNColumns(ncolumns)
	leg.Draw()

	#pv = ROOT.TPaveText(0.25, 0.92, 0.65, .97, "NDC")
	#pv.AddText("CMS Preliminary 2016")
	#pv.SetFillColor(0)
	#pv.SetBorderSize(0)
	#pv.Draw()

	c.SaveAs(file_path + '/' + file_basename + ".png")
	c.SaveAs(file_path + '/' + file_basename + ".pdf")
	c.SaveAs(file_path + '/' + file_basename + ".eps")
	c.SaveAs(file_path + '/' + file_basename + ".C")

def NormalizeStack(stack, normalizeTo=1):

	stack_sum = sum([h.Integral() for h in stack])
	hstack = ROOT.THStack("hs","")
	for h in stack:
		h.Scale(1./stack_sum)
		hstack.Add(h)
	
	return hstack.GetStack(), hstack
