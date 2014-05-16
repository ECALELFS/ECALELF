#!/bin/bash

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
eval `scramv1 runtime -sh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh
#voms-proxy-init -voms cms -out $HOME/gpi.out
voms-proxy-init -voms cms

user=`who am i| awk {'print $1'}`
cert=`ls -l /tmp/x509* | grep heli | awk {'print $9'}`
cert=`basename $cert`
cp /tmp/$cert $CMSSW_BASE/src/data/ 

#cd calibration/SANDBOX
#setenv PATH $PWD/bin:$PATH
#PATH=`echo $PATH | sed 's|/afs/cern.ch/project/eos/installation/pro/bin/||'`

PATH=$PATH:/afs/cern.ch/project/eos/installation/pro/bin/
PATH=$PATH:$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin

export ECALELFINIT=y

