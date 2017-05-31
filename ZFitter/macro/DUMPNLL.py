import ROOT
import sys
ROOT.gROOT.SetBatch()

logfile = sys.argv[1]
region_file = sys.argv[2]
try:
	tag = sys.argv[3]
except:
	tag = ""

with open(region_file, "r") as regionfile:
	categories = [x.strip().split("#")[0] for x in regionfile]
#strip out empty lines and extra whitespace
categories = [x.strip() for x in categories if x]

ncat = len(categories)

table = [line.strip() for line in open(logfile, "r") if "DUMP NLL" in line]

mc_stats = ROOT.TH2F("dielectron_cat_mc","MC stats;Category 1;Category 2", ncat, 0, ncat, ncat, 0, ncat)
data_stats = ROOT.TH2F("dielectron_cat_data","Data stats;Category 1;Category 2", ncat, 0, ncat, ncat, 0, ncat)
from collections import defaultdict
inactive = defaultdict(list)
inactives = ROOT.TProfile2D("inactives","", ncat, 0, ncat, ncat, 0, ncat)
for line in table[1:]:
	cat1, cat2, nll, mc, data, isActive, mc_, data_, = map(float,line[11:].split())
	mc_stats.Fill(cat1, cat2, mc)
	data_stats.Fill(cat1, cat2, data)
	if not isActive:
		inactives.Fill(cat1, cat2,1)
	inactive[int(cat1)].append(isActive)
	inactive[int(cat2)].append(isActive)

region_tag = region_file.split('/')[-1].split('.')[0]
if tag:
	region_tag += "_" + tag


ROOT.gStyle.SetOptStat(0)
c = ROOT.TCanvas()
c.SetLogz()
mc_stats.Draw("colz")
inactives.Draw("sametext")
c.SaveAs("plots/" + region_tag + "_mc_stats.png")
data_stats.Draw("colz")
inactives.Draw("sametext")
c.SaveAs("plots/" + region_tag + "_data_stats.png")

for cat in inactive:
	if not any(inactive[cat]):
		print categories[cat] + " is completely inactive"
