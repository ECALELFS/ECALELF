#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess

from optparse import OptionParser

parser = OptionParser()

parser.add_option('--generateOnly',      action='store_true', dest='generateOnly',      default=False, help='generate jobs only, without submitting them')

(options, args) = parser.parse_args()

#ntupleName = "data-DoubleEG-Run2016BCD_ZSkim_12p9fb_multifit"
ntupleName = "Run2016BCDEFG_WZSkim_Cal_Sep2016_final_v3_multifit"
#ntupleName = "data-SingleElectron-Run2016BCD_WSkim_12p9fb_weights"
#options: "data-Run2015D-25ns-multifit", "data-Run2015D-25ns-multifit" or "data-Run2015D-25ns-stream".
folder = "/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"
categories = ["EB","EB_0_1","EB_1_1479","EE","EE_1479_2","EE_2_25","EEp","EEp_1479_2","EEp_2_25","EEm","EEm_1479_2","EEm_2_25","EEp_2_225","EEp_225_25","EEm_2_225","EEm_225_25"]
yMIN = ["0.85","0.85","0.85","0.65","0.65","0.40","0.65","0.65","0.40","0.65","0.65","0.40","0.40","0.40","0.40","0.40"]
events = ["20000","20000","20000","10000","10000","10000","10000","10000","10000","10000","10000","10000","10000","10000","10000","10000"]
#yMIN = ["0.95","0.95","0.95","0.95","0.95","0.95","0.95","0.95","0.95","0.95","0.95","0.95"]
#categories = ["EEp","EEp_1479_2","EEp_2_25","EEm","EEm_1479_2","EEm_2_25"]
#yMIN = ["0.65","0.65","0.40","0.65","0.65","0.40"]
#ntuplelist = ["data-SingleElectron-Run2016BCD_WSkim_12p9fb_weights"]
#ntuplelist = ["data-DoubleEG-Run2016BCD_ZSkim_12p9fb_multifit"]
#ntuplelist = ["data-SingleElectron-Run2016BCD_WSkim_12p9fb_multifit","data-DoubleEG-Run2016BCD_ZSkim_12p9fb_multifit"]
#ntuplelist = ["SingleElectron-Run2016B-WSkim-Prompt_v2-273150-275376","SingleElectron-Run2016C-WSkim-Prompt_v2-275420-276283","SingleElectron-Run2016D-WSkim-Prompt_v2-276315-276811","SingleElectron-Run2016E-WSkim-Prompt_v1-276830-277420","SingleElectron-Run2016F-WSkim-Prompt_v1-277820-278808","SingleElectron-Run2016G-WSkim-Prompt_v1-278817-279488"]
#ntuplelist=["DoubleEG-Run2016B-ZSkim-Prompt_v2-273150-275376.root","DoubleEG-Run2016C-ZSkim-Prompt_v2-275420-276283.root","DoubleEG-Run2016D-ZSkim-Prompt_v2-276315-276811.root",
#"DoubleEG-Run2016E-ZSkim-Prompt-v2-276830-277420.root","DoubleEG-Run2016F-ZSkim-Prompt-v1-277820-278808.root","DoubleEG-Run2016G-ZSkim-Prompt-v1-278817-279588.root","DoubleEG-Run2016G-ZSkim-Prompt-v1-279589-279931.root"]

ntuplelist = [
#"DoubleEG-Run2016B-ZSkim-Prompt_v2-273150-275376",
#"DoubleEG-Run2016C-ZSkim-Prompt_v2-275420-276283",
#"DoubleEG-Run2016D-ZSkim-Prompt_v2-276315-276811",
#"DoubleEG-Run2016E-ZSkim-Prompt-v2-276830-277420",
#"DoubleEG-Run2016F-ZSkim-Prompt-v1-277820-278808",
#"DoubleEG-Run2016G-ZSkim-Prompt-v1-278817-279588",
#"DoubleEG-Run2016G-ZSkim-Prompt-v1-279589-279931",
"SingleElectron-Run2016B-WSkim-Prompt_v2-273150-275376",
"SingleElectron-Run2016C-WSkim-Prompt_v2-275420-276283",
"SingleElectron-Run2016D-WSkim-Prompt_v2-276315-276811",
"SingleElectron-Run2016E-WSkim-Prompt-v2-276830-277420",
"SingleElectron-Run2016F-WSkim-Prompt-v1-277820-278808",
"SingleElectron-Run2016G-WSkim-Prompt-v1-278817-279588",
"SingleElectron-Run2016G-WSkim-Prompt-v1-279589-279931"
]

folderlist=[
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016B-ZSkim-Prompt_v2/273150-275376/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016C-ZSkim-Prompt_v2/275420-276283/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016D-ZSkim-Prompt_v2/276315-276811/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016E-ZSkim-Prompt-v2/276830-277420/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016F-ZSkim-Prompt-v1/277820-278808/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1/278817-279588/271036_279931-Cal_Sep2016/withExtras2/",
#"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1/279589-279931/271036_279931-Cal_Sep2016/withExtras2/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016B-WSkim-Prompt_v2/273150-275376/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016C-WSkim-Prompt_v2/275420-276283/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016D-WSkim-Prompt_v2/276315-276811/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016E-WSkim-Prompt-v2/276830-277420/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016F-WSkim-Prompt-v1/277820-278808/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016G-WSkim-Prompt-v1/278817-279588/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016G-WSkim-Prompt-v1/279589-279931/271036_279931-Cal_Sep2016/withExtras/"
]


#folderlist=["/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"]
#folderlist=["/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016B-WSkim-Prompt_v2/273150-275376/271036_276811-ICHEP/withExtrasTest/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016C-WSkim-Prompt_v2/275420-276283/271036_276811-ICHEP/withExtrasTest/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016D-WSkim-Prompt_v2/276315-276811/271036_276811-ICHEP/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016E-WSkim-Prompt_v1/276830-277420/276830-277420/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016F-WSkim-Prompt_v1/277820-278808/271036-278808/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/80X_dataRun2_Prompt_v10/SingleElectron-Run2016G-WSkim-Prompt_v1/278817-279488/271036-279116/"]

#folderlist=["/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016B-ZSkim-Prompt_v2/273150-275376/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016C-ZSkim-Prompt_v2/275420-276283/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016D-ZSkim-Prompt_v2/276315-276811/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016E-ZSkim-Prompt-v2/276830-277420/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016F-ZSkim-Prompt-v1/277820-278808/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1/278817-279588/271036_279931-Cal_Sep2016/withExtras2/","/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1/279589-279931/271036_279931-Cal_Sep2016/withExtras2/"]


currentDir = os.getcwd();
CMSSWDir = currentDir+"/../";

os.system("mkdir Job_monitoring_"+ntupleName)

for i in range(len(categories)):
    fn = "Job_monitoring_"+ntupleName+"/Job_"+categories[i];

    outScript = open(fn+".sh","w");
    command = "ZFitter.exe -f EoverPmonitoring_batch_"+ntupleName+".dat --evtsPerPoint 200000 --laserMonitoringEP --EBEE "+categories[i]+" --yMIN 0.95 --yMAX 1.05 --LUMI 24.1 --dayMin 15-03-2016 --dayMax 15-09-2016 --EoPvsPU 1" #use this if you want to run vs Nvtx
#    command = "ZFitter.exe -f EoverPmonitoring_batch_"+ntupleName+".dat --evtsPerPoint 200000 --laserMonitoringEP --EBEE "+categories[i]+" --yMIN 0.8 --yMAX 1.20 --LUMI 24.1 --dayMin 15-03-2016 --dayMax 15-09-2016" #use this if you want to run E/p vs LC
#    command = "ZFitter.exe -f EoverPmonitoring_batch_"+ntupleName+".dat --evtsPerPoint "+events[i]+" --laserMonitoringEP --EBEE "+categories[i]+" --yMIN "+yMIN[i]+" --yMAX 1.15 --LUMI 24.1 --dayMin 15-03-2016 --dayMax 15-09-2016" #default, to be run for E/p vs time
    print command;
    outScript.write('#!/bin/bash');
    outScript.write("\n"+'cd '+CMSSWDir);
    outScript.write("\n"+'eval `scram runtime -sh`');
    outScript.write("\n"+'cd -');
    outScript.write("\necho $PWD");
    outScript.write("mkdir "+categories[i]+"__");
    outScript.write("\nll");

    for j in range(len(ntuplelist)):
        outScript.write("\necho \"copy main tree\" ");
        outScript.write("\ncmsStage "+folderlist[j]+""+ntuplelist[j]+".root ./")
#        outScript.write("\ncmsStage "+folderlist[j]+""+ntuplelist[j]+" ./")
#        outScript.write("\necho \"copy extracalib tree\" ");
#        outScript.write("\ncmsStage "+folderlist[j]+"extraCalibTree-"+ntuplelist[j]+".root ./")

    outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
    outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EB_pTk.root ./")
    outScript.write("\necho \"copia14\" ");
    outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EB_scE.root ./")
    outScript.write("\necho \"copia15\" ");
    outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EE_pTk.root ./")
    outScript.write("\necho \"copia14\" ");
    outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EE_scE.root ./")
    outScript.write("\necho \"copia15\" ");
    outScript.write("\ncmsStage "+folder+"EoverPmonitoring_batch_"+ntupleName+".dat ./")
    outScript.write("\necho \"fine copia\" ");
    
    outScript.write("\nls")
    outScript.write("\necho \"eseguo: "+command+"\" ")
    outScript.write("\n"+command);
    outScript.write("\nls")
    outScript.write("\ncp -v -r "+categories[i]+"__/ "+currentDir+"/"+categories[i]+"_"+ntupleName+"/")
    outScript.close();
    os.system("chmod 777 "+currentDir+"/"+fn+".sh");
    command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
    if not options.generateOnly:
        os.system(command2);
    print command2






'''
fn = "Job_monitoring_"+ntupleName+"/Job_"+"EE";
outScript = open(fn+".sh","w");
command = "ZFitter.exe -f EoverPcalibration_batch_"+ntupleName+".dat --evtsPerPoint 50000 --laserMonitoringEPvsPU --EBEE EE --yMIN 0.6 --yMAX 1.15 --LUMI 12.9 --dayMin 15-03-2016 --dayMax 01-08-2016"
print command;
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+CMSSWDir);
outScript.write("\n"+'eval `scram runtime -sh`');
outScript.write("\n"+'cd -');
outScript.write("\necho $PWD");
outScript.write("mkdir EE__");
outScript.write("\nll");


outScript.write("\necho \"copia1\" ");
outScript.write("\ncmsStage "+folder+ntupleName+".root ./")
outScript.write("\necho \"copia2\" ");
outScript.write("\ncmsStage "+folder+"extraCalibTree-"+ntupleName+".root ./")

outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EE_pTk.root ./")
outScript.write("\necho \"copia14\" ");
outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EE_scE.root ./")
outScript.write("\necho \"copia15\" ");
outScript.write("\ncmsStage "+folder+"EoverPcalibration_batch_"+ntupleName+".dat ./")
outScript.write("\necho \"fine copia\" ");

outScript.write("\nls")
outScript.write("\necho \"eseguo: "+command+"\" ")
outScript.write("\n"+command);
outScript.write("\nls")
outScript.write("\ncp -v -r EE__/ "+currentDir+"/EE_"+ntupleName+"/")
outScript.close();
os.system("chmod 777 "+currentDir+"/"+fn+".sh");
command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
if not options.generateOnly:
    os.system(command2);
print command2
'''





