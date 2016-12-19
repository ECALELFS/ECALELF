1#! /usr/bin/env python
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

currentDir = os.getcwd();
CMSSWDir = currentDir+"/../";

applyPcorr = ["True"]; #decide if you want to apply momentum correction or not         
applyEcorr = ["False"]; #decide if you want to apply energy correction or not                                                                               
split = ["0","1"]; #0=full statistics, 1=even/odd                                                                                                             
cut = ["0.15","0.20","0.30","100"];#size of the E/p cut window (default is 0.15)    
#cut = ["0.10"]
smoothCut = ["1"]; #0=use step-function to reweight, 1=use the E/p distribution                                                                                 
energyType = ["0"]; #0=regression, 1=raw energy
nLoops = "15"; #number of iterations
ntupleName = "Run2016BCDEFG_WZSkim_Cal_Sep2016_final_v3_weights"
#ntupleName = "DYToEE_powheg_13TeV-RunIISpring15DR74-Asym25n-v1-allRange" #options: "data-Run2015D-25ns-multifit", "data-Run2015D-25ns-multifit" or "data-Run2015D-25ns-stream". 
#For the MC use: "DYToEE_powheg_13TeV-RunIISpring15DR74-Asym25n-v1-allRange"
#ntuplelist = ["data-SingleElectron-Run2016BCD_WSkim_12p9fb_multifit","data-DoubleEG-Run2016BCD_ZSkim_12p9fb_multifit"]

folder_dat = "/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/" #folder containing the .dat files (don't touch this)
folder = "/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/" #folder containing the .dat files (don't touch this)
#folder = "/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARECOSIM/DYToEE_powheg_13TeV-RunIISpring15DR74-Asym25n-v1/allRange/246908-258750-Prompt_25ns-v1-esPlanes/" #folder containing the MC ntuples

ntuplelist = [
"DoubleEG-Run2016B-ZSkim-Prompt_v2-weightsReco-273150-275376",
"DoubleEG-Run2016C-ZSkim-Prompt_v2-weightsReco-275420-276283",
"DoubleEG-Run2016D-ZSkim-Prompt_v2-weightsReco-276315-276811",
"DoubleEG-Run2016E-ZSkim-Prompt-v2-weightsReco-276830-277420",
"DoubleEG-Run2016F-ZSkim-Prompt-v1-weightsReco-277820-278808",
"DoubleEG-Run2016G-ZSkim-Prompt-v1-weightsReco-278817-279588",
"DoubleEG-Run2016G-ZSkim-Prompt-v1-weightsReco-279589-279931",
"SingleElectron-Run2016B-WSkim-Prompt_v2-weightsReco-273150-275376",
"SingleElectron-Run2016C-WSkim-Prompt_v2-weightsReco-275420-276283",
"SingleElectron-Run2016D-WSkim-Prompt_v2-weightsReco-276315-276811",
"SingleElectron-Run2016E-WSkim-Prompt-v2-weightsReco-276830-277420",
"SingleElectron-Run2016F-WSkim-Prompt-v1-weightsReco-277820-278808",
"SingleElectron-Run2016G-WSkim-Prompt-v1-weightsReco-278817-279588",
"SingleElectron-Run2016G-WSkim-Prompt-v1-weightsReco-279589-279931"
]

#folderlist=["/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"]
folderlist=[
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016B-ZSkim-Prompt_v2-weightsReco/273150-275376/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016C-ZSkim-Prompt_v2-weightsReco/275420-276283/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016D-ZSkim-Prompt_v2-weightsReco/276315-276811/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016E-ZSkim-Prompt-v2-weightsReco/276830-277420/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016F-ZSkim-Prompt-v1-weightsReco/277820-278808/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1-weightsReco/278817-279588/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/DoubleEG-Run2016G-ZSkim-Prompt-v1-weightsReco/279589-279931/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016B-WSkim-Prompt_v2-weightsReco/273150-275376/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016C-WSkim-Prompt_v2-weightsReco/275420-276283/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016D-WSkim-Prompt_v2-weightsReco/276315-276811/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016E-WSkim-Prompt-v2-weightsReco/276830-277420/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016F-WSkim-Prompt-v1-weightsReco/277820-278808/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016G-WSkim-Prompt-v1-weightsReco/278817-279588/271036_279931-Cal_Sep2016/withExtras/",
"/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Sep2016_final_v3/SingleElectron-Run2016G-WSkim-Prompt-v1-weightsReco/279589-279931/271036_279931-Cal_Sep2016/withExtras/",
]


TOP=currentDir
#cmscaf1nd

#os.system("mkdir Job_"+ntupleName+""+nLoops+"loop_2016_newPcorr")
os.system("mkdir Job_"+ntupleName+""+nLoops+"2016_newPcorr")
os.system("mkdir cfg_"+ntupleName+"_"+nLoops+"2016_newPcorr")
os.system("mkdir ICset_"+ntupleName+"_"+nLoops+"2016_newPcorr")

##RUN CALIBRATION ON BARREL
for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        fn = "Job_"+ntupleName+""+nLoops+"2016_newPcorr/Job_"+"EB"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g];
                        outScript = open(fn+".sh","w");
                        command = "ZFitter.exe -f EoverPcalibration_batch_"+ntupleName+".dat --EOverPCalib --outputPath output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EB"+" --splitStat "+split[b]+" --nLoops "+nLoops+" --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale MomentumCalibration2016_eta1_eta1.root --applyEcorr "+applyEcorr[f]+" useRawEnergy "+energyType[g]+" --inputEnergyScale momentumCalibration2015_EB_scE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd -');
                        outScript.write("\necho $PWD");
                        outScript.write("\nls");

                        for j in range(len(ntuplelist)):
                            outScript.write("\necho \"copy main tree\" ");
                            outScript.write("\ncmsStage "+folderlist[j]+""+ntuplelist[j]+".root ./")
                            outScript.write("\necho \"copy extracalib tree\" ");
                            outScript.write("\ncmsStage "+folderlist[j]+"extraCalibTree-"+ntuplelist[j]+".root ./")
                        
                        outScript.write("\necho \"copiy additional files\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")                                                        
                        outScript.write("\ncmsStage "+folder_dat+"momentumCalibration2015_EB_pTk.root ./")
                        outScript.write("\ncmsStage "+folder_dat+"momentumCalibration2015_EB_scE.root ./")
                        outScript.write("\ncmsStage "+folder_dat+"EoverPcalibration_batch_"+ntupleName+".dat ./")
                        outScript.write("\ncmsStage "+folder_dat+"MomentumCalibration2016_eta1_eta1.root ./")
                        outScript.write("\necho \"end copy\" ");

                        outScript.write("\nls")
                        outScript.write("\necho \"eseguo: "+command+"\" ")
                        outScript.write("\n"+command);
                        outScript.write("\nls")
                        outScript.write("\ncp -v -r output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ "+currentDir)
                        outScript.close();
                        os.system("chmod 777 "+currentDir+"/"+fn+".sh");
                        command2 = "bsub -q cmscaf1nw -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
                        if not options.generateOnly:
                            os.system(command2);
                        print command2


##RUN CALIBRATION ON ENDCAP
for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        fn = "Job_"+ntupleName+""+nLoops+"2016_newPcorr/Job_"+"EE"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g];
                        outScript = open(fn+".sh","w");
                        command = "ZFitter.exe -f EoverPcalibration_batch_"+ntupleName+".dat --EOverPCalib --outputPath output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EE"+" --splitStat "+split[b]+" --nLoops "+nLoops+" --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale MomentumCalibration2016_eta1_eta1.root --applyEcorr "+applyEcorr[f]+" useRawEnergy "+energyType[g]+" --inputEnergyScale momentumCalibration2015_EE_scE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd -');
                        outScript.write("\necho $PWD");
                        outScript.write("\nls");

                        for j in range(len(ntuplelist)):
                            outScript.write("\necho \"copy main tree\" ");
                            outScript.write("\ncmsStage "+folderlist[j]+""+ntuplelist[j]+".root ./")
                            outScript.write("\necho \"copy extracalib tree\" ");
                            outScript.write("\ncmsStage "+folderlist[j]+"extraCalibTree-"+ntuplelist[j]+".root ./")
                        
                        outScript.write("\necho \"copy additional files\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")                                                        
                        outScript.write("\ncmsStage "+folder_dat+"momentumCalibration2015_EE_pTk.root ./")
                        outScript.write("\ncmsStage "+folder_dat+"momentumCalibration2015_EE_scE.root ./")
                        outScript.write("\ncmsStage "+folder_dat+"EoverPcalibration_batch_"+ntupleName+".dat ./")
                        outScript.write("\ncmsStage "+folder_dat+"MomentumCalibration2016_eta1_eta1.root ./")
                        outScript.write("\necho \"end copy\" ");
                        outScript.write("\nls")
                        outScript.write("\necho \"eseguo: "+command+"\" ")
                        outScript.write("\n"+command);
                        outScript.write("\nls")
                        outScript.write("\ncp -v -r output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ "+currentDir)
                        outScript.close();
                        os.system("chmod 777 "+currentDir+"/"+fn+".sh");
                        command2 = "bsub -q cmscaf1nw -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
                        if not options.generateOnly:
                            os.system(command2);
                        print command2


##CREATE CFG FILES TO PRODUCE CALIBRATION PLOTS LATER

createAndPlotIC = "createAndPlotIC_"+ntupleName+"_"+nLoops+"2016_newPcorr.sh"
out2 = open(createAndPlotIC,"w")
                            
#for b in range(len(split)):
for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "EB"+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    fn = "cfg_"+ntupleName+"_"+nLoops+"2016_newPcorr/calibrationPlots_"+name+".py";
                    folder =  "output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    outScript = open(fn,"w");
                    outScript.write("import FWCore.ParameterSet.Config as cms");
                    outScript.write("\nprocess = cms.Process(\"calibrationPlotsEBparameters\")")
                    outScript.write("\nprocess.Options = cms.PSet(")
                    outScript.write("\n\tinFileName     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB.root\"),")
                    outScript.write("\n\tinFileNameEven     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_even.root\"),")
                    outScript.write("\n\tinFileNameOdd     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_odd.root\"),")
                    outScript.write("\n\tnEtaBinsEB     = cms.int32(1),")
                    outScript.write("\n\tnEtaBinsEE     = cms.int32(1),")
                    outScript.write("\n\tis2012Calib    = cms.bool(False),")
                    outScript.write("\n\tisEB           = cms.bool(True),")
                    outScript.write("\n\tevalStat       = cms.int32(1),")
                    outScript.write("\n\toutputFolder   = cms.string(\""+folder+"\"),")
                    outScript.write("\n\toutFileName    = cms.string(\""+name+".root\"),")
                    outScript.write("\n\toutputTxt      = cms.string(\"IC_"+name+"\"),")
                    outScript.write("\n\tfileType       = cms.string(\"cxx\")")
                    outScript.write("\n)")
                    out2.write("\nCalibrationPlots "+fn)


##CREATE CFG FILES TO PRODUCE CALIBRATION PLOTS LATER

#for b in range(len(split)):
for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "EE"+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    fn = "cfg_"+ntupleName+"_"+nLoops+"2016_newPcorr/calibrationPlots_"+name+".py";
                    folder =  "output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    outScript = open(fn,"w");
                    outScript.write("import FWCore.ParameterSet.Config as cms");
                    outScript.write("\nprocess = cms.Process(\"calibrationPlotsEEparameters\")")
                    outScript.write("\nprocess.Options = cms.PSet(")
                    outScript.write("\n\tinFileName     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE.root\"),")
                    outScript.write("\n\tinFileNameEven     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_even.root\"),")
                    outScript.write("\n\tinFileNameOdd     = cms.string(\""+TOP+"/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_odd.root\"),")
                    outScript.write("\n\tnEtaBinsEB     = cms.int32(1),")
                    outScript.write("\n\tnEtaBinsEE     = cms.int32(1),")
                    outScript.write("\n\tis2012Calib    = cms.bool(False),")
                    outScript.write("\n\tisEB           = cms.bool(False),")
                    outScript.write("\n\tevalStat       = cms.int32(1),")
                    outScript.write("\n\toutputFolder   = cms.string(\""+folder+"\"),")
                    outScript.write("\n\toutFileName    = cms.string(\""+name+".root\"),")
                    outScript.write("\n\toutputTxt      = cms.string(\"IC_"+name+"\"),")
                    outScript.write("\n\tfileType       = cms.string(\"cxx\")")
                    outScript.write("\n)")
                    out2.write("\nCalibrationPlots "+fn)

##CREATE FOLDER CONTAINING THE FINAL SET OF IC (EB+EE)

newFolder = "ICset_"+ntupleName+"_"+nLoops+"2016_newPcorr"

for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "IC_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    folder =  "output_"+ntupleName+"_"+nLoops+"2016_newPcorr_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    out2.write("\ncat "+folder+"IC_EB*txt "+folder+"IC_EE*txt > "+newFolder+"/"+name+".txt")
                    
