#!/bin/bash
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
jsonName=271036-273450_noL1T_v1

json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
jsonName=DCSONLY

CHECK=--check

#while [ "1" == "1" ];do sleep 1h; 
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -2 | tail -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}


