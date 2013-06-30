#!/bin/bash

if [ "`basename $PWD`" != "SANDBOX" ];then
    echo "[ERROR] Command to be executed in SANDBOX subdir" >> /dev/stderr
    exit 1
fi

rerecoList="Cal_Nov2012 Cal_Nov2012_ICcombAllR9_v1"
period=Cal_Nov2012
#singleEle="--singleEle"
json=
json_name=

for rereco in ${rerecoList}
  do
# check if python files exist
  if [ ! -e  "config/reRecoTags/$rereco.py" ];then
      echo "Config file for rereco ${rereco} not found " >> /dev/stderr
      exit 1
  fi

  # create all the tasks
  ./scripts/SandboxQuick.sh -t $tag -p ${period} ${singleEle} --rereco --createOnly || exit 1
done

for rereco in ${rerecoList}
  do
  # submit all the tasks
  ( ./scripts/SandboxQuick.sh -t $tag -p ${period} ${singleEle} --rereco --submitOnly || exit 1 ) &
done

# aspetta che tutti i comandi precendenti siano finiti (le rereco)
wait

cd ../ZNtupleDumper

# ntuple production
for rereco in ${rerecoList}
  do
  # create all the tasks
  ../SANDBOX/scripts/SandboxQuick.sh -t $tag -p ${period} --ntuple --json=${json} --json_name=${json_name} --createOnly || exit 1 
done

for rereco in ${rerecoList}
  do
  # submit all the tasks
  ( ../SANDBOX/scripts/SandboxQuick.sh -t $tag -p ${period} --ntuple --json=${json} --json_name=${json_name} --submitOnly || exit 1 ) & 
done

wait 
echo "[STATUS] All rereco and ntuple produced and merged" 

