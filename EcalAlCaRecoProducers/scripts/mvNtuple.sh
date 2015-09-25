#!/bin/bash
# copy from the first directory to the second

usage(){
    echo "`basename $0` -u crab_workind_dir"
    echo " -u | --ui_working_dir crab_working_dir"
}

if ! options=$(getopt -u -o hu: -l help,ui_working_dir: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-u | --ui_working_dir) UI_WORKING_DIR=$2; shift;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

if [ -z "${UI_WORKING_DIR}" ]; then
    echo "[ERROR] crab working directory not specified" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


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


