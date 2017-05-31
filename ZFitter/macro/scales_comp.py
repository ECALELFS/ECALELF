import ROOT
import numpy as np
from collections import defaultdict
import re

low_et = defaultdict(list)
hi_et = defaultdict(list)
scales = defaultdict(list)
scale_errs = defaultdict(list)
sorted_categories = []
max_et = 1000
min_et = 27
max_scale = 0
min_scale = 2
for line in open("test/dato/Moriond17_newRegr_LT/loose25nsRun22016Moriond/invMass_ECAL_pho/table/step6EE-invMass_ECAL_pho-loose25nsRun22016Moriond-EtLeading_32-EtSubLeading_20-noPF-isEle-HggRunEtaR9Et.dat"):
	if line[0] == "#": continue
	if not "scale" in line: continue
	let, hiet, category, scale, scale_err = re.match("scale_Et_(\d*)_(\d*)-(.*)-EtLeading.*=  ([-0-9.]*)[^0-9]*([-0-9.]*).*",line).groups()
	if category not in sorted_categories:
		print category
		sorted_categories.append(category)
	let, hiet, scale, scale_err = map(float,[ let, hiet, scale, scale_err ])
	low_et[category].append(let)
	hi_et[category].append(hiet)
	scales[category].append(scale)
	scale_errs[category].append(scale_err)
	max_scale = max([max_scale, scale+scale_err])
	min_scale = min([min_scale, scale-scale_err])


plots = [
		("loweta_EB","absEta_0_1-bad", "absEta_0_1-gold", "|#eta| < 1, R_{9} < 0.94", "|#eta| < 1, R_{9} > 0.94",),
		("hieta_EB","absEta_1_1.4442-bad", "absEta_1_1.4442-gold", "|#eta| > 1, R_{9} < 0.94", "|#eta| > 1, R_{9} > 0.94"),
		("loweta_EE","absEta_1.566_2-bad", "absEta_1.566_2-gold", "|#eta| < 2, R_{9} < 0.94", "|#eta| < 2, R_{9} > 0.94"),
		("hieta_EE","absEta_2_2.5-bad", "absEta_2_2.5-gold", "|#eta| > 2, R_{9} < 0.94", "|#eta| > 2, R_{9} > 0.94"),
		]


def makeGraph(category, label, color, style):
	low_et_np = np.array(low_et[category], dtype=float)
	hi_et_np = np.array(hi_et[category], dtype=float)
	scales_np = np.array(scales[category], dtype=float)
	scale_errs_np = np.array(scale_errs[category], dtype=float)

	et_ave = (low_et_np + hi_et_np) / 2.
	et_err = abs(low_et_np - hi_et_np) / 2.
	graph= ROOT.TGraphErrors(len(et_ave), et_ave, scales_np,  et_err, scale_errs_np)

	graph.SetMarkerColor(color)
	graph.SetMarkerStyle(style)
	graph.Draw("sameP")

	leg.AddEntry(graph, label, "lp")
	return graph


for name, cat1, cat2, label1, label2 in plots:
	ROOT.gStyle.SetOptStat(0)
	ROOT.gStyle.SetOptTitle(0)

	c = ROOT.TCanvas("c","c",500,350)
	c.SetLeftMargin(.15)
	dummy = ROOT.TH1F("h","",1,min_et, max_et)
	dummy.Draw()
	pad = .05 * (max_scale - min_scale)

	print max_scale + pad, min_scale - pad
	dummy.SetMaximum(1.007)
	dummy.SetMinimum(.996)
	dummy.SetXTitle("E_T [GeV]")
	dummy.SetYTitle("Scale")
	dummy.GetYaxis().SetTitleOffset(1.8)

	leg = ROOT.TLegend(.15, .8, .9, .9)
	graphs = []
	#sorted_categories = low_et.keys()
	#sorted_categories.sort()
	categories = (cat1, cat2)
	labels = (label1, label2)
	for n,(category,label) in  enumerate(zip(categories, labels)):
		if "gold" in category:
			color = ROOT.kRed
		else:
			color = ROOT.kBlue
		marker = n + 20
		graphs.append(makeGraph(category, label, color, marker))

	leg.SetNColumns(2)
	leg.Draw()
	c.SetLogx()
	c.SaveAs("plots/step6_%s_scales.png" % name)

