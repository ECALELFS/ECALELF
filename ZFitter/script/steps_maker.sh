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

echo $invMass_type $scenario

###############################START TO COOK EVERYTHING#########################################################################################################
#comment pileupHist (just to be clean and avoid confusion in step2 --> pileupTree must be used) 
##awk '{ if ($2 == "pileupHist") {$1 = "#"$1}; print }' data/validation/${file}.dat > tmp/val.dat
##mv tmp/val.dat data/validation/${file}.dat 

sel=$4
if [[ $sel = "" ]]; then
    sel="loose"
fi

./script/energyScale.sh --step=1 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
./script/energyScale.sh --step=1stability -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
##Only summary plots if you need them
./script/energyScale.sh --step=1plotter -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel > tmp/1plot.dat

PUName=$(grep "PUName" tmp/1plot.dat | awk -F":" '{print $2}')
mcName=$(grep "mcName" tmp/1plot.dat | awk -F":" '{print $2}')
selection=$(grep "selection" tmp/1plot.dat | awk -F":" '{print $2}')
#trim all whitespaces from variable
PUName="$(echo -e "${PUName}" | tr -d '[[:space:]]')"
mcName="$(echo -e "${mcName}" | tr -d '[[:space:]]')"
selection="$(echo -e "${selection}" | tr -d '[[:space:]]')"

echo "selection is "$selection
###Publish plots
if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/" ];then 
    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/ is being created"
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/ -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1 -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/data -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits/data -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/MC -p
fi

##############Summary plots
##Inial
cp test/dato/${file}/${selection}/${invMass_type}/step1/img/stability/before_run_corr/runNumber/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/  
##Final
cp test/dato/${file}/${selection}/${invMass_type}/step1/img/stability/runNumber/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/  

##############Fit plots
##MC Fits
cp test/MC/${mcName}/${PUName}/${selection}/${invMass_type}/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/MC/  #not 100% sure about name construction for mC
##Initial Fits
cp test/dato/${file}/${selection}/${invMass_type}/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1/Fits/data/
##Final Fits
cp test/dato/${file}/${selection}/${invMass_type}/step1/img/*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/Step1_stab/Fits/data/


####################STEP2###################################
./script/energyScale.sh --step=2 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step2.dat
if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step2" ];then 
    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step2 is being created"
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/ -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/DataMC/ -p
fi

cp test/dato/${file}/loose/${invMass_type}/step2/img/outProfile-scaleStep2smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/
cp test/dato/${file}/loose/${invMass_type}/table/step2-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/
./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step2-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
cp tmp/table_outFile-step2-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/
cp tmp/table_outFile-step2-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step2/

##############Now go to step4 ##############################
./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step4.dat
if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step4" ];then 
    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step4 is being created"
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/ -p
    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/DataMC/ -p
fi

cp test/dato/${file}/loose/${invMass_type}/step4/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/
cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/
cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9-smearEle_err.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/
./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
cp tmp/table_outFile-step4-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/
cp tmp/table_outFile-step4-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4/

#############Now systematics
#./script/energyScale.sh --step=4medium -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step4medium.da
#./script/energyScale.sh --step=4tight -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step4tight.dat


#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step4medium" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4medium/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4medium-${invMass_type}-medium-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4medium-${invMass_type}-medium-Et_20-noPF-HggRunEtaR9-smearEle_err.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4medium-${invMass_type}-medium-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4medium-${invMass_type}-medium-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/
#cp tmp/table_outFile-step4medium-${invMass_type}-medium-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4medium/
#
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step4tight" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4tight/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4tight-${invMass_type}-tight-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4tight-${invMass_type}-tight-Et_20-noPF-HggRunEtaR9-smearEle_err.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4tight-${invMass_type}-tight-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4tight-${invMass_type}-tight-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/
#cp tmp/table_outFile-step4tight-${invMass_type}-tight-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4tight/
#
############MCs###############################################################################
##./script/energyScale.sh --step=4amctnlo -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat| tee tmp/debug_amctnlo.dat
##./script/energyScale.sh --step=4madgraph -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat| tee tmp/debug_madgraph.dat
#
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step4amctnlo" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4amctnlo/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/
#cp tmp/table_outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4amctnlo/
#
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/step4madgraph" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4madgraph/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/
#cp tmp/table_outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/step4madgraph/



