#!/bin/bash
jsonDCS=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-275125_13TeV_PromptReco_Collisions16_JSON.txt

jsonName=DCSONLY
jsonName=271036-275125_v1

#CHECK=--check



#where=remoteGlidein
scheduler=caf
tag_MC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py
tag_Data=config/reRecoTags/80X_dataRun2_Prompt_v9.py

while [ "1" == "1" ];do 
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RUN2016B | grep MINIAOD | head -2 | tail -1 ` --type=MINIAOD -s noSkim  --json=${json} --json_name=${jsonName} ${CHECK}

./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep  MINIAOD | head -1 | tail -1` --type MINIAOD -t ${tag_Data} -s noSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} ${CHECK}
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep  MINIAOD | head -2 | tail -1` --type MINIAOD -t ${tag_Data} -s noSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} ${CHECK}

sleep 10m
done
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016C | grep  MINIAOD | head -1 | tail -1` --type MINIAOD -t ${tag_Data} -s noSkim --json=${json} --json_name=${jsonName} --scheduler=${scheduler} ${CHECK}
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016C | grep  MINIAOD | head -2 | tail -1` --type MINIAOD -t ${tag_Data} -s noSkim --json=${json} --json_name=${jsonName} --scheduler=${scheduler} ${CHECK}


# while [ ! -e "prod_alcareco/DYJets_madgraph-RunIISpring16/allRange/res/finished" ] || [ ! -e "prod_alcareco/DYJets_amcatnlo-RunIISpring16/allRange/res/finished" ]
# do
# sleep 5m	
# ./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -1` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}
	
# ./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -2` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}

# done

# ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -1` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} 
# ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -2` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} 

# while [ "1" == "1" ];
# do
#    sleep 5m
# ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -1` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}
# ./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -2` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}
# done




# #./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16-miniAODv1 | head -1` --isMC --type MINIAOD -t ${tag_MC} -s noSkim  --scheduler=${where} ${CHECK}

# #if [[ $CHECK == "--createOnly" ]]; then
# #    crab -c prod_ntuples/MINIAODNTUPLE/80X_mcRun2_asymptotic_2016_v3/DYJets_amcatnlo-RunIISpring16-miniAODv1/allRange/RunII-2016/ -submit 1
# #fi

