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
applyEcorr = ["False"];
split = ["0","1"];
cut = ["100.","0.05","0.10","0.15","0.20","0.30","0.50"];
smoothCut = ["0","1"];
energyType = ["0"];


for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        fn = "Job/Job_"+"EB"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g];
                        outScript = open(fn+".sh","w");
                        command = "ZFitter.exe -f data/validation/EoverPcalibration.dat --EOverPCalib --outputPath output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EB"+" --splitStat "+split[b]+" --nLoops 15 --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale momentumCalibration2015_EB_pTk.root --applyEcorr "+applyEcorr[f]+" --inputEnergyScale momentumCalibration2015_EB_rawE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd '+currentDir);
                        outScript.write("\n"+command);
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
                        command = "ZFitter.exe -f data/validation/EoverPcalibration.dat --EOverPCalib --outputPath output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/ --do"+"EE"+" --splitStat "+split[b]+" --nLoops 15 --EPMin "+cut[c]+" --noPU --smoothCut "+smoothCut[d]+" --applyPcorr "+applyPcorr[e]+" --inputMomentumScale momentumCalibration2015_EE_pTk.root --applyEcorr "+applyEcorr[f]+" --inputEnergyScale momentumCalibration2015_EE_rawE.root"
                        print command;
                        outScript.write('#!/bin/bash');
                        outScript.write("\n"+'cd '+CMSSWDir);
                        outScript.write("\n"+'eval `scram runtime -sh`');
                        outScript.write("\n"+'cd '+currentDir);
                        outScript.write("\n"+command);
                        outScript.close();
                        os.system("chmod 777 "+currentDir+"/"+fn+".sh");
                        command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
                        os.system(command2);
                        print command2


createAndPlotIC = "createAndPlotIC.sh"
out2 = open(createAndPlotIC,"w")
                            
for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        name = "EB"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                        fn = "cfg/calibrationPlots_"+name+".py";
                        folder =  "output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                        outScript = open(fn,"w");
                        outScript.write("import FWCore.ParameterSet.Config as cms");
                        outScript.write("\nprocess = cms.Process(\"calibrationPlotsEBparameters\")")
                        outScript.write("\nprocess.Options = cms.PSet(")
                        outScript.write("\ninFileName     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB.root\"),")
                        outScript.write("\ninFileNameEven     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_even.root\"),")
                        outScript.write("\ninFileNameOdd     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EB_odd.root\"),")
                        outScript.write("\nnEtaBinsEB     = cms.int32(1),")
                        outScript.write("\nnEtaBinsEE     = cms.int32(1),")
                        outScript.write("\nis2012Calib    = cms.bool(False),")
                        outScript.write("\nisEB           = cms.bool(True),")
                        outScript.write("\nevalStat       = cms.int32(1),")
                        outScript.write("\noutputFolder   = cms.string(\""+folder+"\"),")
                        outScript.write("\noutFileName    = cms.string(\""+name+".root\"),")
                        outScript.write("\noutputTxt      = cms.string(\"IC_"+name+"\"),")
                        outScript.write("\nfileType       = cms.string(\"cxx\")")
                        outScript.write("\n)")
                        out2.write("\nCalibrationPlots "+fn)


for b in range(len(split)):
    for c in range(len(cut)):
        for d in range(len(smoothCut)):
            for e in range(len(applyPcorr)):
                for f in range(len(applyEcorr)):
                    for g in range(len(energyType)):
                        name = "EE"+"_"+split[b]+"_"+cut[c]+"_smoothCut"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]
                        fn = "cfg/calibrationPlots_"+name+".py";
                        folder =  "output_"+cut[c]+"_smooth"+smoothCut[d]+"_pCorr_"+applyPcorr[e]+"_ECorr_"+applyEcorr[f]+"_useRaw"+energyType[g]+"/"
                        outScript = open(fn,"w");
                        outScript.write("import FWCore.ParameterSet.Config as cms");
                        outScript.write("\nprocess = cms.Process(\"calibrationPlotsEEparameters\")")
                        outScript.write("\nprocess.Options = cms.PSet(")
                        outScript.write("\ninFileName     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE.root\"),")
                        outScript.write("\ninFileNameEven     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_even.root\"),")
                        outScript.write("\ninFileNameOdd     = cms.string(\"/afs/cern.ch/user/l/lbrianza/work/PHD/DEF_ECALELF/CHE_SUCCEDE_IN_ECALELF/CALIBRAZIONI/CMSSW_7_4_12/src/Calibration/ZFitter/"+folder+"FastCalibrator_Oct2015_runD_WZ_noEP_EE_odd.root\"),")
                        outScript.write("\nnEtaBinsEB     = cms.int32(1),")
                        outScript.write("\nnEtaBinsEE     = cms.int32(1),")
                        outScript.write("\nis2012Calib    = cms.bool(False),")
                        outScript.write("\nisEB           = cms.bool(False),")
                        outScript.write("\nevalStat       = cms.int32(1),")
                        outScript.write("\noutputFolder   = cms.string(\""+folder+"\"),")
                        outScript.write("\noutFileName    = cms.string(\""+name+".root\"),")
                        outScript.write("\noutputTxt      = cms.string(\"IC_"+name+"\"),")
                        outScript.write("\nfileType       = cms.string(\"cxx\")")
                        outScript.write("\n)")
                        out2.write("\nCalibrationPlots "+fn)
