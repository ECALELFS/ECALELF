#./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep giuseppe` --skim ZSkim --type ALCARECO -t config/reRecoTags/phiSym_materialCorrection_ref.py

./scripts/prodAlcareco.sh `parseDatasetFile.sh alcareco_datasets.dat | grep miniAOD` --skim ZSkim --type ALCARECO -t config/reRecoTags/phiSym_materialCorrection_ref.py --scheduler=lsf