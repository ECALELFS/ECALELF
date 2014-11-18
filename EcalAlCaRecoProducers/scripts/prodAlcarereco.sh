#!/bin/bash
#source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh
source ./scripts/prodFunctions.sh

############################### OPTIONS
#------------------------------ default
SKIM=none
USEPARENT=0
SCHEDULER=caf
USESERVER=0
STORAGE_ELEMENT=caf
#UI_WORKING_DIR=prod_alcarereco
USER_REMOTE_DIR_BASE=group/alca_ecalcalib/ecalelf/alcarereco
NTUPLE_REMOTE_DIR_BASE=group/alca_ecalcalib/ecalelf/ntuples
#LUMIS_PER_JOBS=2000
CREATE=y
SUBMIT=y
DOTREE=1
TYPE=ALCARERECO
usage(){
    echo "`basename $0` {parseDatasetFile options} -t tagFile [options]"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir: alcaraw files remote dir"
    echo "---------- provided by command-line (mandatory)"
    echo "    -t tagFile"
    echo "---------- optional common"
    echo "      json applied only at ntuple level!"
    echo "    --json_name jsonName: additional name in the folder structure to keep track of the used json"
    echo "    --json jsonFile.root"
    echo "    --doExtraCalibTree"
    echo "    --doEleIDTree"
    echo "    --noStandardTree"
    echo "    --alcarerecoOnly"
    echo "    --createOnly"
    echo "    --submitOnly"
    echo "    --check"
    echo "----------"
    echo "    --tutorial: tutorial mode, produces only one sample in you user area"
    echo "    -H, --expertHelp"
}

expertUsage(){
    echo "------------------------------ Expert options"
    echo "    --rereco_remote_dir dir: output remote dir (=${USER_REMOTE_DIR_BASE})"
#    echo "    --ntuple_store arg: storage element for the ntuples (=${STORAGE_ELEMENT})"
#    echo "    --ntuple_remote_dir dir (=${USER_REMOTE_DIR_BASE})"
    echo "    --scheduler caf,lsf,remoteGlidein (=${SCHEDULER})"
    echo "    -f, --filelist arg: produce alcarereco from a list of files"
    echo "    -s, --skim arg: apply the skim (ZSkim, WSkim)"
    echo "    -u, --ui_working_dir arg: crab task folder (=${UI_WORKING_DIR})"
#    echo "    --file_per_job arg: number of files to process in 1 job (=1)"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hHd:n:s:r:t:u: -l help,expertHelp,file_per_job:,nJobs:,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,rereco_remote_dir:,ui_working_dir:,scheduler:,createOnly,submitOnly,check,json:,json_name:,doExtraCalibTree,doEleIDTree,noStandardTree,alcarerecoOnly,tutorial -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-H|--expertHelp)  expertUsage; exit 0;;
	-r|--runrange) RUNRANGE=$2; shift;;
	-d|--datasetpath) DATASETPATH=$2; shift ;;
	-n|--datasetname) DATASETNAME=$2; shift ;;
	--store) STORAGE_ELEMENT=$2; shift;;
	--remote_dir) ALCARAW_REMOTE_DIR_BASE=$2; shift;;
 	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE";shift;;

 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
	--doExtraCalibTree) DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --doExtraCalibTree";;
	--doEleIDTree)      DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --doEleIDTree";;
	--noStandardTree)   DOEXTRACALIBTREE="${DOEXTRACALIBTREE} --noStandardTree";;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--check)      echo "[OPTION] checking jobs"; unset CREATE; unset SUBMIT; CHECK=y; EXTRAOPTION="--check";;
	--alcarerecoOnly) echo "[OPTION] alcarerecoOnly"; DOTREE=0;;
	--rereco_remote_dir) USER_REMOTE_DIR_BASE=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	-f|--filelist) FILELIST="$FILELIST $2"; echo ${FILELIST}; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
	-u | --ui_working_dir) UI_WORKING_DIR=$2; shift;;
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
fi

if [ -z "$DATASETNAME" ];then 
    echo "[ERROR] DATASETNAME not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
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

if [ -z "$ALCARAW_REMOTE_DIR_BASE" ];then 
    echo "[ERROR] ALCARAW_REMOTE_DIR_BASE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


if [ ! -r "$TAGFILE" ];then
    echo "[ERROR] TAGFILE not found or not readable" >> /dev/stderr
    exit 1
fi


if [ -n "${TUTORIAL}" ];then
    case $DATASETPATH in
	/DoubleElectron/Run2012A-ZElectron-22Jan2013-v1/RAW-RECO)
	    ;;
	/DoubleElectron/Run2012B-ZElectron-22Jan2013-v1/RAW-RECO)
	    ;;
	/DoubleElectron/Run2012C-ZElectron-22Jan2013-v1/RAW-RECO)
	    ;;
	/DoubleElectron/Run2012D-ZElectron-22Jan2013-v1/RAW-RECO)
	    ;;
	*)
	    echo "[ERROR] With the tutorial mode, the only permitted datasetpaths are:"
	    echo "        /DoubleElectron/Run2012A-ZElectron-22Jan2013-v1/RAW-RECO"
	    echo "        /DoubleElectron/Run2012B-ZElectron-22Jan2013-v1/RAW-RECO"
	    echo "        /DoubleElectron/Run2012C-ZElectron-22Jan2013-v1/RAW-RECO"
	    echo "        /DoubleElectron/Run2012D-ZElectron-22Jan2013-v1/RAW-RECO"
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

echo "[INFO] RUNRAGE=$RUNRANGE"
echo "[INFO] DATASETPATH=$DATASETPATH"
echo "[INFO] DATASETNAME=$DATASETNAME"
echo "[INFO] ALCARAW_REMOTE_DIR_BASE=$ALCARAW_REMOTE_DIR_BASE"
echo "[INFO] USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE:=alcarereco}"
echo "[INFO] NTUPLE_REMOTE_DIR_BASE=${NTUPLE_REMOTE_DIR_BASE}"
#echo "[INFO] TAGFILE=$TAGFILE"

###############################

#------------------------------
TAG=`basename $TAGFILE .py`

#Setting the ENERGY variable
setEnergy $DATASETPATH

setStoragePath $STORAGE_ELEMENT $SCHEDULER 

case $SCHEDULER in
    caf)
	USESERVER=0
	;;
    lsf)
	USESERVER=0
	;;
    *)
	echo "[ERROR] SCHEDULER not accepted: only caf and lsf" >> /dev/stderr
	usage >> /dev/stderr
	exit 1
	;;
esac



ALCARAW_REMOTE_DIR=$ALCARAW_REMOTE_DIR_BASE/${ENERGY}/${DATASETNAME}/${RUNRANGE}
setUserRemoteDirAlcarereco $USER_REMOTE_DIR_BASE
#USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${TAG}/${DATASETNAME}/${RUNRANGE}

NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR_BASE/${ENERGY}/${TYPE}
if [ -n "${TAG}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${TAG}; fi
NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${DATASETNAME}/${RUNRANGE}
if [ -n "${JSONNAME}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${JSONNAME}; fi
if [ -n "${EXTRANAME}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${EXTRANAME}; fi

#check if rereco already done
#if [ -n "${NTUPLE}" ];then DOTREE=1; fi

echo "[INFO] UI_WORKING_DIR=${UI_WORKING_DIR:=prod_alcarereco/${TAG}/${DATASETNAME}/${RUNRANGE}/${TUTORIAL}}"

#==============================
if [ -n "${CREATE}" ];then

checkRelease ${DATASETPATH}

if [ -z "${TUTORIAL}" ];then
    if [ "`cat alcarereco_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
	echo "[WARNING] Rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
	if [ "${DOTREE}" == "1" ]; then
	    echo "          Doing only ntuples"
	    if [ "`cat ntuple_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
		echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
		exit 0
	    fi
	    ./scripts/prodNtuples.sh -r ${RUNRANGE} -d ${DATASETPATH} -n ${DATASETNAME} --store ${STORAGE_ELEMENT} --remote_dir ${USER_REMOTE_DIR_BASE} --type=ALCARERECO --json=${JSONFILE} --json_name=${JSONNAME} -t ${TAGFILE} ${DOEXTRACALIBTREE} ${EXTRAOPTION}
	    exit 0
	fi
	exit 0
    fi
    
    if [ "`cat ntuple_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
	echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
	echo "          Doing only alcarereco"
	DOTREE=0
    fi
fi

if [ -z "$FILELIST" ];then
    sample=tempFileList-${DATASETNAME}-${RUNRANGE}-${TAG}
    makefilelist.sh $sample $STORAGE_PATH/$ALCARAW_REMOTE_DIR  || exit 1
    FILELIST=filelist/$sample.list
fi

nFiles=`cat $FILELIST | wc -l`
if [ -n "$NJOBS" ];then

    let FILE_PER_JOB=$nFiles/$NJOBS
    if [ "`echo \"$nFiles%$NJOBS\" | bc`" != "0" ];then
	let FILE_PER_JOB=$FILE_PER_JOB+1
    fi
elif [ -n "$FILE_PER_JOB" ];then
    let NJOBS=$nFiles/$FILE_PER_JOB
    if [ "`echo \"$nFiles%$FILE_PER_NJOB\" | bc`" != "0" ];then
	let NJOBS=$NJOBS+1
    fi
else
    NJOBS=$nFiles
    FILE_PER_JOB=1
fi


#==============================
crabFile=tmp/alcarereco.cfg
cat > $crabFile <<EOF
[CRAB]
#use_server = 0
jobtype = cmssw
scheduler = $SCHEDULER

[LSF]
queue = 1nd
[CAF]
queue = cmscaf1nd


[CMSSW]
allow_NonProductionCMSSW = 1
datasetpath=None

pset=python/alcaSkimming.py
pycfg_params=output=alcarereco.root type=ALCARERECO tagFile=${TAGFILE} doTree=${DOTREE} doTreeOnly=0 jsonFile=${JSONFILE} secondaryOutput=ntuple.root isCrab=1

runselection=${RUNRANGE}
split_by_run=0
total_number_of_events=${NJOBS}
number_of_jobs=${NJOBS}

output_file=ntuple.root
get_edm_output=1
check_user_remote_dir=1
use_parent=${USEPARENT}


[USER]
ui_working_dir=$UI_WORKING_DIR
return_data = 0
copy_data = 1

storage_element=caf.cern.ch
user_remote_dir=$USER_REMOTE_DIR
storage_path=$STORAGE_PATH

thresholdLevel=50
eMail = shervin@cern.ch

[GRID]

rb = HC
rb = CERN
proxy_server = myproxy.cern.ch

EOF
fi

if [ -n "${CREATE}" ];then
    crab -cfg ${crabFile} -create || exit 1
    makeArguments.sh -f filelist/$sample.list -u $UI_WORKING_DIR -n $FILE_PER_JOB || exit 1 
    ./scripts/splittedOutputFilesCrabPatch.sh -u ${UI_WORKING_DIR}
#crabMonitorID.sh -r ${RUNRANGE} -n $DATASETNAME -u ${UI_WORKING_DIR} --type ALCARECO
    rm filelist/$sample* -Rf
fi

if [ -n "${SUBMIT}" ]; then
    crab -c ${UI_WORKING_DIR} -submit all
    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TAG}"
    echo -e $STRING >> alcarereco_datasets.dat
    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${NTUPLE_REMOTE_DIR_BASE}\t${TYPE}\t${TAG}\t${JSONNAME}"
    echo -e $STRING >> ntuple_datasets.dat


elif [ -n "${CREATE}" ];then
    echo "##############################"
    echo "To start the crab jobs:"
    echo "crab -c ${UI_WORKING_DIR} -submit all"
fi

if [ -n "${CHECK}" ];then
    resubmitCrab.sh -u ${UI_WORKING_DIR}
    if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
	#echo $dir >> tmp/$TAG.log 
	echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
    else
	mergeOutput.sh -u ${UI_WORKING_DIR} -g ntuple --merged_remote_dir=${NTUPLE_REMOTE_DIR}
    fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
fi



