#!/bin/sh


echo "INITDATA:: PWD=$PWD"
echo "INITDATA:: ls -l "
ls -l
echo "INITDATA:: ls -l $CMSSW_BASE/src/data/ "
ls -l $CMSSW_BASE/src/data/
echo "INITDATA:: who am i "
who am i
whoami
id -nu
user=`whoami`
cert=`ls -l $CMSSW_BASE/src/data/x509* | grep $user | awk {'print $9'}`
cert=`basename $cert`
cp $CMSSW_BASE/src/data/$cert /tmp/$cert

echo "INITDATA:: user=$user ; cert=$cert ; CMSSW_BASE=$CMSSW_BASE "
echo "INITDATA:: ls -l /tmp/$cert"
ls -l /tmp/$cert

echo "INITDATA:: RUNTIME_AREA=$RUNTIME_AREA"
echo "INITDATA:: NJob=$NJob"

echo "INITDATA:: start run cmsRun " 
cmsRun -j $RUNTIME_AREA/crab_fjr_$NJob.xml -p pset.py 
echo "INITDATA:: end run cmsRun"

