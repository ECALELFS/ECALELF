#! /bin/bash

#source ../initCmsEnvCRAB2.sh

#option=--createOnly
#option=--submitOnly
option=--check

data(){
##DATI
where=caf
json=/afs/cern.ch/work/g/gfasanel/80_ECALELF_ntuples_new/CMSSW_8_0_8/src/Calibration/EcalAlCaRecoProducers/json_files_2016/diff_DCS_Golden_10June.txt
jsonName=diff_DCS_Golden_10June
tag=config/reRecoTags/80X_dataRun2_Prompt_v8.py
#

#while [ "1" == "1" ];do 

    ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep DoubleEG-Run2016B-PromptReco-v2-miniAOD_10June_DCS` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} $option

#    sleep 20m
#done

}


if [[ $1 = "data" ]]; then
    data
fi

