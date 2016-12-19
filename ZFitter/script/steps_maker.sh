#! /bin/bash
shopt -s expand_aliases
source ~/.bashrc
file=`basename $1 .dat` #so you can tab the name of the file :-)
interval=$2
invMass_type=$3
sel=$4 #if empty default one is used cutBased....loose

if [[ $1 = "" ]]; then echo "you should specify the validation file"; exit;fi
if [[ $2 = "" ]]; then echo "you should specify the interval"; exit;fi
if [[ $3 = "" ]]; then echo "you should specify the mass Type"; exit;fi
if [[ $4 = "" ]]; then sel="cutBasedElectronID-Spring15-25ns-V1-standalone-loose";fi

echo "[INFO: invMass scenario] " $invMass_type $scenario
###############################START TO COOK EVERYTHING#########################################################################################################
#comment pileupHist (just to be clean and avoid confusion in step2 --> pileupTree must be used) 
##awk '{ if ($2 == "pileupHist") {$1 = "#"$1}; print }' data/validation/${file}.dat > tmp/val.dat
##mv tmp/val.dat data/validation/${file}.dat 

echo "[INFO] You are making step1"
#./script/energyScale.sh --step=1 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
#./script/energyScale.sh --step=1stability -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel
echo "[INFO] You are making step1stab"
#bsub -q cmscaf1nd -eo tmp/step1stab_err.dat -oo tmp/stepa1stab_out.dat -J "step1 stab" "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION; ./script/energyScale.sh --step=1stability -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel; ./script/energyScale.sh --step=1plotter -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel > tmp/1plot.dat"
echo "[INFO] You are making step1plotter"
#./script/energyScale.sh --step=1plotter -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=$sel > tmp/1plot.dat
echo "[INFO] You are making step2"
####################STEP2###################################
#./script/energyScale.sh --step=2 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=Submit_jobs
#./script/energyScale.sh --step=2 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=Fit_Likelihood_1
./script/energyScale.sh --step=2 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=finalize_step2

echo "[INFO] You are making step4"
##############Now go to step4 ##############################
#./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat
#./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=cat_step4
#The typical scenario is that you want to re-do the plots with the correct Lumi label:
#./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=plotOnly_step4
#./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=fit_step4
#./script/energyScale.sh --step=4 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=finalize_step4

echo "[INFO] You are making step5"
#./script/energyScale.sh --step=5 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat 
echo "[INFO] You are making step6"
#./script/energyScale.sh --step=6 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=finalize_step6
#./script/energyScale.sh --step=6 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=write_corr
#./script/energyScale.sh --step=6 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=test
echo "[INFO] You are making step7"
#./script/energyScale.sh --step=7 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --scenario=write_corr
echo "[INFO] You are making step8"
#./script/energyScale.sh --step=8 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat 
echo "[INFO] You are making gain switch study"
#./script/energyScale.sh --step=gainSwitch6 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=${sel}
#./script/energyScale.sh --step=gainSwitch6 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=${sel} --scenario=finalize_gain
#echo "[INFO] You are making gain switch 7"
#./script/energyScale.sh --step=gainSwitch7 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat #--scenario=finalize_gain
#./script/energyScale.sh --step=gainSwitchEne -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=${sel} #--scenario=finalize_gain
#./script/energyScale.sh --step=gainSwitchSeedEne -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat --selection=${sel} #--scenario=finalize_gain

#############Now systematics
#EleID
#./script/energyScale.sh --step=4medium -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step4medium.dat
#./script/energyScale.sh --step=4tight -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat | tee tmp/debug_step4tight.dat

#EleRegression
#./script/energyScale.sh --step=4ele -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat

#R9
#./script/energyScale.sh --step=4R9_93 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat
#./script/energyScale.sh --step=4R9_95 -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat

###Not yet tried
############MCs###############################################################################
##./script/energyScale.sh --step=4amctnlo -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat| tee tmp/debug_amctnlo.dat
##./script/energyScale.sh --step=4madgraph -f data/validation/${file}.dat --invMass_var=${invMass_type} --runRangesFile=data/runRanges/${file}_interval_${interval}.dat| tee tmp/debug_madgraph.dat
#
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4amctnlo/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/
#cp tmp/table_outFile-step4amctnlo-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4amctnlo/
#
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4madgraph/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/
#cp tmp/table_outFile-step4madgraph-${invMass_type}-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4madgraph/

####ET syst###########
#if [ ! -d "/afs/cern.ch/work/g/gfasanel/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25" ];then 
#    echo "~gfasanel/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25 is being created"
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/ -p
#    mkdir_www ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/DataMC/ -p
#fi
#
#cp test/dato/${file}/loose/${invMass_type}/step4Et_25/img/outProfile-scaleStep4smearing_*.png ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4Et_25-${invMass_type}-loose-Et_25-noPF-HggRunEtaR9.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/
#cp test/dato/${file}/loose/${invMass_type}/table/outFile-step4Et_25-${invMass_type}-loose-Et_25-noPF-HggRunEtaR9-smearEle_err.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/
#./script/latex_table_writer.sh test/dato/${file}/loose/${invMass_type}/table/outFile-step4Et_25-${invMass_type}-loose-Et_25-noPF-HggRunEtaR9.dat -Et_20-noPF
#cp tmp/table_outFile-step4Et_25-${invMass_type}-loose-Et_25-noPF-HggRunEtaR9_scale_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/
#cp tmp/table_outFile-step4Et_25-${invMass_type}-loose-Et_25-noPF-HggRunEtaR9_smear_tex.dat ~/scratch1/www/RUN2_ECAL_Calibration/${file}/${invMass_type}/step4Et_25/
