#!/bin/bash
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-275783_13TeV_PromptReco_Collisions16_JSON.txt
jsonName=271036_275783-Prompt

json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt
jsonName=271036_276811-ICHEP

json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-279588_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
jsonName=271036_279588-Prompt

json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-279931_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
jsonName=271036_279931-Cal_Sep2016

PERIOD=CAL_SEP2016_V2

for tagfile in  config/reRecoTags/Cal_Sep2016_final_v3.py 
do
	echo
#	./scripts/removeRereco.sh -t $tagfile -f alcarereco_datasets.dat
#	./scripts/removeRereco.sh -t $tagfile -f ntuple_datasets.dat --json_name=$jsonName
#	continue

	for CHECK in '' --check
	do
		case $tagfile in 
			*/Cal_Sep2016_final_v3.py)
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile  --json=$json25ns --json_name="noJSON" ${CHECK} --alcarerecoOnly  --singleEle --weightsReco
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile  --json=$json25ns --json_name="noJSON" ${CHECK} --alcarerecoOnly  --singleEle
				;;
			*)
				echo 
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile  --json=$json25ns --json_name="noJSON" ${CHECK} --alcarerecoOnly # --singleEle
				;;
		esac
	done

	for CHECK in '' --check
	do
		case $tagfile in 
			*/Cal_Sep2016_final_v3.py)
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile --json=$json25ns --json_name=$jsonName --ntupleOnly  $CHECK --singleEle --weightsReco
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile --json=$json25ns --json_name=$jsonName --ntupleOnly  $CHECK --singleEle
				;;
			*)
				./scripts/RerecoQuick.sh -p ${PERIOD} -t $tagfile --json=$json25ns --json_name=$jsonName --ntupleOnly  $CHECK 
				;;
		esac
	done
done

exit 0
