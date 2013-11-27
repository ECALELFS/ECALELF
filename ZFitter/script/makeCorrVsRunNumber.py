#!/usr/bin/env python

from sys import argv
from math import fabs,sqrt
import string

def readCorrVsRun(input):
    lst = {}
    for line in input.split("\n"):
        l = [ i for i in line.split(" ") if i != "" ]
#        print len(l);
        if len(l) < 6:
            continue
        key,runStart,runEnd,dmM,dmMErr = str(l[0]), int(l[2]), int(l[3]), float(l[4]), float(l[5])
#        print '%s %d %d %f %f' % (key, runStart, runStart, dmM, dmMErr)
        lst[ (key,runStart,runEnd) ] = ( -dmM+1,dmMErr )
    return lst

def readResCorr(input):
    lst = {}
    for line in input.split("\n"):
        l = [ i for i in line.split(" ") if i != "" ]
        if len(l) < 3:
            continue
        key,dmM,dmMErr = str(l[0]),  float(l[1]), float(l[2])
        print '%s %f %f' % (key,dmM,dmMErr)
        lst[ key ] = ( dmM,dmMErr )
    return lst


from optparse import OptionParser
parser = OptionParser()
parser.add_option("--file1", dest="fn1",metavar="FILE1")
parser.add_option("--file2", dest="fn2",metavar="FILE2")
parser.add_option("--output", dest="output",metavar="OUTPUT")
parser.add_option("-l",action="store_true",dest="latex",default=False)
parser.add_option("-c",action="store_true",dest="correction",default=False)

(options, args) = parser.parse_args()

corrVsRun={}
resCorr={}
fn1 = options.fn1
#print "Reading file " +str(fn1)
file1 = open(fn1)
corrVsRun=readCorrVsRun( file1.read() )
fn2 = options.fn2
#print "Reading file " +str(fn2)
file2 = open(fn2)
resCorr=readResCorr( file2.read() )

import sys

if ( not str(options.output).lower() == "none" ):
    sys.stdout = open(str(options.output),"w")
    
if (options.latex == True):
    print "\\documentclass{article}\n\\begin{document}"

cats=sorted(resCorr.keys())
runs=sorted(corrVsRun.keys(), key=lambda period: str(period[0])+str(period[1]))
inEB=False
inEE=False
for cat in cats:
#    if (str(cat).find("EtaBad") != -1 or str(cat).find("EtaGold") != -1):
    if (str(cat).find("-bad") != -1 or str(cat).find("-gold") != -1):
        if (not inEB and options.latex == True):
            if (str(cat).find("EB") != -1):
                print "\n\\begin{table}\n\\begin{tabular}{|c|c|c|c|}\n\\hline\ncategory & RunStart-RunEnd & $\\Delta$E/E & $\\Delta$E/E$_{err}$ \\\\\n"
                inEB=True
        if (not inEE and options.latex == True):
            if (str(cat).find("EE") != -1):
                print "\n\\hline\\end{tabular}\n\\end{table}\n\\begin{table}\n\\begin{tabular}{|c|c|c|c|}\n\\hline\ncategory & RunStart-RunEnd & $\\Delta$E/E & $\\Delta$E/E$_{err}$ \\\\\n"
                inEE=True
        if (options.latex == True):
            print '\\hline'
        else:
            print '###############################################################'
#        print cat,(1+(resCorr[cat][0]/100.))
        for key in runs:
            if (str(cat).find(key[0]) != -1):
                totCorr=(1+(resCorr[cat][0]/100))*(1+(corrVsRun[key][0]))
                errRunCorr=corrVsRun[key][1]
#                print corrVsRun[key][1]
                #sqrt(corrVsRun[key][1]*corrVsRun[key][1]+corrVsRun[key][3]*corrVsRun[key][3])/91.188
                totErr = sqrt((resCorr[cat][1]*resCorr[cat][1])/(100*100) + errRunCorr*errRunCorr)
                #                totCorr=(1+(resCorr[cat][0]/100))
                #                totCorr=(1+((corrVsRun[key][0]-corrVsRun[key][2])/91.188))
                if (options.latex == True):
                    print '%s & %d - %d & %.4f & %.4f\\\\' % (cat,key[1],key[2],1-(totCorr-1.) , totErr)
                elif (options.correction == True):
                    print '%s\trunNumber\t%d\t%d\t%.4f\t%.4f' % (cat,key[1],key[2],1-(totCorr-1.) , totErr)
                else:
                    print '%s\t%d\t%d\t%.4f\t%.4f' % (cat,key[1],key[2],1-(totCorr-1.) , totErr)


if (options.latex == True):
    print "\\hline\n\\end{tabular}\n\\end{table}\n\\end{document}"

