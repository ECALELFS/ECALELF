#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess

currentDir = os.getcwd();
CMSSWDir = currentDir+"/../";

applyPcorr = ["False","True"];
applyEcorr = ["False","True"];
split = ["0","1"];
cut = ["100.","0.05","0.15","0.30","0.50"];
smoothCut = ["0","1"];
energyType = ["0","1"]; #0=regression, 1=raw energy

TOP="$PWD"
#cmscaf1nd


for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        fn = "Job/Job_"+"EB"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g];
                        outScript = open(fn+".sh","w");
                        command = "ZFitter.exe -f EoverPcalibration_batch.dat --EOverPCalib --outputPath output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EB"+" --splitStat "+split[b]+" --nLoops 20 --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale momentumCalibration2015_EB_pTk.root --applyEcorr "+applyEcorr[f]+" useRawEnergy "+energyType[g]+" --inputEnergyScale momentumCalibration2015_EB_rawE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd -');
                        outScript.write("\necho $PWD");
                        outScript.write("\nls");
                        outScript.write("\necho \"copia1\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_256584-257611.root ./")
                        outScript.write("\necho \"copia2\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_256584-257611_extraCalibTree.root ./")
                        outScript.write("\necho \"copia3\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_257612-258158.root ./")
                        outScript.write("\necho \"copia4\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_257612-258158_extraCalibTree.root ./")
                        outScript.write("\necho \"copia5\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v4-25nsReco_258159-258959.root ./")
                        outScript.write("\necho \"copia6\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v4-25nsReco_258159-258959_extraCalibTree.root ./")
                        outScript.write("\necho \"copia7\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_256584-257611.root ./")
                        outScript.write("\necho \"copia8\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_256584-257611_extraCalibTree.root ./")
                        outScript.write("\necho \"copia9\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_257612-258158.root ./")
                        outScript.write("\necho \"copia10\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_257612-258158_extraCalibTree.root ./")
                        outScript.write("\necho \"copia11\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v4-25nsReco_258159-258959.root ./")
                        outScript.write("\necho \"copia12\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v4-25nsReco_258159-258959_extraCalibTree.root ./")

                        outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EB_pTk.root ./")
                        outScript.write("\necho \"copia14\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EB_rawE.root ./")
                        outScript.write("\necho \"copia15\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/EoverPcalibration_batch.dat ./")
                        outScript.write("\necho \"fine copia\" ");
                        outScript.write("\nls")
                        outScript.write("\necho \"eseguo: "+command+"\" ")
                        outScript.write("\n"+command);
                        outScript.write("\nls")
                        outScript.write("\ncp -v -r output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ "+currentDir)
                        outScript.close();
                        os.system("chmod 777 "+currentDir+"/"+fn+".sh");
                        command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
                        os.system(command2);
                        print command2



for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        fn = "Job/Job_"+"EE"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g];
                        outScript = open(fn+".sh","w");
                        command = "ZFitter.exe -f EoverPcalibration_batch.dat --EOverPCalib --outputPath output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EE"+" --splitStat "+split[b]+" --nLoops 20 --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale momentumCalibration2015_EE_pTk.root --applyEcorr "+applyEcorr[f]+" useRawEnergy "+energyType[g]+" --inputEnergyScale momentumCalibration2015_EE_rawE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd -');
                        outScript.write("\necho $PWD");
                        outScript.write("\nls");
                        outScript.write("\necho \"copia1\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_256584-257611.root ./")
                        outScript.write("\necho \"copia2\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_256584-257611_extraCalibTree.root ./")
                        outScript.write("\necho \"copia3\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_257612-258158.root ./")
                        outScript.write("\necho \"copia4\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v3-25nsReco_257612-258158_extraCalibTree.root ./")
                        outScript.write("\necho \"copia5\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v4-25nsReco_258159-258959.root ./")
                        outScript.write("\necho \"copia6\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/DoubleEG-ZSkim-Run2015D-Prompt-v4-25nsReco_258159-258959_extraCalibTree.root ./")
                        outScript.write("\necho \"copia7\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_256584-257611.root ./")
                        outScript.write("\necho \"copia8\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_256584-257611_extraCalibTree.root ./")
                        outScript.write("\necho \"copia9\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_257612-258158.root ./")
                        outScript.write("\necho \"copia10\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v3-25nsReco_257612-258158_extraCalibTree.root ./")
                        outScript.write("\necho \"copia11\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v4-25nsReco_258159-258959.root ./")
                        outScript.write("\necho \"copia12\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/SingleElectron-WSkim-Run2015D-Prompt-v4-25nsReco_258159-258959_extraCalibTree.root ./")

                        outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EE_pTk.root ./")
                        outScript.write("\necho \"copia14\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EE_rawE.root ./")
                        outScript.write("\necho \"copia15\" ");
                        outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/EoverPcalibration_batch.dat ./")
                        outScript.write("\necho \"fine copia\" ");
                        outScript.write("\nls")
                        outScript.write("\necho \"eseguo: "+command+"\" ")
                        outScript.write("\n"+command);
                        outScript.write("\nls")
                        outScript.write("\ncp -v -r output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ "+currentDir)
                        outScript.close();
                        os.system("chmod 777 "+currentDir+"/"+fn+".sh");
                        command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
                        os.system(command2);
                        print command2



createAndPlotIC = "createAndPlotIC.sh"
out2 = open(createAndPlotIC,"w")
                            
#for b in range(len(split)):
for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "EB"+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    fn = "cfg/calibrationPlots_"+name+".py";
                    folder =  "output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    outScript = open(fn,"w");
                    outScript.write("import FWCore.ParameterSet.Config as cms");
                    outScript.write("\nprocess = cms.Process(\"calibrationPlotsEBparameters\")")
                    outScript.write("\nprocess.Options = cms.PSet(")
                    outScript.write("\n\tinFileName     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB.root\"),")
                    outScript.write("\n\tinFileNameEven     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_even.root\"),")
                    outScript.write("\n\tinFileNameOdd     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_odd.root\"),")
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


#for b in range(len(split)):
for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "EE"+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    fn = "cfg/calibrationPlots_"+name+".py";
                    folder =  "output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    outScript = open(fn,"w");
                    outScript.write("import FWCore.ParameterSet.Config as cms");
                    outScript.write("\nprocess = cms.Process(\"calibrationPlotsEEparameters\")")
                    outScript.write("\nprocess.Options = cms.PSet(")
                    outScript.write("\n\tinFileName     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE.root\"),")
                    outScript.write("\n\tinFileNameEven     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_even.root\"),")
                    outScript.write("\n\tinFileNameOdd     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_odd.root\"),")
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


newFolder = "ICset"

for c in range(len(cut)):
    for d in range(len(smoothCut)):
        for e in range(len(applyPcorr)):
            for f in range(len(applyEcorr)):
                for g in range(len(energyType)):
                    name = "IC_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                    folder =  "output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                    out2.write("\ncat "+folder+"IC_EB*txt "+folder+"IC_EE*txt > "+newFolder+"/"+name+".txt")
                    
