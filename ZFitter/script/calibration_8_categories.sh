#! /bin/bash
#you should start from a file with only d and s
#call this script via script/step2_only_caller.sh
file=$1
invMass_type=$2
outDirData=test/dato/
extension=$3
#region1=scaleStep2smearing_1 
#region2=scaleStep2smearing_2 
region1=scaleStep2smearing_1_R9prime 
region2=scaleStep2smearing_2_R9prime 
initParameters1=$4
initParameters2=$5
scenario=$6
echo $invMass_type ${extension} $scenario
echo "scenario is "$scenario
source script/functions.sh
source script/bash_functions_calibration.sh
###############################START TO COOK EVERYTHING#########################################################################################################

#Init, jobs, finalize
if [[ $scenario = "" ]];then
    echo "You should tell me what do you want to do: which is the scenario?"
fi

if [[ $scenario = "CatOnly" ]]; then
    echo "I am here"
    Categorize
fi

if [[ $scenario = "Init" ]]; then
    pileupHist
    Cat_and_pileUpTrees
    Test_1_job
fi

if [[ $scenario = "jobs" ]]; then
    Submit_50_jobs
fi

if [[ $scenario = "finalize_plots" ]]; then
    echo "you are finalizing the plots"
    Likelihood_Fitter
    Plot_data_MC
    #Write_down_root_corr
    Prepare_Slide_8_cat
fi

if [[ $scenario = "finalize" ]]; then
    Likelihood_Fitter
    Plot_data_MC
    Write_down_root_corr
    Prepare_Slide_8_cat
fi

if [[ $scenario = "closure" ]]; then
    Write_down_root_corr
    closure_test
fi

if [[ $scenario = "slide" ]]; then
    echo "you want just to make the slides"
    Prepare_Slide_8_cat_profileOnly
    Prepare_Slide_8_cat
fi

if [[ $scenario = "Full_Story" ]]; then
    pileupHist
    Cat_and_pileUpTrees
    Test_1_job
    Submit_50_jobs
    Likelihood_Fitter
    Plot_data_MC
    Write_down_root_corr
    closure_test
    Prepare_Slide_8_cat
fi