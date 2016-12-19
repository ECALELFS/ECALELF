import ROOT
#0 -> lowEta lowR9
#4 -> lowEta highR9
#7 highEta lowR9
#9 highEta highR9
file_Et_data=ROOT.TFile('Et_hist_smearerCat4_random_data.root','READ')
file_Et_MC  =ROOT.TFile('Et_hist_smearerCat4_random_MC.root','READ')
Et_hist_data=file_Et_data.Get("Et_hist_data_4")
Et_hist_MC  =file_Et_MC.Get("Et_hist_MC_4")
#print Et_hist_data.Integral()
print Et_hist_MC.Integral()
Et_min_bins=[]
Et_max_bins=[]
Et_min_cuts=[]
Et_max_cuts=[]

Et_min=1
Et_max=1

X     =Et_hist_data.ProfileX()
X_size=Et_hist_data.ProfileX().GetSize() -1

###Bin finding via data
bin_counter=1
while(Et_max< X_size):
    #if bin_counter<4:#i primi 3 bin li fai ad alta statistica, poi abbassi i criteri 
    n_min=500000 #150000
    #elif bin_counter<5:
    #    n_min=3000
    #else:
    #    n_min=1000
    
    while(Et_hist_MC.Integral(Et_min,Et_max,Et_min,Et_max)<n_min and Et_max<X_size):
        Et_max+=1
    Et_min_bins.append(Et_min)
    Et_max_bins.append(Et_max)
    Et_min_cuts.append(Et_min*X.GetBinWidth(1))
    Et_max_cuts.append(Et_max*X.GetBinWidth(1))
    Et_min=Et_max
    bin_counter+=1

print "Et_min_list ",Et_min_cuts
print "Et_max_list ",Et_max_cuts
nbins=len(Et_max_bins)
print "You defined ",len(Et_max_bins)," bins"
print "Et_min ", Et_min_bins[nbins -1]
print "Et_max ", Et_max_bins[nbins -1]
print "######Data"
for i in range(0,len(Et_min_cuts)):
    print "Bin ",i," Stat: ",Et_hist_data.Integral(Et_min_bins[i], Et_max_bins[i],Et_min_bins[i], Et_max_bins[i])
print "######Data"

print "######MC"
for i in range(0,len(Et_min_cuts)):
    print "Bin ",i," Stat: ",Et_hist_MC.Integral(Et_min_bins[i], Et_max_bins[i],Et_min_bins[i], Et_max_bins[i])
print "######MC"

