#!/bin/tcsh

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
eval `scramv1 runtime -csh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
case $USER in
    hengne|heli)
	voms-proxy-init -voms cms -hours 192 -valid 192:00
	;;
    *)
	voms-proxy-init -voms cms -out $HOME/gpi.out
	;;
esac

#cd calibration/SANDBOX
#setenv PATH $PWD/bin:$PATH
setenv PATH $PATH":/afs/cern.ch/project/eos/installation/pro/bin/"
setenv PATH $PATH":$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin"

setenv ECALELFINIT y

