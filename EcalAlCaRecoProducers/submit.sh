#! /bin/bash

#source ../initCmsEnvCRAB2.sh

#option=--createOnly
#option=--submitOnly
option=--check
#option=--submit

data(){
##DATI
where=caf
#where=remoteGlidein
#json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-273450_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
#json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-274240_13TeV_PromptReco_Collisions16_JSON.txt  
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
#jsonName=271036-274240_golden
jsonName=DCS_only_06June
tag=config/reRecoTags/80X_dataRun2_Prompt_v8.py
#

#while [ "1" == "1" ];do 
    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep miniAOD | head -1` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} $option

#    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep miniAOD | head -2` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} $option

#    sleep 1h
#done

#while [ "1" == "1" ];do 
#    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep miniAOD | head -1` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} --check
#
#    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep miniAOD | head -2` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} --check
#    
#    sleep 1h
#done 


# crab -c prod_ntuples/MINIAODNTUPLE/80X_dataRun2_Prompt_v8/DoubleEG-Run2016B-PromptReco-v2_test/273150-273730/271036-273450_golden/ -submit 1
# crab -c prod_ntuples/MINIAODNTUPLE/80X_dataRun2_Prompt_v8/DoubleEG-Run2016B-PromptReco-v2_test/273150-273730/271036-273450_golden/ -status
}


mc(){
#while [ "1" == "1" ];do sleep 1h; 
    #where=remoteGlidein
    where=caf
    json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-273450_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
    tag_MC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py
    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16MiniAODv1` --type MINIAOD -t ${tag_MC} --json=${json} --json_name="RunII-2016" --scheduler=${where} ${option} --isMC
    
#    if [[ $option == "--createOnly" ]]; then
#	crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 1
#    fi

    if [[ $option == "--submit" ]]; then
	crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 502-688
    fi

#done

#
# ##Se un job fallisce o ha finito, prenditi l'output
# #getoutput
# crab -c prod_ntuples/MINIAODNTUPLE/74X_dataRun2_Prompt_v4/DoubleEG-emanuele-ZElectron_Run2015D_v3_74X/256584-258158/246908-260627-Prompt_25ns-v1-golden_silver/ -status -getoutput
# # E controlla il res
# /res/
}

if [[ $1 = "data" ]]; then
    data
fi

if [[ $1 = "mc" ]]; then
    mc
fi
