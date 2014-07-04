#!/bin/bash

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
eval `scramv1 runtime -sh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh
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
#PATH=`echo $PATH | sed 's|/afs/cern.ch/project/eos/installation/pro/bin/||'`

PATH=$PATH:/afs/cern.ch/project/eos/installation/pro/bin/
PATH=$PATH:$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin

export ECALELFINIT=y

