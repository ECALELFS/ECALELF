#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh
case $USER in
    hengne|heli|gfasanel)
	voms-proxy-init -voms cms -hours 192 -valid 192:00
	;;
    *)
	voms-proxy-init -voms cms -out $HOME/gpi.out
	;;
esac

PATH=$PATH:/afs/cern.ch/project/eos/installation/pro/bin/
PATH=$PATH:$CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/bin

echo "[CMSSW] " ${CMSSW_BASE}
echo "[CRAB3] " $(which crab) 

export ECALELFINIT=y

