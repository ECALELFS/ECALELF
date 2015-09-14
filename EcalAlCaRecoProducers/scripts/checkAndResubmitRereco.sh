#!/bin/bash
usage(){
    echo "`basename $0` -t tagFile [--ntuple]"
}

if ! options=$(getopt -u -o ht: -l help,tagFile:,ntuple -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-t|--tagFile) TAGFILE=$2;shift;;
	--ntuple) NTUPLE=y;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

if [ ! -e "${TAGFILE}" ];then
    echo "[ERROR] tagfile ${TAGFILE} not found" >> /dev/stderr
    exit 1
fi

TAG=`basename ${TAGFILE} .py`

if [ -z "${NTUPLE}" ]; then 
    dirBase=prod_alcarereco; 
    dirs=${dirBase}/$TAG/*/*
else 
    dirBase=prod_ntuples/ALCARERECO; 
    dirs=${dirBase}/$TAG/*/*/*
fi

if [ ! -d "${dirBase}/$TAG" ];then
    echo "[ERROR] No directory found for tag $TAG in ${dirBase}" >> /dev/stderr
    exit 1
fi

echo -n "" > tmp/$TAG.log
for dir in $dirs
  do
  echo $dir 
  resubmitCrab.sh -u $dir
  if [ ! -e "$dir/res/finished" ];then
      echo $dir >> tmp/$TAG.log 
  fi
#  crab -c $dir -status -getoutput
done


