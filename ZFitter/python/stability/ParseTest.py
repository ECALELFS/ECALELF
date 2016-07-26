#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import re
import ParseTable as pt
from shutil import copyfile
import sys, getopt

inRunRange = ''
inStability = ''
invMass = ''

print sys.argv[:]

myopts, args = getopt.getopt(sys.argv[1:],"r:t:i:")
for o, a in myopts:
    if o == '-r':
        inRunRange = a
    if o == '-t':
        inStability = a
    if o == '-i':
        invMass = a



assert inStability != '', 'No stability file input given, use -t <stability.tex>'
assert inRunRange != '', 'No run range file input given, use -r <runRange.dat>'
assert invMass != '', 'No invariant mass name specified, use -i <invariant mass>'

print "Run range file to be used is ",inRunRange
print "Stability file to be used is ",inStability
print "Invariant mass that was used is ",invMass

if not os.path.exists('Plots/'):
    os.makedirs('Plots/')
if not os.path.exists('data/'):
    os.makedirs('data/')

#Rename and copy the datafiles into data/
runRangeFile = inRunRange.split('/')[-1]
stabilityFile = inRunRange.split('/')[-1].replace('.dat','')+'_'+invMass+'_stability.tex'

copyfile(inRunRange,'data/'+runRangeFile)
copyfile(inStability, 'data/'+stabilityFile)
print "They can now be found in data/ as "+runRangeFile+" and "+stabilityFile 

data_path = 'data/'
plot_path = 'Plots/' + runRangeFile.replace('.dat','')+'/'+invMass+'/'

if not os.path.exists(plot_path):
    os.makedirs(plot_path)

print "Starting plotmaking..."
for category in pt.ecal_regions:

    print "Beginning category ", category,
    if "gold" in category: 
        print " ...skipping: gold"
        continue
    if "bad"  in category: 
        print " ...skipping: bad"
        continue
    print

    #Get runrange and time info from the the runranges file
    d = pt.read_run_range(path=data_path,file=runRangeFile)
    #Get variables information from the stability monitoring .tex file
    d = pt.append_variables(path=data_path,file=stabilityFile,data=d,category=category)

    #Get variables to make plots of (data, not mc or err vars)
    variables = []
    timeVars = ['Nevents', 'UnixTime', 'run_number', 'UnixTime_min', 'UnixTime_max', 'run_min', 'run_max', 'date_min', 'date_max', 'time']
    for label in d.columns.values.tolist():
        if "MC" not in label and label not in timeVars and "_err" not in label:
            variables.append(label)

    #Loop over the vars
    for var in variables:

        #Get associated monte carlo info, or a placeholder
        varmc = var.replace("data","MC")
        if 'MC' not in varmc:
            print "[WARNING] MC counterpart not found for ", var
            mcvalue = -999
            mcerror = -999
        else:
            mcvalue = d[varmc][0]
            mcerror = d[varmc+'_err'][0]

        #Switches on whether the datapoints are evenly distributed along x
        evenXs = [False,True]

        #Plot as function of date or run numbers
        timevars = ['run_min','run_max','time']
        for timevar in timevars:
            for evenX in evenXs:
                pt.plot_stability( time = d[timevar], datavalues = d[var], dataerrors = d[var+'_err'], 
                                   mcvalue = mcvalue, mcerror = mcerror,
                                   label = pt.var_labels[var], category = category,path=plot_path, evenX = evenX )


