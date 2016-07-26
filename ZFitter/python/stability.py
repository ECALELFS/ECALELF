#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import re
import sys, getopt
from stability  import ParseTable as pt 
from shutil     import copyfile
from optparse   import OptionParser
from pprint     import pprint

def get_options():
    parser = OptionParser()
    parser.add_option("-r", "--run-range",
                      dest="inRunRange", default='runRange.dat',
                      help="""
                      Input configuration file that contains the run ranges.
                      This file is produced by the Zfitter (please check the documentation)
                      and can be found in data/runRange/ directory
                      """,
                      metavar="FILE")

    parser.add_option("-s", "--stability",
                      dest="inStability", default='stability.tex',
                      help="""
                      stability tex file produced by the ZFitter
                      """,
                      metavar="FILE")
    
    parser.add_option("-i", "--inv-mass",
                      dest="invMass", default='invMassSC',
                      help="""
                      Invariant mass variable.
                      """)

    return parser.parse_args()
    
    
if __name__ == '__main__':
    (opt, args) =  get_options()
    assert opt.inStability != '', 'No stability file input given, use -s <stability.tex>'
    assert opt.inRunRange  != '', 'No run range file input given, use -r <runRange.dat>'
    assert opt.invMass     != '', 'No invariant mass name specified, use -i <invariant mass>'
    
    print "Run range file to be used is "   ,opt.inRunRange
    print "Stability file to be used is "   ,opt.inStability
    print "Invariant mass that was used is ",opt.invMass
    
    if not os.path.exists('plot-stability/'):
        os.makedirs('plot-stability/')
    if not os.path.exists('data-stability/'):
        os.makedirs('data-stability/')
        
    #Rename and copy the datafiles into data/
    runRangeFile  = opt.inRunRange.split('/')[-1]
    stabilityFile = opt.inRunRange.split('/')[-1].replace('.dat','')+'_'+opt.invMass+'_stability.tex'

    data_path = 'data-stability/'
    plot_path = 'plot-stability/' + runRangeFile.replace('.dat','') + '/' + opt.invMass + '/'
    
    copyfile(opt.inRunRange , data_path +'/'+ runRangeFile)
    copyfile(opt.inStability, data_path +'/'+ stabilityFile)
    print "They can now be found in data/ as " + runRangeFile + " and " + stabilityFile 
    
    if not os.path.exists(plot_path):
        os.makedirs(plot_path)

    print "Starting plotmaking..."
    print "categories :: "
    pprint( pt.ecal_regions ) 
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
        timeVars = ['Nevents'     ,
                    'UnixTime'    ,
                    'run_number'  ,
                    'UnixTime_min',
                    'UnixTime_max',
                    'run_min'     ,
                    'run_max'     ,
                    'date_min'    ,
                    'date_max'    ,
                    'time'        ]
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
                    pt.plot_stability( time = d[timevar], datavalues = d[var],
                                       dataerrors = d[var+'_err'], mcvalue = mcvalue,
                                       mcerror = mcerror, label = pt.var_labels[var],
                                       category = category, path=plot_path, evenX = evenX )


