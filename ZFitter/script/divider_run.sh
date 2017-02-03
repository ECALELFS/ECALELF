#!/bin/bash
#1 is the validation file with the full data/validation/xx.dat
#2 the interval
##file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
file=`basename $1 .dat`
interval=$2
echo "[INFO] data/runRanges/runRangeLimits.dat is used to fix the limits; you can change this in bin/ZFitter.cpp"
./bin/ZFitter.exe -f data/validation/$file.dat --runDivide --nEvents_runDivide $interval --commonCut=noPF-isEle-EtLeading_32-EtSubLeading_20-absEta_1.566_2 | tee tmp/runranges.txt
cat tmp/runranges.txt | grep "-" | grep -v "DEBUG" | grep -v "R9Eleprime" | grep -v "selected" | grep -v "pileup" | grep -v "PU" | grep -v "Adding" | grep -v "eleID" | grep -v "[STATUS]"> data/runRanges/${file}_interval_${interval}.dat
echo "[INFO] ranges for your dataset are now defined in data/runRanges/${file}_interval_${interval}.dat"


