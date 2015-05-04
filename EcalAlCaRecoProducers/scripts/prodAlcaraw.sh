#!/bin/bash
source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh
#source /tmp/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh

############################### OPTIONS
#------------------------------ default
USEPARENT=0
SCHEDULER=caf
USESERVER=0
TYPE=ALCARAW
LUMIS_PER_JOBS=1000  # for ZSkim events is good, WSkim events /=4, SingleElectron /=10
BLACK_LIST=T2_EE_Estonia
CREATE=yes
SUBMIT=yes
OUTPUTFILE=alcaraw
crabFile=tmp/alcaraw.cfg

usage(){
    echo "`basename $0` options"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir"
    echo "---------- optional"
    echo "    -s skim: ZSkim, WSkim, EleSkim: ZSkim for DoubleElectron and WSkim for SingleElectron are automatically activated"
    echo "    --scheduler caf,lsf,remoteGlidein,condor (=${SCHEDULER})"
    echo "    --createOnly"
    echo "    --submitOnly"
    echo "    --check"
    echo "----------"
    echo "    --tutorial: tutorial mode, produces only one sample in you user area"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hd:n:s:r: -l help,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,scheduler:,white_list:,black_list:,createOnly,submitOnly,check,tutorial -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-d|--datasetpath) DATASETPATH=$2; shift ;;
	-n|--datasetname) DATASETNAME=$2; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
	-r|--runrange) RUNRANGE=$2; shift;;
	--store) STORAGE_ELEMENT=$2; shift;;
	--remote_dir) USER_REMOTE_DIR_BASE=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	--white_list) WHITELIST=$2; shift;;
	--black_list) BLACKLIST=$2; shift;;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--check)      echo "[OPTION] checking jobs"; unset CREATE; unset SUBMIT; CHECK=y; EXTRAOPTION="--check";;
        --tutorial)   echo "[OPTION] Activating the tutorial mode"; TUTORIAL=y;;
	(--) shift; break;;
	(-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

#------------------------------ checking
if [ -z "$DATASETPATH" ];then 
    echo "[ERROR] DATASETPATH not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
#else
#    echo "[INFO] DATASETPATH=${DATASETPATH}"
fi


	
if [ -z "$DATASETNAME" ];then 
    echo "[ERROR] DATASETNAME not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
#else
#    echo "[INFO] DATASETNAME=${DATASETNAME}"
fi

if [ -z "$RUNRANGE" ];then 
    echo "[ERROR] RUNRANGE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$STORAGE_ELEMENT" ];then 
    echo "[ERROR] STORAGE_ELEMENT not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


if [ -z "$USER_REMOTE_DIR_BASE" ];then 
    echo "[ERROR] USER_REMOTE_DIR_BASE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
#else
#    echo "[INFO] USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE}"
fi

if [ -n "${TUTORIAL}" ];then
    case $DATASETPATH in
	/DoubleElectron/Run2012A-ZElectron-22Jan2013-v1/RAW-RECO)
	    ;;
	*)
	    echo "[ERROR] With the tutorial mode, the only permitted datasetpath is:"
	    echo "        /DoubleElectron/Run2012A-ZElectron-22Jan2013-v1/RAW-RECO"
	    echo "        Be sure to have it in alcaraw_datasets.dat and to have selected it using the parseDatasetFile.sh"
	    exit 1
	    ;;
    esac
    USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE}/tutorial/$USER
    echo "============================================================"
    echo "= [INFO] With the tutorial mode, the output goes into the directory:"
    echo "=       ${USER_REMOTE_DIR_BASE}"
    echo "= --> Please, remember to remove it after the tests (ask to shervin@cern.ch)"
    /bin/sleep 5s
fi

echo "[INFO] ${RUNRANGE} ${DATASETPATH} ${DATASETNAME} ${USER_REMOTE_DIR_BASE}"
###############################

#------------------------------
case $DATASETPATH in
    */RECO)
	echo "[INFO] Dataset is RECO, using parent for RAW"
	USEPARENT=1
	;;
    */AOD)
	echo "[ERROR] cannot produce ALCARAW from AOD!!!" >> /dev/stderr
	exit 1
	;;
    */RAW-RECO)
	USEPARENT=0
	;;
    */USER)
	USEPARENT=0
	;;
    */RAW)
	USEPARENT=0
	;;
    *)
	echo "[ERROR] Dataset format not recognized: ${DATASETPATH}"
	exit 1
	;;
esac

if [ -z "${SKIM}" ];then
    case $DATASETPATH in
	*DoubleElectron*)
	    SKIM=ZSkim
	    ;;
	*SingleElectron*USER)
	    SKIM=fromWSkim
	    let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/4
	    ;;
	*SingleElectron*RAW)
	    SKIM=WSkim
	    #let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/20
	    ;;
	*SingleElectron*)
	    SKIM=WSkim
	    let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/10
	    ;;
	*ZElectron*)
	    SKIM=ZSkim
	    ;;
    esac
fi
case $DATASETPATH in
	*SingleElectron*USER)
	    let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/4
	    ;;
	*SingleElectron*)
	    let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/10
esac

echo "[INFO] Applying skim: ${SKIM}"

#Setting the ENERGY variable
setEnergy $DATASETPATH

case $DATASETPATH in 
    *Run2012*)
	;;
    *Run2011*)
	let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}*3
	let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/2
	;;
    *Run2010*)
	let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}*3
	let LUMIS_PER_JOBS=${LUMIS_PER_JOBS}/2
	;;
esac

setStoragePath $STORAGE_ELEMENT $SCHEDULER 

checkRelease ${DATASETPATH}

USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${DATASETNAME}/${RUNRANGE:-allRange}
UI_WORKING_DIR=prod_alcaraw/${DATASETNAME}/${RUNRANGE}


### outfiles in case of splitting of the output 
index=001
#OUTFILES=$OUTPUTFILE$index.root
#for index in `seq -f %03.0f 2 9`
#  do
#  OUTFILES="$OUTFILES,$OUTPUTFILE$index.root"
#done


###

cat > ${crabFile} <<EOF
[CRAB]
#use_server = ${USESERVER}
jobtype = cmssw
scheduler = ${SCHEDULER}

[LSF]
queue = 1nd
[CAF]
queue = cmscaf1nd


[CMSSW]
datasetpath=${DATASETPATH}

pset=python/alcaSkimming.py
pycfg_params=output=${OUTPUTFILE}.root skim=${SKIM} type=${TYPE}

runselection=${RUNRANGE}
split_by_run=0
total_number_of_lumis = -1
lumis_per_job=$LUMIS_PER_JOBS

#output_file=${OUTFILES}
get_edm_output=1
check_user_remote_dir=1
use_parent=${USEPARENT}


[USER]
ui_working_dir=$UI_WORKING_DIR
return_data = 0
copy_data = 1
local_stage_out = 0
storage_element=$STORAGE_ELEMENT
user_remote_dir=$USER_REMOTE_DIR
storage_path=$STORAGE_PATH

thresholdLevel=50
eMail = shervin@cern.ch

[GRID]
rb = HC
rb = CERN
proxy_server = myproxy.cern.ch
#se_white_list=$WHITELIST
#se_black_list=$BLACKLIST

EOF

if [ -n "${CREATE}" ];then
    crab -cfg ${crabFile} -create
    ./scripts/splitArguments.sh -u ${UI_WORKING_DIR}
    if [ -d "${UI_WORKING_DIR}/sub-0" ];then
	UI_WORKING_DIRS=`ls -d ${UI_WORKING_DIR}/sub-*`
    else
	UI_WORKING_DIRS=${UI_WORKING_DIR}
    fi
    for ui_working_dir in ${UI_WORKING_DIRS}
      do
      ./scripts/splittedOutputFilesCrabPatch.sh -u ${ui_working_dir}
    done

#crabMonitorID.sh -r ${RUNRANGE} -n $DATASETNAME -u ${UI_WORKING_DIR} --type ALCARAW
fi

if [ -d "${UI_WORKING_DIR}/sub-0" ];then
    UI_WORKING_DIRS=`ls -d ${UI_WORKING_DIR}/sub-*`
else
    UI_WORKING_DIRS=${UI_WORKING_DIR}
fi

if [ -n "$SUBMIT" ]; then
    if [ -z "${TUTORIAL}" ];then
	for ui_working_dir in ${UI_WORKING_DIRS}
	  do
	  echo "[STATUS] Submitting ${ui_working_dir}"
	  crab -c ${ui_working_dir} -submit all
	done
    else
	crab -c ${UI_WORKING_DIR} -submit 1
    fi
else
    echo "##############################"
    echo "To start the crab jobs:"
    set -- $options
    echo -n $0
    while [ $# -gt 0 ]
      do
      case $1 in
	  --createOnly) shift;;
	  (--) shift; break;;
	  (*)   echo -n " $1"; shift;;
      esac
    done  
    echo " --submitOnly"
    echo " or"
    echo "crab -c ${UI_WORKING_DIR} -submit all"
fi

if [ -n "${CHECK}" ];then
    for ui_working_dir in ${UI_WORKING_DIRS}
      do
      echo "[STATUS] Checking ${ui_working_dir}"
      resubmitCrab.sh -u ${ui_working_dir}
    done
    if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
	#echo $dir >> tmp/$TAG.log 
	echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
#    else
#	mergeOutput.sh -u ${UI_WORKING_DIR}
    fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
fi

