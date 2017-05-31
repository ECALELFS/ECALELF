import ROOT
import numpy as np
from collections import defaultdict

low_et = defaultdict(list)
hi_et = defaultdict(list)
scale_withlt = defaultdict(list)
scale_err_withlt = defaultdict(list)
scale_nolt = defaultdict(list)
scale_err_nolt = defaultdict(list)

max_et = 1000
min_et = 27
max_scale = 0
min_scale = 2
for line in open("test/dato/comp.txt"):
	if line[0] == "#": continue
	category = line.split()[0]
	let, hiet, sw, sew, swo, sewo = map(float, line.split()[1:])
	low_et[category].append(let)
	hi_et[category].append(hiet)
	scale_withlt[category].append(sw)
	scale_err_withlt[category].append(sew)
	scale_nolt[category].append(swo)
	scale_err_nolt[category].append(sewo)
	max_scale = max([max_scale, sw+sew, swo+sewo])
	min_scale = min([min_scale, sw-sew, swo-sewo])


ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
c = ROOT.TCanvas("c","c",1280,720)
c.SetLeftMargin(.15)
dummy = ROOT.TH1F("h","",1,min_et, max_et)
dummy.Draw()
pad = .05 * (max_scale - min_scale)

print max_scale + pad, min_scale - pad
dummy.SetMaximum(1.004)
dummy.SetMinimum(.996)
dummy.SetXTitle("E_T [GeV]")
dummy.SetYTitle("Scale")
dummy.GetYaxis().SetTitleOffset(1.8)

leg = ROOT.TLegend(.15, .7, .9, .9)

def makeGraph(category, color, style):
	low_et_np = np.array(low_et[category], dtype=float)
	hi_et_np = np.array(hi_et[category], dtype=float)
	scale_withlt_np = np.array(scale_withlt[category], dtype=float)
	scale_err_withlt_np = np.array(scale_err_withlt[category], dtype=float)
	scale_nolt_np = np.array(scale_nolt[category], dtype=float)
	scale_err_nolt_np = np.array(scale_err_nolt[category], dtype=float)

	et_ave = (low_et_np + hi_et_np) / 2.
	et_err = abs(low_et_np - hi_et_np) / 2.
	print et_err
	graph_withlt = ROOT.TGraphErrors(len(et_ave), et_ave, scale_withlt_np,  et_err, scale_err_withlt_np)
	graph_nolt = ROOT.TGraphErrors(len(et_ave), et_ave, scale_nolt_np,  et_err, scale_err_nolt_np)

	graph_withlt.SetMarkerColor(ROOT.kBlue)
	graph_nolt.SetMarkerColor(ROOT.kRed)
	graph_withlt.SetMarkerStyle(style)
	graph_nolt.SetMarkerStyle(style)
	graph_withlt.Draw("sameP")
	graph_nolt.Draw("sameP")

	leg.AddEntry(graph_withlt, "With LT " + category, "lp")
	leg.AddEntry(graph_nolt, "Without LT " + category, "lp")
	return graph_withlt, graph_nolt

graphs = []
for color,category in  enumerate(low_et):
	graphs.append(makeGraph(category, color + 1, color + 20))

leg.SetNColumns(2)
leg.Draw()
c.SetLogx()
c.SaveAs("plots/scales_comp.png")

