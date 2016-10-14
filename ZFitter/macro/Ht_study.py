import ROOT
file_Et_MC  =ROOT.TFile('Cat_histos/Et_hist_30fb_lowEta_highR9.root','READ')
Et_hist_MC  =file_Et_MC.Get("Et_hist_MC")

print Et_hist_MC.Integral()
X     =Et_hist_MC.ProfileX()

x_size=Et_hist_MC.ProfileX().GetSize() -1
#print "Y size ", Et_hist_MC.ProfileY().GetSize() -1

for i in range(1,x_size):
    print Et_hist_MC




