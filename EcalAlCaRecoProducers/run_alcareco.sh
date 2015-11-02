./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep MINIAOD`  -s ZSkim --scheduler=remoteGlidein -t config/reRecoTags/phiSym_materialCorrection_ref.py --type MINIAODNTUPLE
