#! /bin/bash
file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
./script/energyScale.sh --step=0 -f $1 --runRangesFile=data/runRanges/${file}_interval_$2.dat --invMass_var=${3}