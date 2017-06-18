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
USER_REMOTE_DIR_BASE=group/dpg_ecal/alca_ecalcalib/ecalelf/alcarereco
NTUPLE_REMOTE_DIR_BASE=group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples
LUMIS_PER_JOB=50
CREATE=y
SUBMIT=y
DOTREE=1
TYPE=ALCARERECO
OUTFILES="ntuple.root"
JOBNAME="-SAMPLE-RUNRANGE-JSON"
CRABVERSION=2
crab3File=tmp/alcarereco_cfg.py
PUBLISH=False
#DBS_URL=phys03
DBS_URL=global
BUNCHSPACING=0

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
	echo "    --weightsReco: using weights for local reco"
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
    echo "    --crabVersion, crab version to use (=$CRABVERSION)"
	echo "    -v, --verbose"
#    echo "    --file_per_job arg: number of files to process in 1 job (=1)"
}


#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hHd:n:s:r:t:u:v -l help,expertHelp,file_per_job:,nJobs:,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,rereco_remote_dir:,ui_working_dir:,scheduler:,createOnly,submitOnly,check,ntupleCheck,crabVersion:,json:,json_name:,doExtraCalibTree,doEleIDTree,noStandardTree,alcarerecoOnly,ntupleOnly,tutorial,weightsReco -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

checkVerboseOption

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
 	-t | --tag) 
			TAGFILE=$2; 
			if [ -n "${VERBOSE}" ];then
				echo "[OPTION `basename $0`] TAGFILE:$TAGFILE";
			fi
			shift
			;;
 	--crabVersion) CRABVERSION=$2;  shift;;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
	--doExtraCalibTree) 
			if [ -n "${VERBOSE}" ];then 
				echo "[OPTION `basename $0`] doing extraCalibTree"; 
			fi
			DOEXTRACALIBTREE="--doExtraCalibTree"; let DOTREE=${DOTREE}+2; OUTFILES="${OUTFILES},extraCalibTree.root"
			;;
	--doEleIDTree)
			if [ -n "${VERBOSE}" ];then
				echo "[OPTION `basename $0`] doing eleIDTree"; 
			fi
			let DOTREE=${DOTREE}+4; OUTFILES="${OUTFILES},eleIDTree.root"
			;;
	--noStandardTree) EXTRAOPTION="$EXTRAOPTION --noStandardTree"; let DOTREE=${DOTREE}-1; OUTFILES=`echo ${OUTFILES} | sed 's|ntuple.root,||'`;;
	--createOnly) echo "[OPTION `basename $0`] createOnly"; unset SUBMIT; EXTRAOPTION="$EXTRAOPTION --createOnly";;
	--submitOnly) echo "[OPTION `basename $0`] submitOnly"; unset CREATE; EXTRAOPTION="$EXTRAOPTION --submitOnly";;
	--check)   
			if [ -n "${VERBOSE}" ];then
				echo "[OPTION `basename $0`] checking jobs"; 
			fi
			unset CREATE; unset SUBMIT; CHECK=y; EXTRAOPTION="--check"
			;;
	--ntupleCheck)      echo "[OPTION `basename $0`] checking ntuple jobs"; unset CREATE; unset SUBMIT; NTUPLECHECK=y; EXTRAOPTION="--check";;
	--alcarerecoOnly) 
			if [ -n "${VERBOSE}" ];then 
				echo "[OPTION `basename $0`] alcarerecoOnly"; 
			fi
			OUTFILES="EcalRecalElectron.root"; DOTREE=0
			;;
	--weightsReco)    echo "[OPTION `basename $0`] using weights for local reco"; BUNCHSPACING=-1;;
	--ntupleOnly)     echo "[OPTION `basename $0`] ntupleOnly"; NTUPLEONLY=y;;
	--rereco_remote_dir) USER_REMOTE_DIR_BASE=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	-f|--filelist) FILELIST="$FILELIST $2"; echo ${FILELIST}; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
	-u | --ui_working_dir) UI_WORKING_DIR=$2; shift;;
    --tutorial)   echo "[OPTION `basename $0`] Activating the tutorial mode"; TUTORIAL=y;;
	    
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




##############################
#------------------------------
TAG=`basename $TAGFILE .py`


#Setting the ENERGY variable
setEnergy $DATASETPATH

setStoragePath $STORAGE_ELEMENT $SCHEDULER 


case $DATASETNAME in
#	*-50nsReco) BUNCHSPACING=50;;
#	*-25nsReco) BUNCHSPACING=25;;
	*-weightsReco) 
		BUNCHSPACING=-1
		DATASETNAME=${DATASETNAME}-weightsReco
		;;
esac

JOBNAME="${DATASETNAME}-${TAG}" 

ALCARAW_REMOTE_DIR=$ALCARAW_REMOTE_DIR_BASE/${ENERGY}/${DATASETNAME}/${RUNRANGE}
setUserRemoteDirAlcarereco $USER_REMOTE_DIR_BASE
#USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${TAG}/${DATASETNAME}/${RUNRANGE}

NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR_BASE/${ENERGY}/${TYPE}
if [ -n "${TAG}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${TAG}; fi
NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${DATASETNAME}/${RUNRANGE}
if [ -n "${JSONNAME}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${JSONNAME}; fi
if [ -n "${EXTRANAME}" ];then NTUPLE_REMOTE_DIR=$NTUPLE_REMOTE_DIR/${EXTRANAME}; fi

if [ -n "${VERBOSE}" ];then
	echo "[INFO] RUNRAGE=$RUNRANGE"
	echo "[INFO] DATASETPATH=$DATASETPATH"
	echo "[INFO] DATASETNAME=$DATASETNAME"
	echo "[INFO] ALCARAW_REMOTE_DIR_BASE=$ALCARAW_REMOTE_DIR_BASE"
	echo "[INFO] USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE:=alcarereco}"
	echo "[INFO] NTUPLE_REMOTE_DIR_BASE=${NTUPLE_REMOTE_DIR_BASE}"
	echo "[VERBOSE INFO] BUNCHSPACING=${BUNCHSPACING}"
#echo "[INFO] TAGFILE=$TAGFILE"
fi
###############################

#check if rereco already done
#if [ -n "${NTUPLE}" ];then DOTREE=1; fi

#if [ -n "${VERBOSE}" ];then
UI_WORKING_DIR=${UI_WORKING_DIR:=prod_alcarereco/${TAG}/${DATASETNAME}/${RUNRANGE}/${TUTORIAL}}
#printf '[INFO %19s] %s\n' `basename $0` $UI_WORKING_DIR
echo "[INFO] $UI_WORKING_DIR"
#fi

if [ -e "${UI_WORKING_DIR}/res/finished" ];then
	echo "finished"
	exit 0
fi

#==============================
if [ -n "${CREATE}" ];then

	#check if the rereco has already been done
#	if [ "`cat alcarereco_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME}| grep -c $RUNRANGE`" != "0" ];then
	if checkIfRerecoed ; then
		echo "[WARNING] Rereco $TAG already done for ${RUNRAGE} ${DATASETNAME} ${TAG}"
		if [ "${DOTREE}" != "0" ]; then
			echo "          Doing only ntuples"
			if [ "`cat ntuple_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep ${JSONNAME} |grep -c $RUNRANGE `" != "0" ];then
				echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
				exit 0
			fi
			echo "[INFO] using prodNtuples "
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

	checkRelease ${DATASETPATH}
	
	case ${ALCARAW_REMOTE_DIR_BASE} in
		database)
			FILELIST=""
			;;
		*)
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
			;;
	esac
	

#==============================
	crabFile=tmp/`echo $UI_WORKING_DIR | sed 's|/|_|g'`.cfg
	cat > $crabFile <<EOF
UI_WORKING_DIR=$UI_WORKING_DIR
pset=python/alcaSkimming.py
psetparams="type=ALCARERECO tagFile=${TAGFILE} doTree=${DOTREE} doTreeOnly=0 jsonFile=${JSONFILE}  isCrab=1 bunchSpacing=${BUNCHSPACING}"

outFiles=${OUTFILES}
use_parent=${USEPARENT}

EOF
	case ${ALCARAW_REMOTE_DIR_BASE} in
        database)
			cat >> ${crabFile} <<EOF
dataset=${DATASETPATH}
runrange=${RUNRANGE}
EOF
        ;;
        *)
			cat >> ${crabFile} <<EOF
dataset=None
runrange=
EOF
			;;
	esac
	
	
	cat >> ${crabFile} <<EOF
user_remote_dir="$USER_REMOTE_DIR"
storage_path="$STORAGE_PATH"

EOF

create ${crabFile} || exit $?
#end of create
fi

if [ -n "${SUBMIT}" ]; then
	submit ${UI_WORKING_DIR} ${TAG}
    if [ "`cat alcarereco_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
		echo "[WARNING] Rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
		exit 0
	fi
	

elif [ -n "${CREATE}" ];then
    echo "##############################"
    echo "To start the crab jobs:"
    echo "crab -c ${UI_WORKING_DIR} -submit all"
fi


OUTFILES=`echo ${OUTFILES} | sed 's|,| |g'`

if [ -n "${CHECK}" ];then
	if [ ! -e "${UI_WORKING_DIR}/res" ];then
		echo "[ERROR] No $UI_WORKING_DIR directory found" >> /dev/stderr
		exit 1
	fi
	if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
		check $UI_WORKING_DIR
		if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
             	#echo $dir >> tmp/$TAG.log 
			echo 
#			echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
		else
			if [ -z "${NTUPLEONLY}" ];then
				echo "FINISHED"
				if checkIfRerecoed; then 
					echo 
				else
					echo -e $STRING
					echo -e $STRING >> alcarereco_datasets.dat
				fi
				
				if [ "${DOTREE}" != "0" ];then
					for file in $OUTFILES
					do
						file=`basename $file .root`
						echo "[INF0] Merging $file files"
						mergeOutput.sh -u ${UI_WORKING_DIR} -g $file --merged_remote_dir=${NTUPLE_REMOTE_DIR}
					done
				fi
			else
				if [ "`cat ntuple_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep ${JSONNAME} |grep -c $RUNRANGE `" != "0" ];then
					echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
					exit 0
				fi
				echo "[INFO] using prodNtuples "
				rm filelist/ -Rf
				./scripts/prodNtuples.sh -r ${RUNRANGE} -d ${DATASETPATH} -n ${DATASETNAME} --store ${STORAGE_ELEMENT} --remote_dir ${USER_REMOTE_DIR_BASE} --type=ALCARERECO --json=${JSONFILE} --json_name=${JSONNAME} -t ${TAGFILE} ${DOEXTRACALIBTREE} ${EXTRAOPTION}
			fi
		fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
	fi
fi


