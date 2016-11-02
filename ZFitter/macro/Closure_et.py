from ROOT import *
import numpy as np
gROOT.SetBatch(kTRUE)
gSystem.Load("~/rootlogon_C.so")
rootlogon()

from optparse import OptionParser
parser=OptionParser()
parser.add_option("-r","--region",dest="region",default="barrel")
parser.add_option("-b","--magnet",dest="magnet",default="38T")  
parser.add_option("-t","--type",dest="type",default="scale") 
(options,args)=parser.parse_args()

configfile = 'Closure_et_files/'+options.region+"_"+options.magnet+"_"+options.type+".py"
print "[INFO] configfile is ",configfile
import os
import sys
sys.path.append(os.path.dirname(os.path.expanduser(configfile)))
if(options.region+"_"+options.magnet+"_"+options.type=="barrel_0T_scale"):
    from barrel_0T_scale import *
elif(options.region+"_"+options.magnet+"_"+options.type=="barrel_0T_smearing"):
    from barrel_0T_smearing import *
if(options.region+"_"+options.magnet+"_"+options.type=="endcap_0T_scale"):
    from endcap_0T_scale import *
elif(options.region+"_"+options.magnet+"_"+options.type=="endcap_0T_smearing"):
    from endcap_0T_smearing import *
if(options.region+"_"+options.magnet+"_"+options.type=="barrel_38T_scale"):
    from barrel_38T_scale import *
elif(options.region+"_"+options.magnet+"_"+options.type=="barrel_38T_smearing"):
    from barrel_38T_smearing import *
if(options.region+"_"+options.magnet+"_"+options.type=="endcap_38T_scale"):
    from endcap_38T_scale import *
elif(options.region+"_"+options.magnet+"_"+options.type=="endcap_38T_smearing"):
    from endcap_38T_smearing import *

c1=TCanvas()
x_dummy =[0.5,1.5,2.5]

if(options.type=="scale"):
    for i in range(0,len(y)):
        y[i]=(1-y[i])*100
        err_down[i]=err_down[i]*100
        err_up[i]=err_up[i]*100
else: #smearing
    for i in range(0,len(y)):
        y[i]=(y[i])*100
        err_down[i]=err_down[i]*100
        err_up[i]=err_up[i]*100

#Set x axis label via h_dummy
h_dummy=TH1F("h_dummy","h_dummy",3,0,3)
if(options.type=="scale"):
    h_dummy.SetMinimum(-2.)
    h_dummy.SetMaximum(2)
elif(options.type=="smearing"):
    h_dummy.SetMinimum(0.)
    h_dummy.SetMaximum(2.5)

if(options.region+"_"+options.magnet+"_"+options.type=="endcap_38T_smearing"):
    h_dummy.SetMaximum(3.)

for i in range(0,len(labels)):
    h_dummy.GetXaxis().SetBinLabel(i+1,labels[i])
if(options.type=='scale'):
    h_dummy.GetYaxis().SetTitle("Data energy scale [%]")
else:
    h_dummy.GetYaxis().SetTitle("MC energy smearing [%]")
h_dummy.Draw()
err_dummy=[0.,0.,0.]

gr=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy),np.asarray(y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(err_down),np.asarray(err_up))
gr.SetMarkerStyle(20)
if(options.region=="barrel"):
    gr.SetMarkerColor(kBlue)
    gr.SetLineColor(kBlue)
else:
    gr.SetMarkerColor(kRed)
    gr.SetLineColor(kRed)
gr.Draw("Psame")

if(options.type=="scale"):
    leg=TLegend(0.6,0.8,0.8,0.9)
else:
    leg=TLegend(0.6,0.2,0.8,0.3)

leg.SetBorderSize(0)
if(options.region=="barrel"):
    label_region="EB"
else:
    label_region="EE"

if(options.magnet=="38T"):
    label_field=" (B= 3.8 T)"
else:
    label_field=" (B= 0 T)"

leg.AddEntry(gr,label_region+label_field,"pe")
leg.SetTextSize(0.03)
leg.Draw()
c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/"+options.region+"_"+options.magnet+"_"+options.type+".png")
c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/"+options.region+"_"+options.magnet+"_"+options.type+".C")
c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/"+options.region+"_"+options.magnet+"_"+options.type+".pdf")

##write down corr for tex
file_out = open("Closure_et_files/"+options.region+"_"+options.magnet+"_"+options.type+".tex",'w+')
for i in range(0,len(y)):
    file_out.write("$%s$ & %.3lf & %.3lf\n"%(labels[i],y[i],(err_up[i]+err_down[i])/2))
