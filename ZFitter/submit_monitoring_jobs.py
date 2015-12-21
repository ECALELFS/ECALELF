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

localReco = "multifit" #type of local reco: "multifit" or "weights"

currentDir = os.getcwd();
CMSSWDir = currentDir+"/../";

fn = "Job_monitoring_"+localReco+"/Job_"+"EB";
outScript = open(fn+".sh","w");
command = "ZFitter.exe -f EoverPcalibration_batch_"+localReco+"_hadd.dat --evtsPerPoint 5000 --laserMonitoringEP --EBEE EB --yMIN 0.85 --LUMI 2.5"
print command;
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+CMSSWDir);
outScript.write("\n"+'eval `scram runtime -sh`');
outScript.write("\n"+'cd -');
outScript.write("\necho $PWD");
outScript.write("\nls");

outScript.write("\necho \"copia1\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"+localReco+".root ./")
outScript.write("\necho \"copia2\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"+localReco+"-extraCalibTree.root ./")

outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EB_pTk.root ./")
outScript.write("\necho \"copia14\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EB_scE.root ./")
outScript.write("\necho \"copia15\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/EoverPcalibration_batch_"+localReco+"_hadd.dat ./")
outScript.write("\necho \"fine copia\" ");

outScript.write("\nls")
outScript.write("\necho \"eseguo: "+command+"\" ")
outScript.write("\n"+command);
outScript.write("\nls")
outScript.write("\ncp -v -r EB__/ "+currentDir+"/EB_"+localReco+"/")
outScript.close();
os.system("chmod 777 "+currentDir+"/"+fn+".sh");
command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
if not options.generateOnly:
    os.system(command2);
print command2







fn = "Job_monitoring_"+localReco+"/Job_"+"EE";
outScript = open(fn+".sh","w");
command = "ZFitter.exe -f EoverPcalibration_batch_"+localReco+"_hadd.dat --evtsPerPoint 5000 --laserMonitoringEP --EBEE EE --yMIN 0.65 --yMAX 1.15 --LUMI 2.5"
print command;
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+CMSSWDir);
outScript.write("\n"+'eval `scram runtime -sh`');
outScript.write("\n"+'cd -');
outScript.write("\necho $PWD");
outScript.write("\nls");


outScript.write("\necho \"copia1\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"+localReco+".root ./")
outScript.write("\necho \"copia2\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"+localReco+"-extraCalibTree.root ./")

outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EE_pTk.root ./")
outScript.write("\necho \"copia14\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/momentumCalibration2015_EE_scE.root ./")
outScript.write("\necho \"copia15\" ");
outScript.write("\ncmsStage /store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/EoverPcalibration_batch_"+localReco+"_hadd.dat ./")
outScript.write("\necho \"fine copia\" ");

outScript.write("\nls")
outScript.write("\necho \"eseguo: "+command+"\" ")
outScript.write("\n"+command);
outScript.write("\nls")
outScript.write("\ncp -v -r EE__/ "+currentDir+"/EE_"+localReco+"/")
outScript.close();
os.system("chmod 777 "+currentDir+"/"+fn+".sh");
command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
if not options.generateOnly:
    os.system(command2);
print command2






