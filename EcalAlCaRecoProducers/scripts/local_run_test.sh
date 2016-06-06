#
#tagMC=config/reRecoTags/80X_mcRun2_asymptotic_2016_v3.py 
#file="root://xrootd.unl.edu//store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/00000/008099CA-5501-E611-9AAE-24BE05BDCEF1.root"
##file="root://xrootd.unl.edu//store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3_ext1-v1/20000/0017320C-7BFC-E511-9B2D-0CC47A4C8E34.root"
#cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=file:$file maxEvents=10000 isCrab=0 tagFile=${tagMC} MC=1

#data
#json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-273450_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt
json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt
#json=/afs/cern.ch/work/g/gfasanel/80_ECALELF_ntuples_new/CMSSW_8_0_8/src/Calibration/EcalAlCaRecoProducers/json_test_original.txt
tag_data=config/reRecoTags/80X_dataRun2_Prompt_v8.py
file="root://xrootd.unl.edu//store/data/Run2016B/DoubleEG/MINIAOD/PromptReco-v2/000/273/158/00000/0227DB1C-E719-E611-872C-02163E0141F9.root"
cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=file:$file maxEvents=1000 isCrab=0 tagFile=${tag_data} jsonFile=${json} doTree=1
#if doTree=0 => nojsonFile applied