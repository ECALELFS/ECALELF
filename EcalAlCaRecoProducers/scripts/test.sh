#!/bin/bash
GT=80X_dataRun2_Prompt_v8
echo "============================================================"
echo "Script for sandbox validation on a small amount of events   "


echo "[INFO] Creating a user directory in /tmp"
local_dir=/tmp/$USER/sandboxValidation
mkdir -p $local_dir

myFile=`das_client --query="file dataset=/DoubleEG/Run2016B-v2/RAW" --limit 1 | grep store`

echo "[INFO] Copy RAW-RECO file in local: taking file from EOS @CERN"
xrdcp -v  $local_dir

cmsDriver.py myreco -s RAW2DIGI,RECO  --filein=$myFile --data --conditions=$ --era=Run2_2016  --scenario=pp --processName=reRECO --customise=L1Trigger/Configuration/customiseReEmul.L1TEventSetupForHF1x1TPs

echo "[INFO] Running on 1000 events, output in $local_dir"
cmsRun alcaSkimming.py files=file://$local_dir/RAW-RECO.root output=$local_dir/sandbox.root maxEvents=1000

echo "[INFO] Rerecoing with global tag 30Nov (in test.py)"
cmsRun alcaSkimming.py files=file://$local_dir/sandbox.root output=$local_dir/sandboxRereco.root

echo "[INFO] Copying official reco file in local"
xrdcp -v root://eoscms//eos/cms/store/group/alca_ecalcalib/sandbox/rereco30Nov-AOD.root $local_dir

echo "[INFO] Using reRecoValidation python for validation"
python reRecoValidation.py AOD > AOD.validationDump
python reRecoValidation.py sandboxRereco > sandboxRereco.validationDump

echo "[INFO] Checking the differences"


exit 0



