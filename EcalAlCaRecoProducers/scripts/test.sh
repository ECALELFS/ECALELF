xrdcp -v  $local_dir


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



