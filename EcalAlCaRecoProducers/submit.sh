#! /bin/bash

#source ../initCmsEnvCRAB2.sh

#option=--createOnly
#option=--submitOnly
option=--check
#To force the hadd of ntuple create the file "finished" in the res directory of your jobs
#touch ...../res/finisched and then --chek again
#option=--submit

data(){
##DATI
where=caf
#where=remoteGlidein #for data use caf (datasets are at CERN)
#json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-273450_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
#json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-274240_13TeV_PromptReco_Collisions16_JSON.txt 
json=/afs/cern.ch/work/g/gfasanel/80_ECALELF_ntuples_new/CMSSW_8_0_8/src/Calibration/EcalAlCaRecoProducers/json_files_2016/myJson_274241_274421.txt
jsonName=274241-274421_golden
jsonName=Golden_json_10June
tag=config/reRecoTags/80X_dataRun2_Prompt_v8.py
#

#while [ "1" == "1" ];do 
    #v1 has no runs in the json
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep DoubleEG-Run2016B-PromptReco-v2-miniAOD_10June| grep 274421` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} $option


    #sleep 20m
#done

}

mc(){
#while [ "1" == "1" ];do sleep 1h; 
    where=remoteGlidein
    #where=caf
    json=No_json_for_MC
    tag_MC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py
    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep ZToEE_NNPDF30-powheg-Spring16` --type MINIAOD -t ${tag_MC} --json=${json} --json_name="RunII-2016" --scheduler=${where} ${option} --isMC
    #./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep DYToEE_NNPDF30_13TeV-powheg-pythia8` --type MINIAOD -t ${tag_MC} --json=${json} --json_name="RunII-2016" --scheduler=${where} ${option} --isMC
#    sleep 20m
#done    

}

if [[ $1 = "data" ]]; then
    data
fi

if [[ $1 = "mc" ]]; then
    mc
fi


# crab -c prod_ntuples/MINIAODNTUPLE/80X_dataRun2_Prompt_v8/DoubleEG-Run2016B-PromptReco-v2_test/273150-273730/271036-273450_golden/ -submit 1
# crab -c prod_ntuples/MINIAODNTUPLE/80X_dataRun2_Prompt_v8/DoubleEG-Run2016B-PromptReco-v2_test/273150-273730/271036-273450_golden/ -status

#    if [[ $option == "--createOnly" ]]; then
#	crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 1
#    fi

#    if [[ $option == "--submit" ]]; then
#	crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 502-688
#    fi


#
# ##Se un job fallisce o ha finito, prenditi l'output
# #getoutput
# crab -c prod_ntuples/MINIAODNTUPLE/74X_dataRun2_Prompt_v4/DoubleEG-emanuele-ZElectron_Run2015D_v3_74X/256584-258158/246908-260627-Prompt_25ns-v1-golden_silver/ -status -getoutput
# # E controlla il res
# /res/