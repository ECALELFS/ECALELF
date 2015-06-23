#!/bin/bash
source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh
############################### OPTIONS
#------------------------------ default
SKIM=none
ALCATYPE="ALCA:EcalCalZElectron"
USEPARENT=0
SCHEDULER=caf
USESERVER=1
TYPE=ALCARECO
ISMC="no"
LUMIS_PER_JOBS=200 # 4000 for ZSkim events is good, WSkim events /=4, SingleElectron /=10
EVENTS_PER_JOB=150000 #150k for ZSkim DYtoEE powheg
BLACKLIST=T2_EE_Estonia
CREATE=yes
SUBMIT=yes
OUTPUTFILE=alcareco
crab2File=tmp/alcareco.cfg
crab3File=tmp/alcareco_cfg.py
CONDITIONS=auto:run2_data
crabFile=${crab3File}
DOTREE=0
NJOBS=100
OUTFILES="ntuple.root"
JOBNAME="-SAMPLE-RUNRANGE-JSON"
PUBLISH="False"
#CRABVERSION=3
CRABVERSION=3
FROMRAW=no
CMSSWCONFIG="reco_ALCA.py"
SQRTS=13TeV
INPUTDBS=global
usage(){
    echo "`basename $0` options"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir"
    echo "    --dbs_url url: for not global dbs (user production)"
    echo "---------- optional"
    echo "    --isMC: specify is the dataset is MC"
    echo "    -s or --skim: ZSkim, WSkim, EleSkim"
    echo "    --type: ALCARECO (or EcalCal), ALCARAW (OrEcalUncal). Determines if you produce ALCARECO (EcalCal*.root) or ALCARAW (EcalUncal*.root) files."
    echo "    --scheduler caf,lsf,remoteGlidein (=${SCHEDULER})  (only for crab2)"
    echo "    --createOnly. Only create the CMSSW and CRAB configuration files, do not submit"
    echo "    --submitOnly. Only submit the already created configuration files."
    echo "    --check. Check on the status of already submited CRAB jobs"
    echo "    --jobname name. Name of your job for crab."
    echo "    --json_name jsonName. Additional name in the folder structure to keep track of the used json"
    echo "    --json jsonFile.root:. Specifiy a run/lumi range. Better to not use a json file for the alcareco production"
    echo "    --doTree (NOT CURRENTLY SUPPORTED)" # arg (=${DOTREE}): 0=no tree, 1=standard tree only, 2=extratree-only, 3=standard+extra trees"
    echo "    --njobs nJobs : number of jobs, an integer (only crab2)"
  	echo "    --publish : Publish output dataset on DAS"
  	echo "    --fromRAW : Use this option is you are running on RAW, so that RECO,RAW2DIGI steps are run first. (Expert Only)"
    echo "----------"
    echo "    --tutorial: tutorial mode, produces only one sample in you user area"
    echo "    --develRelease: CRAB do not check if the CMSSW version is in production (only if you are sure what you are doing)"
	echo "----------"
}




#------------------------------ parsing
# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hd:n:s:r: -l help,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,dbs_url:,scheduler:,isMC,type:,submit,white_list:,black_list:,createOnly,submitOnly,check,json:,jobname:,doTree,doExtraCalibTree,doEleIDTree,doPdfSystTree,noStandardTree,njobs:,publish,fromRAW,tutorial,develRelease -- "$@")
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
	--dbs_url)    DBS_URL=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	--isMC) echo "[OPTION] data is MC" ; ISMC="yes" ;;
	--type) TYPE=$2 ; shift;;
	--white_list) WHITELIST=$2; shift;;
	--black_list) BLACKLIST=$2; shift;;
	--createOnly) unset SUBMIT; unset CHECK;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--check)      echo "[OPTION] checking jobs"; unset CREATE; unset SUBMIT; CHECK=y; EXTRAOPTION="--check";;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;
        --tutorial)   echo "[OPTION] Activating the tutorial mode"; TUTORIAL=y;;
	--develRelease) echo "[OPTION] Request also CMSSW release not in production!"; DEVEL_RELEASE=y;;
	--doTree) let DOTREE=${DOTREE}+1; OUTFILES="${OUTFILES},ntuple.root";;
	--doExtraCalibTree) let DOTREE=${DOTREE}+2; OUTFILES="${OUTFILES},extraCalibTree.root";;
	--doEleIDTree) let DOTREE=${DOTREE}+4;;
	--doPdfSystTree) let DOTREE=${DOTREE}+8;;
	--noStandardTree) let DOTREE=${DOTREE}-1; OUTFILES=`echo ${OUTFILES} | sed 's|ntuple.root,||'`;;

    --njobs) NJOBS=$2; shift; echo "[OPTION] Request njobs = ${NJOBS}";;
		--jobname) JOB=$2; shift;;
		--publish) echo "[OPTION] output will be published on DAS"; PUBLISH="True" ;;
		--fromRAW) echo "[OPTION] fromRAW: first perform RAW2DIGI,RECO step on input files." ; FROMRAW="yes";;
		(--) shift; break;;
		(-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
		(*) break;;
	esac
	shift
done
JOBNAME="${DATASETNAME}" 
# _${RUNRANGE}_${JOB}
#------------------------------ checking
echo "[INFO] using CRAB version ${CRABVERSION}"
if [ -z "$DATASETPATH" ];then 
    echo "[ERROR] DATASETPATH not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$DATASETNAME" ];then 
    echo "[ERROR] DATASETNAME not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
else
    echo "[INFO] DATASETNAME=${DATASETNAME}"
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
else
    echo "[INFO] USER_REMOTE_DIR_BASE=${USER_REMOTE_DIR_BASE}"
fi

if [ -n "${TUTORIAL}" ];then
    case $DATASETPATH in
	/RelVal*)
	    ;;
	*)
	    echo "[ERROR] With the tutorial mode, the only permitted datasetpath is:"
	    echo "        /RelVal*"
	    echo "        Be sure to have it in alcareco_datasets.dat and to have selected it using the parseDatasetFile.sh"
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
###############################
#------------------------------
if [ -z "${SKIM}" ];then
    case $DATASETPATH in
	*DoubleElectron*)
	    SKIM=ZSkim
	    ;;
	*SingleElectron*USER)
	    SKIM=fromWSkim
	    ;;
	*SingleElectron*)
	    SKIM=WSkim
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
		*USER)
	let INPUTDBS=phys03
	;;
esac

if [ "$TYPE" == "EcalCal" ] || [ "$TYPE" == "ALCARECO" ]; then
case $SKIM in
	ZSkim)
		ALCATYPE="ALCA:EcalCalZElectron"
		;;
	none) EVENTS_PER_JOB=20000;;
	WSkim)
		ALCATYPE="ALCA:EcalCalWElectron"
		EVENTS_PER_JOB=20000
		;;
esac
fi

if [ "$TYPE" == "EcalUncal" ] || [ "$TYPE" == "ALCARAW" ]; then
case $SKIM in
	ZSkim)
		ALCATYPE="ALCA:EcalUncalZElectron"
		;;
	none) EVENTS_PER_JOB=20000;;
	WSkim)
		ALCATYPE="ALCA:EcalUncalWElectron"
		EVENTS_PER_JOB=20000
		;;
esac
fi

if [ "$TYPE" == "EcalRecal" ] || [ "$TYPE" == "ALCARERECO" ]; then
		ALCATYPE="ALCA:EcalRecalElectron"
		CUSTOMISE="--process=RERECO --customise Calibration/EcalAlCaRecoProducers/customRereco.EcalRecal "
fi

if [ "$FROMRAW" == "yes" ]; then
RECOPATH="RAW2DIGI,RECO,"
CMSSWCONFIG="reco_RAW2DIGI_RECO_ALCA.py"
fi



#Setting the ENERGY variable
setEnergy $DATASETPATH

SQRTS=$ENERGY
if [ "$ISMC" == "no" ]; then
DATA="--data"
echo "$ISMC, $SQRTS"
	if [ "$SQRTS" == "8TeV" ]; then
  CONDITIONS=auto:run1_data
	echo "[INFO] $SQRTS, setting conditions to $CONDITIONS" 
	fi
fi
if [ "$ISMC" == "yes" ]; then
CONDITIONS=auto:run2_mc_50ns
	if [ "$SQRTS" == "8TeV" ]; then
  CONDITIONS=auto:run1_mc_50ns
  CONDITIONS=auto:run1_data
	echo "[INFO] $SQRTS, setting conditions to $CONDITIONS" 
	fi
fi

setStoragePath $STORAGE_ELEMENT $SCHEDULER 

if [ -z "${CHECK}" ];then checkRelease ${DATASETPATH}; fi

USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${DATASETNAME}/${RUNRANGE:-allRange}
UI_WORKING_DIR=prod_alcareco/${DATASETNAME}/${RUNRANGE}

if [ "$RUNRANGE" == "allRange" -o "`echo $RUNRANGE |grep -c -P '[0-9]+-[0-9]+'`" == "0" ];then
    unset RUNRANGE
fi

# make argument.xml file if do MC
#if [ "$TYPE" == "ALCARECOSIM" ] && [ -n "${CREATE}" ];then
#  makeArgumentsWithDataPath.sh -d ${DATASETPATH} -n ${NJOBS} -o _tmp_argument.xml
#  #redefine the NJOBS
#  NJOBS=`grep "</Job>" _tmp_argument.xml | wc -l`
#fi

if [ -z "${CHECK}" ] || [ -n "${CREATE}" ];then
echo "[INFO] Preparing job: ${JOBNAME}"
echo "[INFO] Storage Element $STORAGE_ELEMENT"
echo "[INFO Run Range ${RUNRANGE}"

OUTFILES=`echo $OUTFILES | sed 's|^,||'`

echo "[INFO] Generating CMSSW configuration, using:"
echo "[INFO]  cmsDriver.py reco -s ${RECOPATH}${ALCATYPE} -n 10 ${DATA} --conditions=$CONDITIONS --nThreads=4 --customise_commands=\"process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))\" $CUSTOMISE--no_exec " 

cmsDriver.py reco -s ${RECOPATH}${ALCATYPE} -n 10 ${DATA} --conditions=$CONDITIONS --nThreads=4 --customise_commands="process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))" $CUSTOMISE --no_exec

echo "[INFO] Generating CRAB3 configuration"
TYPENAME=$TYPE
if [ ${ISMC} = "yes" ];then
TYPENAME="${TYPENAME}SIM"
fi
#==============================
cat > ${crab2File} <<EOF
[CRAB]
#use_server = $USESERVER
jobtype = cmssw
scheduler = $SCHEDULER
[LSF]
queue = 1nd
#resource = type==SLC5_64
[CAF]
queue = cmscaf1nd
resource = type==SLC5_64
EOF
cat > ${crab3File} <<EOF
from CRABClient.UserUtilities import config
config = config()
config.General.requestName = '${JOBNAME}'
config.General.workArea = 'crab_projects'
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '${CMSSWCONFIG}'
#config.JobType.pyCfgParams = ['output=${OUTPUTFILE}.root', 'skim=${SKIM}', 'type=$TYPE','doTree=${DOTREE}', 'jsonFile=${JSONFILE}', 'secondaryOutput=ntuple.root', 'isCrab=1']
config.JobType.allowUndistributedCMSSW = True
config.Data.inputDataset = '${DATASETPATH}'
config.Data.inputDBS = '${INPUTDBS}'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 3
config.Data.lumiMask = '${JSONFILE}'
config.Data.runRange = '${RUNRANGE}'
config.Data.outLFNDirBase = '/store/${USER_REMOTE_DIR_BASE}/${USER}/${TYPENAME}/${SQRTS}'
config.Data.publication = ${PUBLISH}
config.Data.publishDataName = '${JOBNAME}'
#config.Site.storageSite = '$STORAGE_ELEMENT'
config.Site.storageSite = "T2_CH_CERN"
#config.Site.storageSite = "T2_UK_London_IC"
EOF

echo "--> CMSSW config created: ${CMSSWCONFIG}"
echo "--> CRAB3 config created: ${crab3File}"

cat >> ${crab2File} <<EOF
[CMSSW]
datasetpath=${DATASETPATH}
use_dbs3  = 1
EOF
if [ -n "${DBS_URL}" ];then
    echo "dbs_url=${DBS_URL}" >> ${crab2File}
fi

cat >> ${crab2File} <<EOF
pset=python/alcaSkimming.py
pycfg_params=output=${OUTPUTFILE}.root skim=${SKIM} type=$TYPE doTree=${DOTREE} jsonFile=${JSONFILE} secondaryOutput=ntuple.root isCrab=1 

runselection=${RUNRANGE}
split_by_run=0
EOF

if [ "$DOTREE" -gt "0" ]; then
   cat >> ${crab2File} <<EOF
output_file=${OUTFILES}
EOF
fi 

if [ "$TYPE" == "ALCARECOSIM" ];then
    cat >> ${crab2File} <<EOF
total_number_of_events = -1
events_per_job=${EVENTS_PER_JOB}
EOF
#total_number_of_events=${NJOBS}
#number_of_jobs=${NJOBS}
#EOF
else
    cat >> ${crab2File} <<EOF
total_number_of_lumis = -1
lumis_per_job=${LUMIS_PER_JOBS}
EOF

if [ -n "${CREATE}" ] && [ -z "${SUBMIT}" ];then
echo "[INFO] to submit please use option --submitOnly"
exit 1
fi

if [ -n "${DEVEL_RELEASE}" ]; then
cat >> ${crab2File} <<EOF
allow_NonProductionCMSSW = 1
EOF


###
cat >> ${crab2File} <<EOF
get_edm_output=1
check_user_remote_dir=1
use_parent=${USEPARENT}

[USER]
ui_working_dir=$UI_WORKING_DIR
return_data = 0
copy_data = 1
local_stage_out = 1
storage_element=$STORAGE_ELEMENT
user_remote_dir=$USER_REMOTE_DIR
storage_path=$STORAGE_PATH
EOF

#if [ "$TYPE" == "ALCARECOSIM" ];then
#   cat >> ${crabFile} <<EOF
#script_exe=initdata.sh
#additional_input_files=${cert}
#EOF
#if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
#echo $dir >> tmp/$TAG.log
#echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
# elif [ "$TYPE" == "ALCARECOSIM" ];then
# mergeOutput.sh -u ${UI_WORKING_DIR} -g PUDumper --noRemove --merged_remote_dir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALELF/puFiles/
#fi

cat >> ${crab2File} <<EOF
thresholdLevel=80
eMail = shervin@cern.ch

[GRID]
rb = HC
rb = CERN
proxy_server = myproxy.cern.ch
#se_white_list=$WHITELIST
se_black_list=$BLACKLIST

EOF

fi
fi


if [ -n "${CREATE}" ];then
	case ${CRABVERSION} in
		2)
			crab -cfg ${crab2File} -create || exit 1
			;;
		3)
			;;
	esac
fi

 #if [ "$TYPE" == "ALCARECOSIM" ];then
 #  mv _tmp_argument.xml ${UI_WORKING_DIR}/share/arguments.xml 
 #fi 
 
./scripts/splittedOutputFilesCrabPatch.sh -u ${UI_WORKING_DIR}
#crabMonitorID.sh -r ${RUNRANGE} -n $DATASETNAME -u ${UI_WORKING_DIR} --type ALCARECO

 #clean up extral lines
 #awk ' /file_list=\"\"/ &&c++>0 {next} 1 ' ${UI_WORKING_DIR}/job/CMSSW.sh > _tmp_CMSSW.sh
 #chmod +x _tmp_CMSSW.sh
 #mv _tmp_CMSSW.sh ${UI_WORKING_DIR}/job/CMSSW.sh
# echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
# else
# mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}
fi

if [ -n "$SUBMIT" ]; then
	case ${CRABVERSION} in
		2)     crab -c ${UI_WORKING_DIR} -submit all;;
		3)
			echo "[INFO] submitting to CRAB3"
			crab submit -c $crabFile 
			;;
	esac
elif [ -z "${CHECK}" ];then
    echo "##############################"
    echo "To start the crab jobs:"
    echo "crab -c ${UI_WORKING_DIR} -submit all"
fi


if [ -n "${CHECK}" ];then
	case ${CRABVERSION} in
		2)
			resubmitCrab.sh -u ${UI_WORKING_DIR}
			if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
	#echo $dir >> tmp/$TAG.log 
				echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
#    elif [ "$TYPE" == "ALCARECOSIM" ];then
#	mergeOutput.sh -u ${UI_WORKING_DIR} -g PUDumper --noRemove --merged_remote_dir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALELF/puFiles/
    #fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
#    else
#	mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}
			fi 
			;;
		3)
			crab status crab_projects/crab_${JOBNAME}
			echo "crab status crab_projects/crab_${JOBNAME}"
			;;
	esac

fi

################
#echo "###############"
#echo "# crab config file created in tmp/alcareco.cfg"
#echo "# now create the crab job and submit it"
#echo "crab -cfg tmp/alcareco.cfg -create -submit"
