#! /bin/bash

echo $1 
if [[ $1 = "data" ]]; then
    echo "you are running locally on data"
    #file=root://xrootd.unl.edu//store/data/Run2015D/DoubleEG/MINIAOD/04Dec2015-v2/10000/0006F6FD-EE9D-E511-B3D4-00304865C426.root
    #file=root://xrootd.unl.edu//store/data/Run2015D/DoubleEG/MINIAOD/04Dec2015-v2/10000/002AF322-F49D-E511-9236-0025905A60BC.root
    file=root://xrootd.unl.edu//store/data/Run2015C_25ns/DoubleEG/MINIAOD/16Dec2015-v1/20000/00D3DF54-58B3-E511-BFE8-0025904C6414.root
    tag=config/reRecoTags/76X_dataRun2_v15.py
    tag_RUNC=config/reRecoTags/74X_dataRun2_EcalCalib_v1.py
    json_File=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt

    #cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=$file maxEvents=1000 isCrab=0 tagFile=${tag} jsonFile=${json_File}
    cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=$file maxEvents=1000 isCrab=0 tagFile=${tag_RUNC} jsonFile=${json_File}
    #Ti trovi una ntupla in locale:
    ls ntuple_numEvent1000.root
fi

if [[ $1 = "MC" ]]; then
    echo "you are running locally on MC"
    ## A noi interessa miniAOD v2
    file="root://xrootd.unl.edu//store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PU25nsData2015v1_HCALDebug_76X_mcRun2_asymptotic_v12-v1/00000/006C9F73-3FB9-E511-9AFE-001E67E95C52.root"
    tagMC=config/reRecoTags/76X_mcRun2_v12.py

    cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=file:$file maxEvents=1000 isCrab=0 tagFile=${tagMC} MC=1
fi