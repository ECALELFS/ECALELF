#!/bin/bash
json50ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-255031_13TeV_PromptReco_Collisions15_50ns_JSON_v2.txt
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-258159_13TeV_PromptReco_Collisions15_25ns_JSON_v3.txt
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-258714_13TeV_PromptReco_Collisions15_25ns_JSON.txt
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-258750_13TeV_PromptReco_Collisions15_25ns_JSON.txt
jsonName=246908-258750-Prompt_25ns-v1-esPlanes

CHECK="--check"
#CHECK="--submitOnly" 
# 
for tagfile in  config/reRecoTags/phiSym_materialCorrection_ref.py 
do
	echo
	#./scripts/removeRereco.sh -t $tagfile -f alcarereco_datasets.dat
#./scripts/removeRereco.sh -t $tagfile -f ntuple_datasets.dat
#	continue
	case $tagfile in 
		*ref.py)
 			#./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" --singleEle ${CHECK}
			#./scripts/RerecoQuick.sh -p RUN2015CD -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" --singleEle ${CHECK}
			./scripts/RerecoQuick.sh -p RUN2015D -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" --singleEle ${CHECK}
			;;
		*)
 			#./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK}
			./scripts/RerecoQuick.sh -p RUN2015CD -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK}
			;;
	esac

done



#unset CHECK
for tagfile in  config/reRecoTags/phiSym_materialCorrection_ref.py
do
	./scripts/RerecoQuick.sh -p RUN2015CD    -t $tagfile --json=$json25ns --json_name=$jsonName  --singleEle --doExtraCalibTree --ntupleOnly ${CHECK}
#	./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile --json=$json50ns --json_name=246908-255031-Prompt_50ns_v2-bis --singleEle --doExtraCalibTree --ntupleOnly ${CHECK}
done

exit 0
/bin/sleep 10m
CHECK="--check"
for tagfile in  config/reRecoTags/phiSym_materialCorrection_{ref,v1,v2,v3}.py
do
	./scripts/RerecoQuick.sh -p RUN2015CD -t $tagfile --json=$json25ns --json_name=246908-257599-Prompt_25ns-bis    --singleEle --doExtraCalibTree --ntupleOnly ${CHECK}
	./scripts/RerecoQuick.sh -p RUN2015C  -t $tagfile --json=$json50ns --json_name=246908-255031-Prompt_50ns_v2-bis --singleEle --doExtraCalibTree --ntupleOnly ${CHECK}
done




