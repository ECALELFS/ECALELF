#!/bin/bash
json50ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-255031_13TeV_PromptReco_Collisions15_50ns_JSON_v2.txt
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260426_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt
json25ns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt
jsonName=246908-260627-Prompt_25ns-v1-full5x5fix-golden_silver

json25nsBoff=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_ZeroTesla_25ns_JSON.txt
jsonName=246908-260627-Prompt_ZeroTesla_25ns

json25ns=$json25nsBoff

#CHECK="--check"
# config/reRecoTags/pulseShapeStudy_Run2015_rereco_v1.py
# 
#config/reRecoTags/phiSym_materialCorrection_v{2,4,5,6,7,8,9,10}.py
#config/reRecoTags/preshowerClustering_v{1,2,3}.py config/reRecoTags/phiSym_materialCorrection_ref.py
#
#config/reRecoTags/phiSym_materialCorrection_ref.py config/reRecoTags/phiSym_materialCorrection_v{1,2,4,5,6,7,8,9,10}.py config/reRecoTags/Cal_Nov2015_PS_v{1,2,3,4,5}.py 
# config/reRecoTags/Cal_Nov2015_ICEoP_v{1,2,3,4,5,6,7,8,9,10,11,12}.py
#
#config/reRecoTags/Cal_Nov2015_PS_v1_ESmip_v1.py 
#config/reRecoTags/Cal_Nov2015_ICEoP_v1{4,5,6,7}{,_bis}.py 
# config/reRecoTags/74X_dataRun2_Prompt_v4.py config/reRecoTags/Cal_Nov2015_ICcomb_v{2,3}.py
for tagfile in config/reRecoTags/Cal_Nov2015_newGT_0T_v5.py
do
	echo
#	./scripts/removeRereco.sh -t $tagfile -f alcarereco_datasets.dat
#	./scripts/removeRereco.sh -t $tagfile -f ntuple_datasets.dat --json_name=$jsonName
#	continue

	for CHECK in ''  --check
	do
		case $tagfile in 
			*/Cal_Nov2015_PS_v1.py)
 			#./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK} #--singleEle 
				./scripts/RerecoQuick.sh -p Cal_Nov2015 -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK} --singleEle --weightsReco
				;;
			*)
				echo 
## 			./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK}
				./scripts/RerecoQuick.sh -p RUN2015CDBoff -t $tagfile  --alcarerecoOnly --json=$json25ns --json_name="noJSON" ${CHECK}
				;;
		esac
	done


	for CHECK in ''  --check
	do
		case $tagfile in 
			*/Cal_Nov2015_PS_v1.py)
#				./scripts/RerecoQuick.sh -p RUN2015CD    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly  --singleEle --weightsReco $CHECK
				./scripts/RerecoQuick.sh -p RUN2015CD    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly  --singleEle  $CHECK
				;;
			*)
				./scripts/RerecoQuick.sh -p RUN2015CDBoff    -t $tagfile --json=$json25ns --json_name=$jsonName --ntupleOnly  $CHECK
				;;
		esac
	done
done
exit 0
sleep 5m
#config/reRecoTags/Cal_Nov2015_PS_v{1,2,3,4,5}.py config/reRecoTags/phiSym_materialCorrection_ref.py 

#config/reRecoTags/Cal_Nov2015_PS_v{1,2,3,4,5}.py config/reRecoTags/phiSym_materialCorrection_ref.py 
for tagfile in config/reRecoTags/Cal_Nov2015_PS_v{1,2,3,4,5}.py
do
	./scripts/RerecoQuick.sh -p RUN2015D    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly  #--singleEle --doExtraCalibTree
done

sleep 15m
for tagfile in config/reRecoTags/Cal_Nov2015_PS_v{1,2,3,4,5}.py
do
	./scripts/RerecoQuick.sh -p RUN2015D    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly --check #--singleEle --doExtraCalibTree
	./scripts/RerecoQuick.sh -p RUN2015D    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly --check #--singleEle --doExtraCalibTree
done


exit 0
CHECK="--check"
for tagfile in config/reRecoTags/phiSym_materialCorrection_v{2,4,5,6,7,8,9,10}.py
do
	./scripts/RerecoQuick.sh -p RUN2015CD    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly ${CHECK} #--singleEle --doExtraCalibTree
#	./scripts/RerecoQuick.sh -p RUN2015C50ns -t $tagfile --json=$json50ns --json_name=246908-255031-Prompt_50ns_v2-bis --singleEle --doExtraCalibTree --ntupleOnly ${CHECK}
done

for tagfile in config/reRecoTags/phiSym_materialCorrection_v{2,4,5,6,7,8,9,10}.py
do
	./scripts/RerecoQuick.sh -p RUN2015CD    -t $tagfile --json=$json25ns --json_name=$jsonName  --ntupleOnly ${CHECK} #--singleEle --doExtraCalibTree
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




