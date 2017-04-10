#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh
case $USER in
    *)
		voms-proxy-init -voms cms -out $HOME/gpi.out
		;;
esac

PATH=$PATH:/afs/cern.ch/project/eos/installation/pro/bin/
PATH=$PATH:$CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/bin
PATH=$PATH:$HOME/scratch0/LSFsubmit
echo "[CMSSW] " ${CMSSW_BASE}
echo "[CRAB3] " $(which crab) 

export ECALELFINIT=y

