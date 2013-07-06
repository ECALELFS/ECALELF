#!/bin/bash
source $CMSSW_BASE/src/calibration/ALCARAW_RECO/scripts/prodFunctions.sh

############################### OPTIONS
#------------------------------ default
USESERVER=0
SCHEDULER=caf
STORAGE_ELEMENT=caf
isMC=0
UI_WORKING_DIR=prod_ntuples
USER_REMOTE_DIR_BASE=group/alca_ecalcalib/ecalelf/ntuples
LUMIS_PER_JOBS=2000
DOEXTRACALIBTREE=0
CREATE=y
SUBMIT=y
DOTREE=1

usage(){
    echo "`basename $0` {parseDatasetFile options} --type={type} [options]"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir: origin files remote dir"
    echo "---------- provided by command-line (mandatory)"
    echo "    --type alcareco|alcarecosim|ALCARERECO:"
    echo "           alcareco: produced on data"
    echo "           ALCARECOSIM|alcarecosim: alcareco produced on MC"
    echo "           ALCARERECO: alcareco format after rereco on ALCARAW"
    echo " *** for MC ***"
    echo "    --isMC"
    echo " *** for DATA ***"
    echo "    --json_name jsonName: additional name in the folder structure to keep track of the used json"
    echo "    --json jsonFile.root"

    echo "---------- optional common"
    echo "    --doExtraCalibTree"
    echo "    --doEleIDTree"
    echo "    --noStandardTree"
    echo "    --createOnly"
    echo "    --submitOnly"
    echo "    --check"

    echo "----------"
    echo "    --tutorial: tutorial mode, produces only one sample in you user area"
    echo "    -H, --expertHelp"
}

expertUsage(){
    echo "------------------------------ Expert options"
    echo "    --ntuple_store arg: storage element for the ntuples (=${STORAGE_ELEMENT})"
    echo "    --ntuple_remote_dir dir (=${USER_REMOTE_DIR_BASE})"
    echo "    --scheduler caf,lsf,remoteGlidein (=${SCHEDULER})"
    echo "    -f, --filelist arg: produce ntuples from a list of files"
    echo "    -s, --skim arg: apply the skim (ZSkim, WSkim)"
    echo "    -t, --tag arg: tag name of the alcarereco"
    echo "    --ui_working_dir arg: crab task folder (=${UI_WORKING_DIR})"
    echo "    --extraName arg: additional name for folder structure (to make different versions) (='')"
    echo "    --file_per_job arg: number of files to process in 1 job (=1)"
}

    
#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hHd:n:s:r:t:f: -l help,expertHelp,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,scheduler:,isMC,ntuple_remote_dir:,json:,tag:,type:,json_name:,sandbox_remote_dir:,ui_working_dir:,extraName:,doExtraCalibTree,doEleIDTree,noStandardTree,createOnly,submitOnly,check,file_per_job: -- "$@")
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
	--store) ORIGIN_STORAGE_ELEMENT=$2; 
	    case ${ORIGIN_STORAGE_ELEMENT} in
		caf) ;;
		caf.cern.ch) ;;
		*)
		    echo "[ERROR] Origin storage_element ${ORIGIN_STORAGE_ELEMENT} != caf not implemented yet" >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    shift;;
	--remote_dir) ORIGIN_REMOTE_DIR_BASE=$2; shift;;

	--ntuple_remote_dir) USER_REMOTE_DIR_BASE=$2; echo ${USER_REMOTE_DIR_BASE}; shift;;

 	--type) TYPE=$2; shift;
	    case $TYPE in 
		alcareco | ALCARECO)
		    if [ "${isMC}" == "1" ]; then 
			TYPE=ALCARECOSIM; 
		    else
			TYPE=ALCARECO
		    fi
		    ;;
		ALCARECOSIM)
		    ;;
		alcarereco | ALCARERECO)
		    TYPE=ALCARERECO
		    if [ "${isMC}" == "1" ]; then
			echo "[ERROR] Incompatible options: TYPE=${TYPE} and --isMC" >> /dev/stderr
			exit 1
		    fi
		    ;;
		*)
		    echo "[OPTION ERROR] Type $TYPE not recognize" >> /dev/stderr
		    usage >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    ;;
	--isMC) isMC=1; TYPE=ALCARECOSIM;;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;

	-f|--filelist) FILELIST="$FILELIST $2"; echo ${FILELIST}; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
 	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE"; TAG=`basename $TAGFILE .py`; shift;;
	--ntuple_store) STORAGE_ELEMENT=$2; shift;;
	--ui_working_dir) UI_WORKING_DIR=$2; shift;;
	--sandbox_remote_dir) shift;; # ignore the option
	--scheduler) SCHEDULER=$2; shift;;
	#--puWeight) PUWEIGHTFILE=$2; shift;;
	--extraName) EXTRANAME=$2;shift;;
	--doExtraCalibTree) let DOTREE=${DOTREE}+2;;
	--doEleIDTree) let DOTREE=${DOTREE}+4;;
	--noStandardTree) let DOTREE=${DOTREE}-1;;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--check)      echo "[OPTION] checking jobs"; CHECK=y; EXTRAOPTION="--check"; unset CREATE; unset SUBMIT;;

 	--file_per_job) FILE_PER_JOB=$2; shift ;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


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

if [ -z "$TYPE" ];then 
    echo "[ERROR] TYPE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$JSONFILE" -a "$TYPE" != "ALCARECOSIM" ];then 
    echo "[ERROR] JSONFILE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$JSONNAME" -a "$TYPE" != "ALCARECOSIM" ];then 
    echo "[ERROR] JSONNAME not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


#Setting the ENERGY variable
setEnergy $DATASETPATH


if [ "${TYPE}" != "ALCARERECO" -a "${TYPE}" != "ALCARAW" ];then
    ORIGIN_REMOTE_DIR_BASE=`echo ${ORIGIN_REMOTE_DIR_BASE} | sed 's|alcaraw|alcareco|g'`
elif [ "${TYPE}" != "alcarereco" ];then
    echo ${ORIGIN_REMOTE_DIR_BASE}
#    ORIGIN_REMOTE_DIR_BASE=`echo ${ORIGIN_REMOTE_DIR_BASE} | sed 's|sandbox|alcareco|g'`
fi

if [ -z "${CHECK}" ];then
if [ "${TYPE}" == "ALCARERECO" ];then
    if [ "`cat ntuple_datasets.dat | grep ${DATASETNAME}  | grep ${JSONNAME} | grep $TAG | grep -c $RUNRANGE`" != "0" ];then
	echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
	exit 0
    fi
#else
#    if [ "`cat ntuple_datasets.dat | grep -v ALCARERECO | grep ${DATASETNAME} | grep ${JSONNAME} | grep -c $RUNRANGE`" != "0" ];then
#	echo "[WARNING] Ntuple for $JSONNAME  already done for ${RUNRANGE} ${DATASETNAME}"
#	exit 0
#    fi
fi
fi

# make the filelist before parsing the options and arguments
options="-d ${DATASETPATH} -n ${DATASETNAME} -r ${RUNRANGE} --remote_dir ${ORIGIN_REMOTE_DIR_BASE}"
if [ -n "${TAGFILE}" ];then options="$options -t ${TAGFILE}"; fi

case ${ORIGIN_REMOTE_DIR_BASE} in
    database)
	FILELIST=""
	;;
    *)
	echo ${FILELIST}
	if [ -z "${FILELIST}" ];then
	    FILELIST=filelist/${TAG}/${DATASETNAME}-${RUNRANGE}.list
	fi

	if [ ! -e "${FILELIST}" ];then
	    filelistDatasets.sh $options || exit 1
	fi
	;;
esac


if [ -z "$USER_REMOTE_DIR_BASE" ];then 
    echo "[ERROR] USER_REMOTE_DIR_BASE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi




#==============================

if [ -n "$FILELIST" ]
    then
    
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
fi


###############################



setStoragePath $STORAGE_ELEMENT $SCHEDULER in


#------------------------------

UI_WORKING_DIR=$UI_WORKING_DIR/${TYPE}/${TAG}/${DATASETNAME}/${RUNRANGE}/${JSONNAME}/${EXTRANAME}

USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${TYPE}
if [ -n "${TAG}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${TAG}; fi
USER_REMOTE_DIR=$USER_REMOTE_DIR/${DATASETNAME}/${RUNRANGE}
if [ -n "${JSONNAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${JSONNAME}; fi
if [ -n "${EXTRANAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${EXTRANAME}; fi
USER_REMOTE_DIR=$USER_REMOTE_DIR/unmerged



#${ENERGY}/
#${DATASETNAME}/tmp-${DATASETNAME}-${RUNRANGE}

OUTFILES=ntuple.root
if [ ! -d "tmp" ];then mkdir tmp/; fi
cat > tmp/crab.cfg <<EOF
[CRAB]
scheduler=$SCHEDULER
jobtype=cmssw
use_server=$USESERVER

[CMSSW]
EOF
case ${ORIGIN_REMOTE_DIR_BASE} in
        database)
        cat >> tmp/crab.cfg <<EOF
total_number_of_lumis = -1
lumis_per_job=${LUMIS_PER_JOBS}
datasetpath=${DATASETPATH}
EOF
        ;;
        *)
        cat >> tmp/crab.cfg <<EOF
total_number_of_events=${NJOBS}
number_of_jobs=${NJOBS}
datasetpath=None
EOF
	;;
esac

cat >> tmp/crab.cfg <<EOF
runselection=${RUNRANGE}
split_by_run=0
check_user_remote_dir=1
pset=python/alcaSkimming.py
pycfg_params=type=${TYPE} doTree=${DOTREE} doTreeOnly=1 jsonFile=${JSONFILE}
get_edm_output=1
output_file=${OUTFILES}


use_parent=0
#lumi_mask=


[LSF]
queue = 1nh
[CAF]
queue = cmscaf1nh


[USER]

ui_working_dir=${UI_WORKING_DIR}
return_data = 0
copy_data = 1

storage_element=$STORAGE_ELEMENT
user_remote_dir=$USER_REMOTE_DIR
storage_path=$STORAGE_PATH

thresholdLevel=50
eMail = shervin@cern.ch

[GRID]

rb = HC
rb = CERN
proxy_server = myproxy.cern.ch

EOF


if [ -n "${CREATE}" ];then
    crab -cfg tmp/crab.cfg -create || exit 1
    if [ -n "$FILELIST" ];then
	makeArguments.sh -f $FILELIST -u $UI_WORKING_DIR -n $FILE_PER_JOB || exit 1
    fi
fi

if [ -n "$SUBMIT" ];then
    crab -c ${UI_WORKING_DIR} -submit
    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TYPE}\t${TAG}\t${JSONNAME}"
    echo -e $STRING >> ntuple_datasets.dat

else
    echo "crab -c ${UI_WORKING_DIR} -submit"
fi

if [ -n "${CHECK}" ];then
    resubmitCrab.sh -u ${UI_WORKING_DIR}
    if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
	#echo $dir >> tmp/$TAG.log 
	echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
    else
	mergeOutput.sh -u ${UI_WORKING_DIR}
    fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
fi

