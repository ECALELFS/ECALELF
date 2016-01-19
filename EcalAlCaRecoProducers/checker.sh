source ../initCmsEnvCRAB2.sh
#where=lsf
where=remoteGlidein
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt
jsonName=246908-260627-Prompt_25ns-v1-golden_silver
tag=config/reRecoTags/74X_dataRun2_Prompt_v4.py

./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep emanuele-ZElectron_Run2015D_v3` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} --check

./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep emanuele-ZElectron_Run2015D_v4` --type MINIAOD -t ${tag} --scheduler=${where} --json=${json} --json_name=${jsonName} --check