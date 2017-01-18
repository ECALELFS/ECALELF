#! /bin/bash
source script/functions.sh
source script/bash_functions_calibration.sh

file=`basename $1  .dat`

if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi

#add branch r9 => do not use it until the r9 corrections are available
##./script/addBranch.sh data/validation/${file}.dat R9Eleprime
#make pileupHist: they are used to make pileupTrees, and also they are required for step1
##pileupHist #if possible use the official ones, not the quick and dirty reweight
##make pileupTree
pileupTrees
#

if [[ $2 = "" ]]; then
    echo "you should specify the interval if you want to divide the runs"; exit;
fi
interval=$2
echo "Splitting in runNumbers: events ~ $2"
echo "During the splitting the range limits in data/runRanges/runRangeLimits.dat are considered"
./script/divider_run.sh $file $interval
