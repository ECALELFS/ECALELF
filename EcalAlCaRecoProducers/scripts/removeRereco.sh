#!/bin/bash
################
source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh
##############
#fileList=./alcarereco_datasets.dat
usage(){
    echo "`basename $0`"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir: origin files remote dir"
    echo "    --type arg"

    echo "    -f fileList: alcarereco_datasets.dat ntuple_datasets.dat"
    echo "    -t tagFile"
#    echo "    -p period"
    echo "    -l: list available rerecoes on EOS"
	echo "    --dry-run"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:p:f:lr:d:n: -l help,tag:,period:,datasetpath:,datasetname:,runrange:,store:,remote_dir:,type:,json_name:,dryRun -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-r|--runrange) RUNRANGE=$2; shift;;
	-d|--datasetpath) DATASETPATH=$2; shift ;;
	-n|--datasetname) DATASETNAME=$2; shift ;;
	--store) STORAGE_ELEMENT=$2; shift;;
	--remote_dir) USER_REMOTE_DIR_BASE=$2; shift;;
 	--type) TYPE=$2; shift;;
      	--json_name) JSONNAME=$2; shift;;

	-f) fileList=$2; shift;;
	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE"; TAG=`basename ${TAGFILE} .py`; shift;;
#	-p) PERIOD=$2; echo "[OPTION] PERIOD:$PERIOD"; shift;;
	-l) LIST=y;;
	--dryRun) DRYRUN=y;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

#------------------------------ checking
if [ -n "${LIST}" ];then
    echo "[STATUS] Listing available rerecoes on EOS"
    cut -f 6 $fileList  | sort | uniq
    exit 0
fi

if [ -z "${fileList}" ];then
    echo "[ERROR] fileList not defined" >> /dev/stderr
    exit 1
fi

if [ -z "${RUNRANGE}" ];then
    case ${fileList} in
	alcarereco_datasets.dat) ;;
	ntuple_datasets.dat) ;;
	*)
	    echo "[ERROR] `basename $0`: ${fileList} not alcarereco_datasets.dat either ntuple_datasets.dat" >> /dev/stderr
	    exit 1
	    ;;
    esac
	if [ -n "${JSONNAME}" ];then
		datasets=`parseDatasetFile.sh ${fileList} | grep -e "$TAG" | grep ${JSONNAME}`
	else
		datasets=`parseDatasetFile.sh ${fileList} | grep -e "$TAG"`
	fi
else 
    datasets=1
fi

if [ -z "${TAG}" -a "${TYPE}" == "ALCARERECO" ];then
    echo "[ERROR] TAG not defined" >> /dev/stderr
    exit 1
fi
if [ "${TAG}" != "${TAGFILE}" -a ! -r "$TAGFILE" ];then
    echo "[ERROR] TAGFILE not found or not readable" >> /dev/stderr
    exit 1
fi


#if [ -n "$PERIOD" ];then
#    datasets=`parseDatasetFile.sh ${fileList} | grep -e "$TAG$" | sed 's|$|,|' | grep "${PERIOD},"`
#else
#    datasets=`parseDatasetFile.sh ${fileList} | grep -e "$TAG$"`
#fi


# set IFS to newline in order to divide using new line the datasets
IFS=$'\n'

for dataset in $datasets
  do
#  echo $dataset
#  if [ -z "${SINGLEELE}" -a "`echo $dataset | grep -c SingleElectron`" != "0" ];then continue; fi
  if [ "$dataset" != "1" ];then
      RUNRANGE=`echo $dataset | cut -d ' ' -f 2`
      DATASETPATH=`echo $dataset | cut -d ' ' -f 4`
      DATASETNAME=`echo $dataset | cut -d ' ' -f 6`
      USER_REMOTE_DIR_BASE=`echo $dataset | cut -d ' ' -f 10`
      TYPE=`echo $dataset | awk '{for(i=1; i<=NF;i++){if(match($i,"type")){print $(i+1)}}}'`
      JSONNAME=`echo $dataset | awk '{for(i=1; i<=NF;i++){if(match($i,"json_name")){print $(i+1)}}}'`
  fi
  
  setEnergy $DATASETPATH 
  
  echo ${USER_REMOTE_DIR_BASE}
  if [ -z "${TYPE}" ];then
      setUserRemoteDirAlcarereco $USER_REMOTE_DIR_BASE
  else
      setUserRemoteDirNtuple ${USER_REMOTE_DIR_BASE}
  fi
  USER_REMOTE_DIR=/eos/cms/store/${USER_REMOTE_DIR}
  echo "Removing ${USER_REMOTE_DIR}"
  eos.select ls -s ${USER_REMOTE_DIR} &> /dev/null || {
      echo "${USER_REMOTE_DIR} not found" 
      if [ -z "${DRYRUN}" ];then sed  -i "\%$RUNRANGE.*$DATASETPATH.*$DATASETNAME.*$TAG.*$JSONNAME% d" $fileList; fi
      continue
  }

  if [ -z "${DRYRUN}" ];then
	echo ${USER_REMOTE_DIR}
    eos.select rm -r ${USER_REMOTE_DIR}  || exit 1

#    sed  -i "\%$RUNRANGE.*$DATASETPATH.*$DATASETNAME.*$TAG.*$JSONNAME% d" $fileList
  else
	  echo "[DRYRUN] $USER_REMOTE_DIR"
  fi
done

