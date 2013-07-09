#!/bin/bash
# copy from the first directory to the second


UI_WORKING_DIR=$1
USER_REMOTE_DIR=`grep '^user_remote_dir=' ${UI_WORKING_DIR}/share/crab.cfg |cut -d '=' -f 2` 
STORAGE_PATH=`grep 'storage_path=' ${UI_WORKING_DIR}/share/crab.cfg  |cut -d '=' -f 2`


NTUPLE_REMOTE_DIR=${STORAGE_PATH}/${USER_REMOTE_DIR}

ALCARERECO_REMOTE_DIR=`cat ${UI_WORKING_DIR}/share/arguments.xml | grep InputFiles | sed 's|.*InputFiles="||g;s|/[^/]*\.root.*||;' | sort | uniq | sed 's|root://eoscms.cern.ch/||;s|root://eoscms/||'`


# make the list of files with "ntuple" in the name 
makefilelist.sh -g ntuple ntupleList $ALCARERECO_REMOTE_DIR || exit 1


if [ ! -d "/tmp/$USER" ];then
    mkdir -p /tmp/$USER
fi

for file in `cat filelist/ntupleList.list`
  do
  file=`echo $file | sed 's|root://eoscms.cern.ch/||'`
  newFile=`echo $file | sed "s|${ALCARERECO_REMOTE_DIR}|${NTUPLE_REMOTE_DIR}|"`
#  echo $ALCARERECO_REMOTE_DIR $file $newFile
  eos.select cp $file $newFile || exit 1
  eos.select rm $file
done
rm filelist/ntupleList.list


