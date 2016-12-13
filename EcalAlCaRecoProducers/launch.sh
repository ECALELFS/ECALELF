#!/bin/bash
jsonDCS=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt
#Cert_271036-275125_13TeV_PromptReco_Collisions16_JSON.txt

jsonName=DCSONLY
jsonName=271036-284044_23Sep2016_v1


CHECK=--check
#CHECK=--createOnly


#where=remoteGlidein
scheduler=caf
tag_MC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py
tag_Prompt=config/reRecoTags/80X_dataRun2_Prompt_v14.py
tag_Rereco=config/reRecoTags/80X_dataRun2_2016SeptRepro_v4.py

fileList=alcareco_datasets.dat
PERIOD=MORIOND2017

IFS=$'\n'
datasetsData=(`./scripts/parseDatasetFile.sh $fileList | grep VALID | sed 's|$|,|' | grep "${PERIOD},"`)
datasetsMC=(`./scripts/parseDatasetFile.sh $fileList | grep VALID | sed 's|$|,|' | grep "MCICHEP2016,"`)
# set IFS to newline in order to divide using new line the datasets



for dataset in ${datasetsMC[@]}
do
	datasetName=`echo $dataset | awk '{print $6}'`
	echo $datasetName
#	continue
	case $datasetName in
		*MINIAODSIM)
#			echo $datasetName
			./scripts/prodNtuples.sh  --isMC --type=MINIAOD -t ${tag_MC} -s noSkim --scheduler=${scheduler} --doEleIDTree --extraName=newNtuples ${CHECK} $dataset
			;;
		*-withES)
			echo "--> $datasetName"
			./scripts/prodAlcareco.sh --isMC --type=ALCARECO -t ${tag_MC} -s ZSkim --scheduler=${scheduler}  ${CHECK} $dataset
			./scripts/prodNtuples.sh  --isMC --type=ALCARECO -t ${tag_MC} -s ZSkim --scheduler=${scheduler} --doEleIDTree --extraName=newNtuples ${CHECK} $dataset
			;;
	esac
done
exit 0
for dataset in ${datasetsData[@]}
  do
	
	if [ "`echo $dataset | grep -c SingleElectron`" != "0" -a "`echo $dataset | grep -c ZElectron`" != "0" ];then continue; fi
	datasetName=`echo $dataset | awk '{print $4}'`
#	echo $dataset
#	continue
	case $datasetName in
		*PromptReco*)
			./scripts/prodNtuples.sh  --type=MINIAOD -t ${tag_Prompt} -s noSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} --doEleIDTree --extraName=newNtuples ${CHECK} ${dataset} || exit 1
			;;
		*)
			./scripts/prodNtuples.sh  --type=MINIAOD -t ${tag_Rereco} -s noSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} --doEleIDTree --extraName=newNtuples ${CHECK} ${dataset} || exit 1
			;;
	esac
	
done
	
exit 0
while [ "1" == "1" ];do
sleep 15m
done
exit 0

#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep -v MINIAOD | head -1 | tail -1` --type ALCARECO -t ${tag_Data} -s ZSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} #${CHECK}
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016B | grep -v MINIAOD | head -2 | tail -1` --type ALCARECO -t ${tag_Data} -s ZSkim  --json=${json} --json_name=${jsonName} --scheduler=${scheduler} #${CHECK}
#./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep Run2016C | grep -v  MINIAOD | head -2 | tail -1` --type ALCARECO -t ${tag_Data} -s ZSkim --json=${json} --json_name=${jsonName} --scheduler=${scheduler} #${CHECK}

#



# while [ ! -e "prod_alcareco/DYJets_madgraph-RunIISpring16/allRange/res/finished" ] || [ ! -e "prod_alcareco/DYJets_amcatnlo-RunIISpring16/allRange/res/finished" ]
# do
# sleep 5m	
# ./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -1` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}
	
# ./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep RunIISpring16 | grep -v MINIAODSIM | head -2` --isMC --type ALCARECO -t ${tag_MC} -s ZSkim  --scheduler=${scheduler} ${CHECK}

# done


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

