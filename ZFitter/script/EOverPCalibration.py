#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess
import ROOT
from optparse import OptionParser
from subprocess import Popen
import commands
from commands import getstatusoutput
import datetime
import argparse
import string

if __name__ == '__main__':

    parser = OptionParser()

    parser.add_option ('--inputFile', action="store", dest="inputFile", default="data/validation/EoverPcalibration.dat", help="config file")
    parser.add_option ("--isBarrel", action="store", dest="isBarrel", default=1, help="1=barrel, 0=endcap")
    parser.add_option ("--jsonFileName", action="store", dest="jsonFileName", default="../EOverPCalibration/json/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt", help="jsonFileName")
    parser.add_option ("--isMiscalib", action="store", dest="isMiscalib", default="false", help="apply the initial miscalibration")
    parser.add_option ("--miscalibMethod", action="store", dest="miscalibMethod", default="1", help="miscalibration method")
    parser.add_option ("--miscalibMap", action="store", dest="miscalibMap", default="/gwteray/users/brianza/scalibMap2.txt", help="map for the miscalibration")
    parser.add_option ("--isSaveEPDistribution", action="store", dest="isSaveEPDistribution", default="false", help="save E/P distribution")
    parser.add_option ("--isMCTruth", action="store", dest="isMCTruth", default="false", help="option for MC")
    parser.add_option ("--isEPselection", action="store", dest="isEPselection", default="false", help="apply E/p selection")
    parser.add_option ("--isPtCut", action="store", dest="isPtCut", default="false", help="apply Pt cut")
    parser.add_option ("--PtMin", action="store", dest="PtMin", default="0.", help="treshold for the Pt cut")
    parser.add_option ("--isfbrem", action="store", dest="isfbrem", default="false", help="apply fbrem cut")
    parser.add_option ("--fbremMax", action="store", dest="fbremMax", default="100.", help="fbrem treshold")
    parser.add_option ("--isR9selection", action="store", dest="isR9selection", default="false", help="apply R9 selection")
    parser.add_option ("--R9Min", action="store", dest="R9Min", default="-1.", help="R9 treshold")
    parser.add_option ("--inputMomentumScale", action="store", dest="inputMomentumScale", default="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALELF/EOverPCalibration/MomentumCalibration2012", help="input momentum scale")
    parser.add_option ("--typeEB", action="store", dest="typeEB", default="eta1",help="")
    parser.add_option ("--typeEE", action="store", dest="typeEE", default="eta1",help="")
    parser.add_option ("--outputPath", action="store", dest="outputPath", default="output/output_runD/", help="output dir for E/P calibration")
    parser.add_option ("--outputFile", action="store", dest="outputFile", default="FastCalibrator_Oct22_Run2012ABC_Cal_Dic2012", help="output file for E/P calibration")
    parser.add_option ("--numberOfEvents", action="store", dest="numberOfEvents", default="-1", help="number of events (-1=all)")
    parser.add_option ("--useZ", action="store", dest="useZ", default="1", help="use Z events")
    parser.add_option ("--useW", action="store", dest="useW", default="1", help="use W events")
    parser.add_option ("--splitStat", action="store", dest="splitStat", default="1", help="split statistic")
    parser.add_option ("--nLoops", action="store", dest="nLoops", default="20", help="number of iteration of the L3 algorithm")
    parser.add_option ("--isDeadTriggerTower", action="store", dest="isDeadTriggerTower", default="false", help="")
    parser.add_option ("--inputFileDeadXtal", action="store", dest="inputFileDeadXtal", default="NULL", help="")

    (options, args) = parser.parse_args()

    if options.isBarrel==1:
        command = "ZFitter.exe --EOverPCalib -f %s --doEB --jsonFileName %s --isMiscalib %s --miscalibMethod %s --miscalibMap %s --isSaveEPDistribution %s --isMCTruth %s --isEPselection %s --isPtCut %s --PtMin %s --isfbrem %s --fbremMax %s --isR9selection %s --R9Min %s --inputMomentumScale %s --typeEB %s --typeEE %s --outputPath %s --outputFile %s --numberOfEvents %s --useZ %s --useW %s --splitStat %s --nLoops %s --isDeadTriggerTower %s --inputFileDeadXtal %s"%(options.inputFile, options.jsonFileName, options.isMiscalib, options.miscalibMethod, options.miscalibMap, options.isSaveEPDistribution, options.isMCTruth, options.isEPselection, options.isPtCut, options.PtMin, options.isfbrem, options.fbremMax, options.isR9selection, options.R9Min, options.inputMomentumScale, options.typeEB, options.typeEE, options.outputPath, options.outputFile, options.numberOfEvents, options.useZ, options.useW, options.splitStat, options.nLoops, options.isDeadTriggerTower, options.inputFileDeadXtal) 

    else:
        command = "ZFitter.exe --EOverPCalib -f %s --doEE --jsonFileName %s --isMiscalib %s --miscalibMethod %s --miscalibMap %s --isSaveEPDistribution %s --isMCTruth %s --isEPselection %s --isPtCut %s --PtMin %s --isfbrem %s --fbremMax %s --isR9selection %s --R9Min %s --inputMomentumScale %s --typeEB %s --typeEE %s --outputPath %s --outputFile %s --numberOfEvents %s --useZ %s --useW %s --splitStat %s --nLoops %s --isDeadTriggerTower %s --inputFileDeadXtal %s"%(options.inputFile, options.jsonFileName, options.isMiscalib, options.miscalibMethod, options.miscalibMap, options.isSaveEPDistribution, options.isMCTruth, options.isEPselection, options.isPtCut, options.PtMin, options.isfbrem, options.fbremMax, options.isR9selection, options.R9Min, options.inputMomentumScale, options.typeEB, options.typeEE, options.outputPath, options.outputFile, options.numberOfEvents, options.useZ, options.useW, options.splitStat, options.nLoops, options.isDeadTriggerTower, options.inputFileDeadXtal) 


    currentDir = os.getcwd()+'/';

    print currentDir;

    outScript = open("script/Job.sh","w");

    outScript.write('#!/bin/bash');
    outScript.write("\n"+'cd '+currentDir);
    outScript.write("\n"+'eval `scram runtime -sh`');
    #outScript.write("\n cd -");
    #outScript.write("\n cd /tmp/");

    outScript.write("\n"+ command )

    outScript.close();

    print 'Closing out script';

    os.system("chmod 777 "+currentDir+"script/Job.sh");
    os.system("bsub -q 8nh "+currentDir+"script/Job.sh")
