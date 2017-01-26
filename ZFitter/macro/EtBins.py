import sys
import ROOT

filename = sys.argv[1]
n_events_per_bin = float(sys.argv[2])

f = ROOT.TFile.Open(filename)
et_h = f.Get("hist0")

xbins = et_h.GetXaxis().GetNbins()
ybins = et_h.GetYaxis().GetNbins()

i = 1
et_bins = [i]
et_values = [et_h.GetXaxis().GetBinLowEdge(i)]
while i <= xbins and i <= ybins:
	integral = et_h.Integral(et_bins[-1], i, et_bins[-1], i)
	if integral > n_events_per_bin:
		et_bins.append(i)
		et_values.append(et_h.GetXaxis().GetBinLowEdge(i))
	i += 1
et_bins.append(i)
et_values.append(et_h.GetXaxis().GetBinLowEdge(i))
print et_values

c = ROOT.TCanvas("c","c")

et_h.SetStats()
ROOT.gStyle.SetOptStat(1111111)
et_h.Draw("colz")
et_h.SetXTitle("E_{T1}")
et_h.SetYTitle("E_{T2}")

boxes = []
for i in xrange(len(et_values)-1):
	boxes.append(ROOT.TBox( et_values[i], et_values[i], et_values[i+1], et_values[i+1]))

for b in boxes:
	b.SetFillStyle(0)
	b.SetLineColor(ROOT.kRed)
	b.Draw()

c.SaveAs("plots/ET_diag.png")
