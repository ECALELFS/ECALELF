#!/bin/bash
#1 is the validation file with the full data/validation/xx.dat
#2 the interval
file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
interval=$2
./bin/ZFitter.exe -f data/validation/$file.dat --runDivide --nEvents_runDivide $interval >tmp/runranges.txt
#cat tmp/runranges.txt | grep "-" | grep -v "DEBUG" | grep -v "pileup" | grep -v "R9Eleprime" | grep -v "selected" #show at screen
cat tmp/runranges.txt | grep "-" | grep -v "DEBUG" | grep -v "pileup" | grep -v "R9Eleprime" | grep -v "selected"> data/runRanges/${file}_interval_${interval}.dat

