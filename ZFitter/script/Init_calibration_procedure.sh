#! /bin/bash
source script/functions.sh
source script/bash_functions_calibration.sh

file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)

if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi

#add branch r9 
./script/addBranch.sh data/validation/${file}.dat R9Eleprime
#make pileupHist: they are used to make pileupTrees, and also they are required for step1
pileupHist
#make pileupTree
pileupTrees

if [[ $2 = "" ]]; then
    echo "you should specify the interval if you want to divide the runs"; exit;
fi
interval=$2
./script/divider_run.sh $file $interval