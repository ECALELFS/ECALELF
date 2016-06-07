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

ntupleName = "data-SingleElectron-MINIAOD-Run2016B"
#options: "data-Run2015D-25ns-multifit", "data-Run2015D-25ns-weights" or "data-Run2015D-25ns-stream".
folder = "/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/ntupleEoP/"

currentDir = os.getcwd();
CMSSWDir = currentDir+"/../";

os.system("mkdir Job_monitoring_"+ntupleName)
fn = "Job_monitoring_"+ntupleName+"/Job_"+"EB";

outScript = open(fn+".sh","w");
command = "ZFitter.exe -f EoverPcalibration_batch_"+ntupleName+".dat --evtsPerPoint 5000 --laserMonitoringEP --EBEE EB --yMIN 0.85 --LUMI 0.8 --dayMin 15-03-2016 --dayMax 06-06-2016"
print command;
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+CMSSWDir);
outScript.write("\n"+'eval `scram runtime -sh`');
outScript.write("\n"+'cd -');
outScript.write("\necho $PWD");
outScript.write("mkdir EB__");
outScript.write("\nls");

outScript.write("\necho \"copia1\" ");
outScript.write("\ncmsStage "+folder+ntupleName+".root ./")
outScript.write("\necho \"copia2\" ");
outScript.write("\ncmsStage "+folder+"extraCalibTree-"+ntupleName+".root ./")

outScript.write("\necho \"copia13\" ");
#                        outScript.write("\ncp -v /afs/cern.ch/user/l/lbrianza/work/public/ntupleEoP/* .")
outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EB_pTk.root ./")
outScript.write("\necho \"copia14\" ");
outScript.write("\ncmsStage "+folder+"momentumCalibration2015_EB_scE.root ./")
outScript.write("\necho \"copia15\" ");
outScript.write("\ncmsStage "+folder+"EoverPcalibration_batch_"+ntupleName+".dat ./")
outScript.write("\necho \"fine copia\" ");

outScript.write("\nls")
outScript.write("\necho \"eseguo: "+command+"\" ")
outScript.write("\n"+command);
outScript.write("\nls")
outScript.write("\ncp -v -r EB__/ "+currentDir+"/EB_"+ntupleName+"/")
outScript.close();
os.system("chmod 777 "+currentDir+"/"+fn+".sh");
command2 = "bsub -q cmscaf1nd -cwd "+currentDir+" "+currentDir+"/"+fn+".sh";
if not options.generateOnly:
    os.system(command2);
print command2







fn = "Job_monitoring_"+ntupleName+"/Job_"+"EE";
outScript = open(fn+".sh","w");
command = "ZFitter.exe -f EoverPcalibration_batch_"+ntupleName+".dat --evtsPerPoint 5000 --laserMonitoringEP --EBEE EE --yMIN 0.85 --LUMI 0.8 --dayMin 15-03-2016 --dayMax 06-06-2016"
print command;
outScript.write('#!/bin/bash');
outScript.write("\n"+'cd '+CMSSWDir);
outScript.write("\n"+'eval `scram runtime -sh`');
outScript.write("\n"+'cd -');
outScript.write("\necho $PWD");
outScript.write("mkdir EE__");
outScript.write("\nls");


outScript.write("\necho \"copia1\" ");
outScript.write("\ncmsStage "+folder+ntupleName+".root ./")
outScript.write("\necho \"copia2\" ");
outScript.write("\ncmsStage "+folder+"extraCalibTree"+ntupleName+".root ./")

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






