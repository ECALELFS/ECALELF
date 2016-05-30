#! /bin/bash
#you should start from a file with only d and s
#call this script via script/step2_only_caller.sh
file=`basename $1 .dat`
invMass_type=$2
outDirData=test/dato/
extension=${file}_${invMass_type}
scenario=$3
eleID=$4
initParameters1=$5
initParameters2=$6

####CAREFUL: region name without .dat 
#region1=scaleStep2smearing_1_R9prime_lastRun2015
#region2=scaleStep2smearing_2_R9prime_lastRun2015
region1=scaleStep2smearing_1 
region2=scaleStep2smearing_2 
#region1=scaleStep2smearing_1_R9prime 
#region1=scaleStep2smearing_1_R9prime_et
#region1=scaleStep2smearing_1_R9prime_et_check
#region2=scaleStep2smearing_2_R9prime 
#region1=scaleStep0
#region1=scaleStep0_bis
#region1=scaleStep0_et
#region1=scaleStep0_et_highR9
#region1=scaleStep0_et_2
#region2=scaleStep0_et_2

CommonCuts=Et_20-noPF #Standard common cuts
#CommonCuts=Et_30-noPF #Common cuts for 0T

echo $invMass_type ${extension} $scenario
echo "scenario is "$scenario
echo "directory is "$extension
source script/functions.sh
source script/bash_functions_calibration.sh
###############################START TO COOK EVERYTHING#########################################################################################################

#Init, jobs, finalize
if [[ $scenario = "" ]];then
    echo "You should tell me what do you want to do: which is the scenario?"
fi

if [[ $scenario = "Init" ]]; then
    #./script/Init_calibration_procedure.sh ${file} #R9Eleprime branch, pileupHist, pileUpTrees
    Categorize
    Test_1_job
fi

if [[ $scenario = "CatOnly" ]]; then
    echo "I am categorizing"
    echo "region1 is $region1"
    if [[ ${region2} != "" ]];then
	echo "region2 is $region2"
    fi
    Categorize
fi

if [[ $scenario = "Plot_job" ]]; then
    echo "I am running a plot job"
    Just_plot
fi

if [[ $scenario = "Test_job" ]]; then
    echo "I am running a test job"
    Test_1_job
fi

if [[ $scenario = "Plot_Test_job" ]]; then
    echo "I am running a test job"
    Plot_Test_job
fi

if [[ $scenario = "Closure_Et" ]]; then
    echo "I am running a test job with corrections"
    #corrections="--corrEleType=Zcorr_76 --smearEleType=Zcorr_76"
    #corrections="--corrEleType=Zcorr_76"
    corrections="--corrEleType=Zcorr_76 --initFile=smear_ele_corrections/init_RUN2_highR9.dat --profileOnly --plotOnly --nSmearToy=1"
    echo $corrections
    Test_1_job $corrections
fi

if [[ $scenario = "Closure_Et_stoc" ]]; then
    echo "I am running a test job with stochastic smearing corrections"
    corrections="--corrEleType=Zcorr_76 --smearEleType=Zcorr_76_stoc --outDirFitResData=test/dato/fitres/stochastic_smearing_applied --initFile=smear_ele_corrections/init_RUN2_RUN1like.dat"
    echo $corrections
    Test_1_job_stoc $corrections
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