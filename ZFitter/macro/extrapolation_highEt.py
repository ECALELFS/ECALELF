from ROOT import *
import numpy as np
gROOT.SetBatch(kTRUE)
gSystem.Load("~/rootlogon_C.so")
rootlogon()

#from optparse import OptionParser
#parser=OptionParser()
#parser.add_option("-r","--region",dest="region",default="barrel")
#parser.add_option("-b","--magnet",dest="magnet",default="38T")  
#parser.add_option("-t","--type",dest="type",default="scale") 
#(options,args)=parser.parse_args()
#
#configfile = 'Closure_et_files/'+options.region+"_"+options.magnet+"_"+options.type+".py"
#print "[INFO] configfile is ",configfile
#import os
#import sys
#sys.path.append(os.path.dirname(os.path.expanduser(configfile)))
#if(options.region+"_"+options.magnet+"_"+options.type=="barrel_0T_scale"):
#    from barrel_0T_scale import *
#elif(options.region+"_"+options.magnet+"_"+options.type=="barrel_0T_smearing"):
#    from barrel_0T_smearing import *
#if(options.region+"_"+options.magnet+"_"+options.type=="endcap_0T_scale"):
#    from endcap_0T_scale import *
#elif(options.region+"_"+options.magnet+"_"+options.type=="endcap_0T_smearing"):
#    from endcap_0T_smearing import *
#if(options.region+"_"+options.magnet+"_"+options.type=="barrel_38T_scale"):
#    from barrel_38T_scale import *
#elif(options.region+"_"+options.magnet+"_"+options.type=="barrel_38T_smearing"):
#    from barrel_38T_smearing import *
#if(options.region+"_"+options.magnet+"_"+options.type=="endcap_38T_scale"):
#    from endcap_38T_scale import *
#elif(options.region+"_"+options.magnet+"_"+options.type=="endcap_38T_smearing"):
#    from endcap_38T_smearing import *

c1=TCanvas()
x_dummy =[40.,65.,100., 140.]
#y       =[0.008,-0.165,-0.32]
#err_down=[0.004,0.02,0.05]
#err_up  =[0.004,0.02,0.05]
#err_dummy=[0.,0.,0.]

y       =[0.045,-0.084,-0.3,-0.44]
err_down=[0.0,0.,0.,0.]
err_up=[0.0,0.0,0.0,0.0]
#err_down=[0.001,0.006,0.05,0.05]
#err_up=[0.001,0.006,0.05,0.05]
#err_up  =[0.004,0.02,0.05]
err_dummy=[0.,0.,0.,0.]

#if(options.type=="scale"):
#for i in range(0,len(y)):
#        y[i]=(1-y[i])*100
#        err_down[i]=err_down[i]*100
#        err_up[i]=err_up[i]*100
#else: #smearing
#    for i in range(0,len(y)):
#        y[i]=(y[i])*100
#        err_down[i]=err_down[i]*100
#        err_up[i]=err_up[i]*100

gr=TGraphAsymmErrors(len(x_dummy),np.asarray(x_dummy),np.asarray(y),np.asarray(err_dummy),np.asarray(err_dummy),np.asarray(err_down),np.asarray(err_up))
gr.GetXaxis().SetLimits(0.,150.)
f=TF1("fit_func","pol1",35.,150.)
#gr.Fit(f,"OFR+")
gr.SetMinimum(-1.)
gr.SetMarkerStyle(20)
#gr.SetMinimum(0.)
#if(options.region=="barrel"):
#    gr.SetMarkerColor(kBlue)
#    gr.SetLineColor(kBlue)
#else:
#    gr.SetMarkerColor(kRed)
#    gr.SetLineColor(kRed)

gr.GetXaxis().SetTitle("E_{T} [GeV]")
gr.GetYaxis().SetTitle("Residual data energy shift [%]")
gr.Draw("APsame")

#leg.AddEntry(gr,label_region+label_field,"pe")
#leg.SetTextSize(0.03)
#leg.Draw()

c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/extrapolation_et/stochastic_check.png")
#c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/extrapolation_et/"+options.region+"_"+options.magnet+"_"+options.type+".png")
#c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/extrapolation_et/"+options.region+"_"+options.magnet+"_"+options.type+".C")
#c1.SaveAs("~/scratch1/www/RUN2_ECAL_Calibration/extrapolation_et/"+options.region+"_"+options.magnet+"_"+options.type+".pdf")

##write down corr for tex
#file_out = open("Closure_et_files/"+options.region+"_"+options.magnet+"_"+options.type+".tex",'w+')
#for i in range(0,len(y)):
#    file_out.write("$%s$ & %.3lf & %.3lf\n"%(labels[i],y[i],(err_up[i]+err_down[i])/2))
