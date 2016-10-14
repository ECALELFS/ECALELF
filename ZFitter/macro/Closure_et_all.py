from ROOT import *
import ROOT
import numpy as np
import os
#os.system("cd ~")
#os.system("root -q compile_logon.C")
#os.system("cd -")
gROOT.SetBatch(kTRUE)
gSystem.Load("~/rootlogon_C.so")
rootlogon()

from optparse import OptionParser
parser=OptionParser()
parser.add_option("-t","--type",dest="type",default="scale") 
(options,args)=parser.parse_args()

configfile = 'Closure_et_files/'
print "[INFO] configfile is ",configfile
import os
import sys
sys.path.append(os.path.dirname(os.path.expanduser(configfile)))
import barrel_0T_scale 
import barrel_38T_scale 
import endcap_0T_scale 
import endcap_38T_scale 

import barrel_0T_smearing
import barrel_38T_smearing
import endcap_0T_smearing
import endcap_38T_smearing


c1=TCanvas()
x_dummy =[3.,9.,15.]
x_dummy_barrel_38T =[1.,7.,13.]
x_dummy_barrel_0 =[2.,8.,14.]
x_dummy_endcap_38T =[4.,10.,16.]
x_dummy_endcap_0 =[5.,11.,17.]


if(options.type=="scale"):
    offset_barrel_0T=(1-barrel_0T_scale.y[0])*100
    offset_endcap_0T=(1-endcap_0T_scale.y[0])*100
    for i in range(0,len(x_dummy)):
        barrel_38T_scale.y[i]=(1- barrel_38T_scale.y[i])*100
        barrel_38T_scale.err_down[i]=barrel_38T_scale.err_down[i]*100
        barrel_38T_scale.err_up[i]=barrel_38T_scale.err_up[i]*100
        endcap_38T_scale.y[i]=(1- endcap_38T_scale.y[i])*100
        endcap_38T_scale.err_down[i]=endcap_38T_scale.err_down[i]*100
        endcap_38T_scale.err_up[i]=endcap_38T_scale.err_up[i]*100

        barrel_0T_scale.y[i]=(1- barrel_0T_scale.y[i])*100 - offset_barrel_0T
        barrel_0T_scale.err_down[i]=barrel_0T_scale.err_down[i]*100
        barrel_0T_scale.err_up[i]=barrel_0T_scale.err_up[i]*100
        endcap_0T_scale.y[i]=(1- endcap_0T_scale.y[i])*100 - offset_endcap_0T
        endcap_0T_scale.err_down[i]=endcap_0T_scale.err_down[i]*100
        endcap_0T_scale.err_up[i]=endcap_0T_scale.err_up[i]*100

else: #smearing
    for i in range(0,len(x_dummy)):
        barrel_38T_smearing.y[i]=(barrel_38T_smearing.y[i])*100
        barrel_38T_smearing.err_down[i]=barrel_38T_smearing.err_down[i]*100
        barrel_38T_smearing.err_up[i]=barrel_38T_smearing.err_up[i]*100
        endcap_38T_smearing.y[i]=(endcap_38T_smearing.y[i])*100
        endcap_38T_smearing.err_down[i]=endcap_38T_smearing.err_down[i]*100
        endcap_38T_smearing.err_up[i]=endcap_38T_smearing.err_up[i]*100

        barrel_0T_smearing.y[i]=(barrel_0T_smearing.y[i])*100
        barrel_0T_smearing.err_down[i]=barrel_0T_smearing.err_down[i]*100
        barrel_0T_smearing.err_up[i]=barrel_0T_smearing.err_up[i]*100
        endcap_0T_smearing.y[i]=(endcap_0T_smearing.y[i])*100
        endcap_0T_smearing.err_down[i]=endcap_0T_smearing.err_down[i]*100
        endcap_0T_smearing.err_up[i]=endcap_0T_smearing.err_up[i]*100

#Set x axis label via h_dummy
###########################

h_dummy=TH1F("h_dummy","h_dummy",18,0,18)
h_dummy.GetXaxis().SetNdivisions(3,0)
if(options.type=="scale"):
    h_dummy.SetMinimum(-1.5)
    h_dummy.SetMaximum(2.2)
elif(options.type=="smearing"):
    h_dummy.SetMinimum(0.)
    h_dummy.SetMaximum(4.2)

#for i in range(0,len(barrel_38T_scale.labels)):
#    h_dummy.GetXaxis().SetBinLabel(i+1,"#splitline{EB: "+barrel_38T_scale.labels[i]+ "}{EE: "+barrel_0T_scale.labels[i]+"}")
if(options.type=='scale'):
    h_dummy.GetYaxis().SetTitle("Data energy scale [%]")
else:
    h_dummy.GetYaxis().SetTitle("MC energy smearing [%]")
h_dummy.GetXaxis().SetLabelSize(0)
h_dummy.Draw()
err_dummy=[0.,0.,0.]

if(options.type=="scale"):
    gr_barrel_38=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_barrel_38T),np.asarray(barrel_38T_scale.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(barrel_38T_scale.err_down),np.asarray(barrel_38T_scale.err_up))
    gr_endcap_38=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_endcap_38T),np.asarray(endcap_38T_scale.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(endcap_38T_scale.err_down),np.asarray(endcap_38T_scale.err_up))
    gr_barrel_0=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_barrel_0),np.asarray(barrel_0T_scale.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(barrel_0T_scale.err_down),np.asarray(barrel_0T_scale.err_up))
    gr_endcap_0=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_endcap_0),np.asarray(endcap_0T_scale.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(endcap_0T_scale.err_down),np.asarray(endcap_0T_scale.err_up))
else:
    gr_barrel_38=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_barrel_38T),np.asarray(barrel_38T_smearing.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(barrel_38T_smearing.err_down),np.asarray(barrel_38T_smearing.err_up))
    gr_endcap_38=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_endcap_38T),np.asarray(endcap_38T_smearing.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(endcap_38T_smearing.err_down),np.asarray(endcap_38T_smearing.err_up))
    gr_barrel_0=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_barrel_0),np.asarray(barrel_0T_smearing.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(barrel_0T_smearing.err_down),np.asarray(barrel_0T_smearing.err_up))
    gr_endcap_0=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy_endcap_0),np.asarray(endcap_0T_smearing.y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(endcap_0T_smearing.err_down),np.asarray(endcap_0T_smearing.err_up))

gr_barrel_38.SetMarkerSize(1.5)
gr_barrel_38.SetMarkerStyle(20)
gr_barrel_38.SetMarkerColor(kBlue)
gr_barrel_38.SetLineColor(kBlue)
gr_endcap_38.SetMarkerSize(1.5)
gr_endcap_38.SetMarkerStyle(20)
gr_endcap_38.SetMarkerColor(kRed)
gr_endcap_38.SetLineColor(kRed)
gr_barrel_0.SetMarkerSize(1.5)
gr_barrel_0.SetMarkerStyle(kOpenSquare)
gr_barrel_0.SetMarkerColor(kBlue) 
gr_barrel_0.SetLineColor(kBlue)
gr_endcap_0.SetMarkerSize(1.5)
gr_endcap_0.SetMarkerStyle(kOpenSquare)
gr_endcap_0.SetMarkerColor(kRed)
gr_endcap_0.SetLineColor(kRed) 

if(options.type=="scale"):
    box=TBox(0.1,-1,18,1)
    #shadedYellow=TColor.GetColorTransparent(kYellow,0.35)
    #box.SetFillColor(shadedYellow)
    box.SetFillColor(kYellow-9)
    box.SetFillStyle(3001)
    box.SetLineColor(kYellow-9)
    #box.SetFillColor(12)
    #box.SetFillStyle(3004)
    box.Draw("same")

if(options.type=="scale"):
    line=TLine(0,0,18,0)
    line.SetLineWidth(2)
    line.Draw("same")
gr_barrel_38.GetXaxis().SetLimits(0,18)
gr_barrel_38.Draw("Psame")
gr_endcap_38.Draw("Psame")
gr_barrel_0.Draw("Psame")
gr_endcap_0.Draw("Psame")

#if(options.type=="scale"):
leg=TLegend(0.3,0.7,0.99,0.9)
#else:
#    leg=TLegend(0.6,0.2,0.8,0.3)

leg.SetFillStyle(0)
leg.SetBorderSize(0)
if(options.type=="scale"):
    leg.AddEntry(box ,"syst. uncertainty","f")
leg.AddEntry(gr_barrel_38,"EB 3.8T","p")
leg.AddEntry(gr_barrel_0 ,"EB 0T","p")
leg.AddEntry(gr_endcap_38,"EE 3.8T","p")
leg.AddEntry(gr_endcap_0 ,"EE 0T","p")
leg.SetTextSize(0.04)
leg.Draw()

low_barrel =ROOT.TLatex(0.17,0.2,"EB: E_{T}< 60 GeV")
low_barrel.SetNDC()
low_barrel.SetTextSize(0.025)
low_barrel.Draw()
low_endcap =ROOT.TLatex(0.17,0.15,"EE: E_{T}< 50 GeV")
low_endcap.SetNDC()
low_endcap.SetTextSize(0.025)
low_endcap.Draw()

medium_barrel =ROOT.TLatex(0.37,0.2,"EB: 60 GeV <E_{T}< 100 GeV")
medium_barrel.SetNDC()
medium_barrel.SetTextSize(0.025)
medium_barrel.Draw()
medium_endcap =ROOT.TLatex(0.37,0.15,"EE: 50 GeV <E_{T}< 85 GeV")
medium_endcap.SetNDC()
medium_endcap.SetTextSize(0.025)
medium_endcap.Draw()

high_barrel =ROOT.TLatex(0.63,0.2,"EB: E_{T}> 100 GeV")
high_barrel.SetNDC()
high_barrel.SetTextSize(0.025)
high_barrel.Draw()
high_endcap =ROOT.TLatex(0.63,0.15,"EE: E_{T}> 85 GeV")
high_endcap.SetNDC()
high_endcap.SetTextSize(0.025)
high_endcap.Draw()


c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/summary_"+options.type+".png")
c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/summary_"+options.type+".pdf")
c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/Closure_et/summary_"+options.type+".C")


