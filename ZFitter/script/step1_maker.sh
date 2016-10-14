#! /bin/bash
shopt -s expand_aliases
source ~gfasanel/.bashrc
file=$(echo $1 |sed "s|data/validation/||"| sed "s|.dat||" ) #so you can tab the name of the file :-)
interval=$2
invMass_type=$3

if [[ $1 = "" ]]; then
    echo "you should specify the validation file"; exit;
fi
if [[ $2 = "" ]]; then
    echo "you should specify the interval"; exit;
fi
if [[ $3 = "" ]]; then
    echo "you should specify the mass Type"; exit;
fi

sel=$4
if [[ $sel = "" ]]; then
    sel="loose"
fi

./script/energyScale.sh --step=1 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
#./script/energyScale.sh --step=1stability -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
###Only summary plots if you need them
#./script/energyScale.sh --step=1plotter -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel > tmp/1plot.dat
#
#PUName=$(grep "PUName" tmp/1plot.dat | awk -F":" '{print $2}')
#mcName=$(grep "mcName" tmp/1plot.dat | awk -F":" '{print $2}')
#selection=$(grep "selection" tmp/1plot.dat | awk -F":" '{print $2}')
##trim all whitespaces from variable
#PUName="$(echo -e "${PUName}" | tr -d '[[:space:]]')"
#mcName="$(echo -e "${mcName}" | tr -d '[[:space:]]')"
#selection="$(echo -e "${selection}" | tr -d '[[:space:]]')"
#
#echo "selection is "$selection
####Publish plots
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/ is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1 -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/data -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits/data -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/MC -p
#fi
#
###############Summary plots
###Inial
#cp test/dato/${file}/${selection}/${invMass_type}/step1/img/stability/before_run_corr/runNumber/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/  
###Final
#cp test/dato/${file}/${selection}/${invMass_type}/step1/img/stability/runNumber/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/  
#
###############Fit plots
###MC Fits
#cp test/MC/${mcName}/${PUName}/${selection}/${invMass_type}/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/MC/  #not 100% sure about name construction for mC
###Initial Fits
#cp test/dato/${file}/${selection}/${invMass_type}/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/data/
###Final Fits
#cp test/dato/${file}/${selection}/${invMass_type}/step1/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits/data/