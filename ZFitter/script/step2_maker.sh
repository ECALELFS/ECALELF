#! /bin/bash
shopt -s expand_aliases
source ~gfasanel/.bashrc
file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
interval=$2
invMass_type=$3
scenario=$4

if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi
if [[ $2 = "" ]]; then
    echo "you should specify the interval"; exit;
fi
if [[ $3 = "" ]]; then
    echo "you should specify the mass Type"; exit;
fi
if [[ $4 = "" ]]; then
    echo "you should specify the scenario"; exit;
fi

#region1=scaleStep2smearing_1 
#region2=scaleStep2smearing_2 
#initParameters1=$4
#initParameters2=$5
echo $invMass_type $scenario

###############################START TO COOK EVERYTHING#########################################################################################################
#comment pileupHist (just to be clean and avoid confusion in step2 --> pileupTree must be used) 
##awk '{ if ($2 == "pileupHist") {$1 = "#"$1}; print }' data/validation/${file}.dat > tmp/val.dat
##mv tmp/val.dat data/validation/${file}.dat 
./script/energyScale.sh --step=2 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=${scenario}



