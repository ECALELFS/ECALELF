#!/bin/tcsh

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
cmsenv
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
voms-proxy-init -voms cms -out $HOME/gpi.out

#cd calibration/SANDBOX
#setenv PATH $PWD/bin:$PATH
setenv PATH $PATH":/afs/cern.ch/project/eos/installation/pro/bin/"
setenv PATH $PATH":$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin"

