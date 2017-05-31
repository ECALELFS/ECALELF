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

#plot.ndraw_entries = 100000
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

isMC = True
category = "Et_25-isEle-eleID_loose25nsRun22016Moriond"
#category = "Et_5-eleID_loose25nsRun22016Moriond"

lt_names = [ "5 - 75", "75 - 80", "80 - 85", "85 - 90", "90 - 95", "95 - 100",
				 "100 - 200", "200 - 400", "400 - 800", "800 - 2000", ]

inc_filename = "tmp/Moriond17_oldRegr/s1_chain.root"
inc_chain = ROOT.TFile.Open(inc_filename).Get("selected")

lt_binned_chains = []
for i in range(1,11):
	lt_binned_filename = "tmp/LTregrOld/s%d_chain.root" % i
	lt_binned_chain = ROOT.TFile.Open(lt_binned_filename).Get("selected")
	lt_binned_chains.append(lt_binned_chain)

lt_binned_filename = "tmp/LTregrOld/s_chain.root"
lt_binned_all = ROOT.TFile.Open(lt_binned_filename).Get("selected")

datafilename = "tmp/Moriond17_oldRegr/d_chain.root"
data_chain = ROOT.TFile.Open(datafilename).Get("selected")

#inclusive vs binned stack
#invmass
hist_index = 0

def comp(branchname, filename, binning, xlabel, ratio, nele, logx=False, remake=False):
	global hist_index
	lt_hists = []
	lt_sum = 0

	if not remake:
		print "opening tmp/LT_%s.root" % filename
		f = ROOT.TFile.Open("tmp/LT_%s.root" % filename, "READ")
		inclusive = f.Get("hist%d" % hist_index)
		hist_index += 1
		for i in xrange(1,11):
			lt_hists.append(f.Get("hist%d" % hist_index ))
			hist_index += 1
			print lt_hists[-1].GetTitle()
			lt_sum += lt_hists[-1].Integral()
	else:
		inclusive = plot.GetTH1(inc_chain, branchname, isMC, binning, category, label="Inclusive Madgraph", usePU = False, useEleIDSF=nele)

		for name, chain in zip(lt_names,lt_binned_chains):
			print "Getting", name
			h = plot.GetTH1(chain, branchname, isMC, binning, category, label= "L_{T} " + name + " GeV", usePU = False, useEleIDSF=nele)
			lt_hists.append(h)
			lt_sum += h.Integral()

	if remake:
		f = ROOT.TFile.Open("tmp/LT_%s.root" % filename, "RECREATE")
		for h in lt_hists + [inclusive]:
			h.Write()
		f.Close()

	plot.ColorData(inclusive)
	plot.ColorMCs(lt_hists)
	#inclusive.Scale(1./inclusive.Integral())

	rajdeep_new = [828.72, 129.496, 155.178, 217.287, 175.158, 50.51, 91.48, 3.69, 0.196, 0.0072]
	old_xs = [8.670e+02, 1.345e+02, 1.599e+02, 2.295e+02, 1.654e+02, 4.896e+01, 9.401e+01, 3.588e+00, 2.012e-01, 8.329e-03]
	fit_new = [ 9.93694e-01, 9.72633e-01, 9.90570e-01, 9.89391e-01, 9.92841e-01, 8.56845e-01, 8.91049e-01, 9.04437e-01, 6.09500e-01,1] 
	filename = "before_" + filename
	#for i, h in enumerate(lt_hists):
		#print "### XS RATIO: ", h.GetTitle(), old_xs[i], old_xs[i] * fit_new[i]
		#h.Scale(fit_new[i])
		#h.Scale(rajdeep_new[i]/old_xs[i])
	#	h.Scale(1./lt_sum)
	#inclusive.Scale(1000*4.957e+03/49144274.0)
	inclusive.Scale(1000*5147.39/49144274.0)
	
	lt_hists.reverse()
	plot.PlotDataMC(inclusive, lt_hists, "plots/validation/LTBinned/", "LT_correctXS_comparison_" + filename, xlabel=xlabel + " [GeV]", ylabel="Fraction", ylabel_unit="GeV", stack_mc=True, ratio=ratio, logx=logx)

	#data_h = plot.GetTH1(data_chain, branchname, False, binning, category, label="Data OldRegr", usePU = False)
	#lt_binned_all_h = plot.GetTH1(lt_binned_all, branchname, isMC, binning, category, label="LTBinned Madgraph", usePU = False, useEleIDSF=nele)

	#mc_hists = [lt_binned_all_h, inclusive]
	#plot.ColorData(data_h)
	#plot.ColorMCs(mc_hists)
	#plot.Normalize(data_h, mc_hists)
	#plot.PlotDataMC(data_h, mc_hists, "plots/validation/LTBinned/", "LT_dataMC_" + filename, xlabel=xlabel + " [GeV]", ylabel="Events", ylabel_unit="GeV", ratio=ratio, logx=logx)

branchname = "invMass_ECAL_ele"
filename = "invMass_ECAL_ele"
binning = "(100,80,100)"
xlabel  = "M_{ee}"
comp(branchname, filename, binning, xlabel, (.9,1.1), 2)

branchname = "energy_ECAL_ele[0]/cosh(etaSCEle[0]) + energy_ECAL_ele[1]/cosh(etaSCEle[1])"
filename = "LT"
binning = "(100,40,200)"
xlabel  = "L_{T}"
comp(branchname, filename, binning, xlabel, (.8,1.2), 2)

filename = "LT_wide"
binning = "(350,50,400)"
comp(branchname, filename, binning, xlabel, (.8,1.2), 2, logx=True)

branchname = "energy_ECAL_ele/cosh(etaSCEle)"
filename = "ET"
binning = "(100,20,80)"
xlabel  = "E_{T}"
comp(branchname, filename, binning, xlabel, (.8, 1.2), 1)

filename = "ET_wide"
binning = "(100,40,300)"
comp(branchname, filename, binning, xlabel, (.8,1.2), 1, logx=True)

branchname = "etaSCEle"
filename = "eta"
binning = "(100,-2.5,2.5)"
xlabel  = "#eta_{SC}"
comp(branchname, filename, binning, xlabel, True, 1)

branchname = "R9Ele"
filename = "R9"
binning = "(100,.3,1)"
xlabel  = "R_{9}"
comp(branchname, filename, binning, xlabel, True, 1)
