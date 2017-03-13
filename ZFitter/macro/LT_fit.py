import ROOT

f = ROOT.TFile.Open("tmp/LT_LT.root")

inc = f.Get("hist0")
inc.Scale(1000*4.957e+03/49144274.0)

lt_hists = [f.Get("hist%d" % i ) for i in range(1,10)]

def myfit(x, p):
	ret = 0
	for h,scale in zip(lt_hists,p):
		ret += h.GetBinContent( h.FindBin(x[0])) * scale
	return ret


fit = ROOT.TF1("myfit", myfit, 50, 500, 10)
fit.SetParameters(*([1]*len(lt_hists)))

ROOT.gROOT.SetBatch(True)

c = ROOT.TCanvas()

inc.Fit("myfit", "N")

inc.Draw()
fit.Draw("same")

c.SetLogx()
c.SetLogy()
c.SaveAs("plots/LT_fit.png")

stack = ROOT.THStack()
for h,scale in zip(lt_hists, fit.GetParameters()):
	h.Scale(scale)
	stack.Add(h)

res = inc.Clone()
res.Add(stack.GetStack().Last(), -1)
res.Divide(inc)
res.SetMaximum(.2)
res.SetMinimum(-.2)
#res.GetXaxis().SetRangeUser(50,150)
res.Draw()

c.SetLogx(False)
c.SetLogy(False)
c.SaveAs("plots/LT_fit_res.png")

