#source ../initCmsEnv.sh
#cmsenv
./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep giuseppe` --skim ZSkim --type ALCARECO -t config/reRecoTags/phiSym_materialCorrection_ref.py

#--jobname test_giuseppe

#./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep MINIAOD`  -s ZSkim --scheduler=remoteGlidein -t config/reRecoTags/phiSym_materialCorrection_ref.py --createOnly
#scheduler only for crab2
#--isMC
#./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep giuseppe`  -s ZSkim --scheduler=remoteGlidein -t config/reRecoTags/phiSym_materialCorrection_ref.py --createOnly
#--type MINIAODNTUPLE
