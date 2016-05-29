#!/bin/bash
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
jsonName=271036-273450_noL1T_v1

json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
jsonName=DCSONLY

CHECK=--check

#while [ "1" == "1" ];do sleep 1h; 
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -2 | tail -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}


where=remoteGlidein
tag_MC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16-miniAODv1 | head -1` --isMC --type MINIAOD -t ${tag_MC} -s noSkim  --scheduler=${where} ${CHECK}

#if [[ $CHECK == "--createOnly" ]]; then
#    crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 1
#fi

