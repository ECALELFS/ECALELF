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
LUMIS_PER_JOB=20
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
if ! options=$(getopt -u -o hHd:n:s:r:t:u: -l help,expertHelp,file_per_job:,nJobs:,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,rereco_remote_dir:,ui_working_dir:,scheduler:,createOnly,submitOnly,check,ntupleCheck,crabVersion:,json:,json_name:,doExtraCalibTree,doEleIDTree,noStandardTree,alcarerecoOnly,ntupleOnly,tutorial,weightsReco -- "$@")
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
	-v|--verbose)    VERBOSE=y;;
	-r|--runrange) RUNRANGE=$2; shift;;
	-d|--datasetpath) DATASETPATH=$2; shift ;;
	-n|--datasetname) DATASETNAME=$2; shift ;;
	--store) STORAGE_ELEMENT=$2; shift;;
	--remote_dir) ALCARAW_REMOTE_DIR_BASE=$2; shift;;
 	-t | --tag) TAGFILE=$2; echo "[OPTION] TAGFILE:$TAGFILE";shift;;

 	--crabVersion) CRABVERSION=$2;  shift;;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
	--doExtraCalibTree) DOEXTRACALIBTREE="--doExtraCalibTree"; let DOTREE=${DOTREE}+2; OUTFILES="${OUTFILES},extraCalibTree.root";;
	--doEleIDTree) let DOTREE=${DOTREE}+4; OUTFILES="${OUTFILES},eleIDTree.root";;
	--noStandardTree) EXTRAOPTION="$EXTRAOPTION --noStandardTree"; let DOTREE=${DOTREE}-1; OUTFILES=`echo ${OUTFILES} | sed 's|ntuple.root,||'`;;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT; EXTRAOPTION="$EXTRAOPTION --createOnly";;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE; EXTRAOPTION="$EXTRAOPTION --submitOnly";;
	--check)      echo "[OPTION] checking jobs"; unset CREATE; unset SUBMIT; CHECK=y; EXTRAOPTION="--check";;
	--ntupleCheck)      echo "[OPTION] checking ntuple jobs"; unset CREATE; unset SUBMIT; NTUPLECHECK=y; EXTRAOPTION="--check";;
	--alcarerecoOnly) echo "[OPTION] alcarerecoOnly"; OUTFILES=""; DOTREE=0;;
	--weightsReco)    echo "[OPTION] using weights for local reco"; BUNCHSPACING=-1;;
	--ntupleOnly)     echo "[OPTION] ntupleOnly"; NTUPLEONLY=y;;
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


if [ -n "${VERBOSE}" ];then
	echo "DEBUG ALCARERECO DOEXTRACALIBTREE $DOEXTRACALIBTREE"
	echo "DEBUG DOTREEE $DOTREE"
fi

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

if [ -n "${VERBOSE}" ];then
	echo "[INFO] RUNRAGE=$RUNRANGE"
	echo "[INFO] DATASETPATH=$DATASETPATH"
	echo "[INFO] DATASETNAME=$DATASETNAME"
	echo "[INFO] ALCARAW_REMOTE_DIR_BASE=$ALCARAW_REMOTE_DIR_BASE"
	echo "[INFO] USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE:=alcarereco}"
	echo "[INFO] NTUPLE_REMOTE_DIR_BASE=${NTUPLE_REMOTE_DIR_BASE}"
#echo "[INFO] TAGFILE=$TAGFILE"
fi
###############################

#------------------------------
TAG=`basename $TAGFILE .py`
JOBNAME="${DATASETNAME}-${TAG}" 

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

case $DATASETNAME in
#	*-50nsReco) BUNCHSPACING=50;;
#	*-25nsReco) BUNCHSPACING=25;;
	*-weightsReco) BUNCHSPACING=-1;;
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

#if [ -n "${VERBOSE}" ];then
echo "[INFO] UI_WORKING_DIR=${UI_WORKING_DIR:=prod_alcarereco/${TAG}/${DATASETNAME}/${RUNRANGE}/${TUTORIAL}}"
#fi

checkIfRerecoed(){
	STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TAG}"
	if [ "`grep ${RUNRANGE} alcarereco_datasets.dat | grep ${DATASETPATH} |grep ${DATASETNAME} | grep ${STORAGE_ELEMENT} | grep ${USER_REMOTE_DIR_BASE} | grep ${TAG}$ | wc -l`" == "0" ]; then 
		echo "NotRERECOED"
		return 1; 
	else 
		grep ${RUNRANGE} alcarereco_datasets.dat | grep ${DATASETNAME} | grep ${DATASETPATH} | grep ${STORAGE_ELEMENT} | grep ${USER_REMOTE_DIR_BASE} | grep ${TAG}$ 
		return 0; 
	fi
	#return is true if not-rerecoed
}

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
#resource = type==SLC6_64 r&& usage[mem=1024,swap=2500]
resource = type==SLC6_64 


[CMSSW]
allow_NonProductionCMSSW = 1

pset=python/alcaSkimming.py
pycfg_params= type=ALCARERECO tagFile=${TAGFILE} doTree=${DOTREE} doTreeOnly=0 jsonFile=${JSONFILE}  isCrab=1 bunchSpacing=${BUNCHSPACING}

runselection=${RUNRANGE}
split_by_run=0

output_file=${OUTFILES}
get_edm_output=1
check_user_remote_dir=1
use_parent=${USEPARENT}

EOF
	case ${ALCARAW_REMOTE_DIR_BASE} in
        database)
			cat >> ${crabFile} <<EOF
total_number_of_lumis = -1
datasetpath=${DATASETPATH}
lumis_per_job=${LUMIS_PER_JOB}
#number_of_jobs=494
#dbs_url = ${DBS_URL}
EOF
        ;;
        *)
			cat >> ${crabFile} <<EOF
total_number_of_events=${NJOBS}
#total_number_of_events=494
number_of_jobs=${NJOBS}
#number_of_jobs=494
datasetpath=None
EOF
			;;
	esac
	
	
	cat >> ${crabFile} <<EOF
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
	cat > ${crab3File} <<EOF
from CRABClient.UserUtilities import config
config = config()
config.General.requestName = '${JOBNAME}'
config.General.workArea = 'crab_projects'
config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles= ['${OUTFILES}']
config.JobType.psetName = 'python/alcaSkimming.py'
config.JobType.pyCfgParams = ['tagFile=${TAGFILE}', 'skim=${SKIM}', 'type=$TYPE','doTree=${DOTREE}', 'jsonFile=${JSONFILE}', 'isCrab=1', 'bunchSpacing=${BUNCHSPACING}']
config.JobType.allowUndistributedCMSSW = True
config.Data.inputDataset = '${DATASETPATH}'
config.Data.inputDBS = '${DBS_URL}'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.lumiMask = '${JSONFILE}'
config.Data.runRange = '${RUNRANGE}'
config.Data.outLFNDirBase = '/store/${USER_REMOTE_DIR}'
config.Data.publication = ${PUBLISH}
config.Data.publishDataName = '${JOBNAME}'
#config.Site.storageSite = '$STORAGE_ELEMENT'
config.Site.storageSite = "T2_CH_CERN"
#config.Site.storageSite = "T2_UK_London_IC"
EOF

	case ${CRABVERSION} in
		2)
			crab -cfg ${crabFile} -create || exit 1
			if [ -n "${FILE_PER_JOB}" ];then
				makeArguments.sh -f filelist/$sample.list -u $UI_WORKING_DIR -n $FILE_PER_JOB || exit 1 
			fi
			./scripts/splittedOutputFilesCrabPatch.sh -u ${UI_WORKING_DIR}
#crabMonitorID.sh -r ${RUNRANGE} -n $DATASETNAME -u ${UI_WORKING_DIR} --type ALCARECO
			rm filelist/$sample* -Rf
			;;
		3)
			;;
	esac
#end of create
fi

if [ -n "${SUBMIT}" ]; then
    if [ "`cat alcarereco_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
		echo "[WARNING] Rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
		exit 0
	fi
	case ${CRABVERSION} in
		2)  
			crab -c ${UI_WORKING_DIR} -submit all;
#    if [ "`cat alcarereco_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep -c $RUNRANGE`" != "0" ];then
#	echo "[WARNING] Rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
#	if [ "${DOTREE}" == "1" ]; then
#	    echo "          Doing only ntuples"
#	    if [ "`cat ntuple_datasets.dat | grep  \"$TAG[ ]*\$\" | grep ${DATASETNAME} | grep ${JSONNAME} |grep -c $RUNRANGE `" != "0" ];then
#		echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"
#		exit 0
#	    fi
#				echo "[INFO] using prodNtuples "
#	    ./scripts/prodNtuples.sh -r ${RUNRANGE} -d ${DATASETPATH} -n ${DATASETNAME} --store ${STORAGE_ELEMENT} --remote_dir ${USER_REMOTE_DIR_BASE} --type=ALCARERECO --json=${JSONFILE} --json_name=${JSONNAME} -t ${TAGFILE} ${DOEXTRACALIBTREE} ${EXTRAOPTION}
#	    exit 0
#	fi
#	exit 0
#    fi
	 ;;
		3)
			echo "[INFO] submitting to CRAB3"
			crab submit -c $crab3File
			;;
	esac
	
#    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TAG}"
#    echo -e $STRING >> alcarereco_datasets.dat
#    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${NTUPLE_REMOTE_DIR_BASE}\t${TYPE}\t${TAG}\t${JSONNAME}"
#    echo -e $STRING >> ntuple_datasets.dat 
	

elif [ -n "${CREATE}" ];then
    echo "##############################"
    echo "To start the crab jobs:"
    echo "crab -c ${UI_WORKING_DIR} -submit all"
fi


OUTFILES=`echo ${OUTFILES} | sed 's|,| |'`

if [ -n "${CHECK}" ];then
	case ${CRABVERSION} in
		2)
			# first round of check
			if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
				echo "[INFO] UI_WORKING_DIR=${UI_WORKING_DIR} "
				resubmitCrab.sh -u ${UI_WORKING_DIR}
			fi
			if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
             	#echo $dir >> tmp/$TAG.log 
				echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
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
			;;
		3)
			echo "crab status crab_projects/crab_${JOBNAME}"
			crab status crab_projects/crab_${JOBNAME} |  tee temp.txt
			DONE=`cat temp.txt | grep finished | grep "100%" | wc -l`
			if [ $DONE == 0 ];then
			echo "jobs not finished!"
			fi
			if [ $DONE == 1 ];then
			echo "done!"
			fi

			;;
	esac
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
fi



