import sys, os
import argparse
import re

parser = argparse.ArgumentParser(description='category stats', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("chain", help="chain file")
parser.add_argument("name", help="name for output")
parser.add_argument("--isMC", action="store_true", default=False)
args = parser.parse_args()

sys.path.insert(0, os.getcwd() + '/python')
import ROOT
import plot
import progress
from ElectronCategory import cutter

ROOT.gROOT.SetBatch(True)
#common_cut = "eleID_loose25nsRun22016Moriond-EtLeading_32-EtSubLeading_20-noPF-isEle"
#tcut = cutter.GetCut(common_cut, args.isMC)
#plot.ndraw_entries = 100000

from collections import defaultdict
f  = ROOT.TFile.Open(args.chain)
chain = f.Get("selected")

branchname =  [x.GetName() for x in chain.GetListOfFriends() if x.GetName().startswith("smearerCat")][0]

#open regions file, taking care of comments
region_name = branchname.replace("smearerCat_","")
with open("data/regions/" + region_name + ".dat", "r") as regionfile:
	categories = [x.strip().split("#")[0] for x in regionfile]

#strip out empty lines and extra whitespace
categories = [x.strip() for x in categories if x]

ncat = len(categories)

from itertools import combinations_with_replacement as cwr
dicat = cwr(range(ncat), 2)
dicat_names = cwr(categories, 2)
cat1,cat2 =  map(list, zip(*dicat))


c = ROOT.TCanvas()
ROOT.gStyle.SetOptStat(0)
progress.start(args.name)
h2 = ROOT.TH2F(args.name, args.name, ncat, 0, ncat, ncat, 0 ,ncat)

lt_h2 = {}

pattern = re.compile(".*(LT.*?)-.*")
oldfilename = ""
#plot.ActivateBranches(chain, "smearerCat", common_cut)
plot.ActivateBranches(chain, "smearerCat", "")
#selector = ROOT.TTreeFormula("selector", str(tcut), chain)
for i,entry in enumerate(chain):
	filename = entry.GetFile().GetName()
	if filename != oldfilename:
		#selector.UpdateFormulaLeaves()
		#if selector.EvalInstance() == False: continue
		oldfilename = filename
		if "LT" in filename:
			h2key = pattern.match(filename).group(1)
			if h2key not in lt_h2:
				lt_h2[h2key] = ROOT.TH2F(args.name + h2key, args.name + ' ' + h2key, ncat, 0, ncat, ncat, 0 ,ncat)
		else:
			h2key = None
	progress.update(100. * i/chain.GetEntries())
	if entry.smearerCat[0] >=0:
		x = cat1[entry.smearerCat[0]]
		y = cat2[entry.smearerCat[0]]
		h2.Fill(x,y)
		if h2key in lt_h2:
			lt_h2[h2key].Fill(x,y)
progress.end()

h2.Draw("colz")
c.SetLogz()
c.SaveAs("plots/smearerCat_" + h2.GetName() + "_" + region_name + ".png")
for key in lt_h2:
	lt_h2[key].Draw("colz")
	c.SaveAs("plots/smearerCat_" + lt_h2[key].GetName() + "_" + region_name + ".png")

from distutils.version import LooseVersion
header =  "{} {:35} {:35} {:11}".format(args.name, "cat1", "cat2", args.name)
sorted_keys = lt_h2.keys()
sorted_keys.sort(key=LooseVersion)
for key in sorted_keys:
	header += " {:11}".format(key)
print header
for n, names in enumerate(dicat_names):
	gbin = h2.FindBin(cat1[n], cat2[n])
	line = "{0} {names[0]:35} {names[1]:35} {1:>11}".format(args.name, int(h2.GetBinContent(gbin)), names=names, )
	for key in sorted_keys:
		line += " {:>11}".format(int(lt_h2[key].GetBinContent(gbin)))
	print line

f.Close()
