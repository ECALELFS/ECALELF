#!/bin/bash
source $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/scripts/prodFunctions.sh

############################### OPTIONS
#------------------------------ default
USESERVER=0
SCHEDULER=caf
STORAGE_ELEMENT=caf
isMC=0
UI_WORKING_DIR=prod_ntuples
#USER_REMOTE_DIR_BASE=group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples
USER_REMOTE_DIR_BASE=ntuples
#LUMIS_PER_JOBS=12000 #5000 is also too much -> cpu limit usage
LUMIS_PER_JOBS=100
DOEXTRACALIBTREE=0
CREATE=y
SUBMIT=y
DOTREE=1
PDFSYST=0
SKIM=""
OUTFILES="ntuple.root"
crabFile=tmp/ntuple.cfg
CRABVERSION=2
FROMCRAB3=0
JOBINDEX=""
ISPRIVATE=0

usage(){
    echo "`basename $0` {parseDatasetFile options} --type={type} [options]"
    echo "---------- provided by parseDatasetFile (all mandatory)"
    echo "    -r runRange"
    echo "    -d, --datasetpath path"
    echo "    -n, --datasetname name"
    echo "    --store dir"
    echo "    --remote_dir dir: origin files remote dir"
    echo "---------- provided by command-line (mandatory)"
    echo "    --type alcareco|alcarecosim|ALCARERECO|miniAOD|MINIAOD:"
    echo "           alcareco: produced on data"
    echo "           ALCARECOSIM|alcarecosim: alcareco produced on MC"
    echo "           ALCARERECO: alcareco format after rereco on ALCARAW"
    echo "           miniAOD|MINIAOD: ntuple production from miniAOD"
    echo " *** for MC ***"
    echo "    --isMC"
    echo "    --isParticleGun: redundant, --skim=partGun is the same"
    echo " *** for DATA ***"
    echo "    --json_name jsonName: additional name in the folder structure to keep track of the used json"
    echo "    --json jsonFile.root"

    echo "---------- optional common"
    echo "    --doExtraCalibTree"
    echo "    --doEleIDTree"
    echo "    --doPdfSystTree"
    echo "    --noStandardTree"
    echo "    --createOnly"
    echo "    --submitOnly"
    echo "    --check"
    echo "    --isPrivate: it is a privately produced dataset (not central or prompt)"

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
    echo "    --develRelease: CRAB do not check if the CMSSW version is in production (only if you are sure what you are doing)"
}

    
#------------------------------ parsing

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hHd:n:s:r:t:f: -l help,expertHelp,datasetpath:,datasetname:,skim:,runrange:,store:,remote_dir:,scheduler:,isMC,isParticleGun,ntuple_remote_dir:,json:,tag:,type:,json_name:,ui_working_dir:,extraName:,doExtraCalibTree,doEleIDTree,doPdfSystTree,noStandardTree,createOnly,submitOnly,check,isPrivate,file_per_job:,develRelease -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options
#echo $options

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
		    isMC=1
		    ;;
		MINIAOD| miniAOD)
		    TYPE=MINIAODNTUPLE
		    ;;
		ALCARECOSIM)
		    isMC=1
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
	--isMC) isMC=1;;# TYPE=ALCARECOSIM;;
	--isParticleGun) isPARTICLEGUN="y"; SKIM=partGun;;
 	--json) JSONFILE=$2;  shift;;
	--json_name) JSONNAME=$2; shift;;

	-f|--filelist) FILELIST="$FILELIST $2"; echo ${FILELIST}; shift ;;
	-s|--skim) SKIM=$2 ; shift;;
	-t | --tag) 
			if [ -n "${TAG}" ];then
				if [ "`basename $2 .py`" != "${TAG}" ];then
					echo "[ERROR] -t options called twice with different tag!" >> /dev/stderr
					exit 1
				fi
			else
				TAGFILE=$2;
				case $TAGFILE in
					config/reRecoTags/*) TAG=`basename ${TAGFILE} .py` ;;
					*) TAG=$TAGFILE; TAGFILE=config/reRecoTags/$TAG.py;;
				esac						
			echo "[OPTION] GLOBALTAG:$TAGFILE"; 
			fi
			shift 
			;;
	--ntuple_store) STORAGE_ELEMENT=$2; shift;;
	--ui_working_dir) UI_WORKING_DIR=$2; shift;;
	--scheduler) SCHEDULER=$2; shift;;
	#--puWeight) PUWEIGHTFILE=$2; shift;;
	--extraName) EXTRANAME=$2;shift;;
        #name of the output files is hardcoded in ZNtupleDumper
	--doExtraCalibTree) echo "[OPTION] doExtraCalibTree"; let DOTREE=${DOTREE}+2; OUTFILES="${OUTFILES},extraCalibTree.root";;
	--doEleIDTree) let DOTREE=${DOTREE}+4;OUTFILES="${OUTFILES},eleIDTree.root";;
	--doPdfSystTree) let DOTREE=${DOTREE}+8;;
	--noStandardTree) let DOTREE=${DOTREE}-1; OUTFILES=`echo ${OUTFILES} | sed 's|ntuple.root,||'`;;
	--createOnly) echo "[OPTION] createOnly"; unset SUBMIT;;
	--submitOnly) echo "[OPTION] submitOnly"; unset CREATE;;
	--check)      
			#echo "[OPTION] checking jobs"; 
			CHECK=y; EXTRAOPTION="--check"; unset CREATE; unset SUBMIT;;
	--isPrivate)      echo "[OPTION] private dataset"; ISPRIVATE=1;;

 	--file_per_job) FILE_PER_JOB=$2; shift ;;
	--develRelease) echo "[OPTION] Request also CMSSW release not in production!"; DEVEL_RELEASE=y;;

	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

echo "[OPTION] doExtraCalibTree"; let DOTREE=${DOTREE}+2; OUTFILES="${OUTFILES},extraCalibTree.root";

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

#if [ -z "$JSONFILE" -a "$TYPE" != "ALCARECOSIM" ];then 
#    echo "[ERROR] JSONFILE not defined" >> /dev/stderr
#    usage >> /dev/stderr
#    exit 1
#fi
#
#if [ -z "$JSONNAME" -a "$TYPE" != "ALCARECOSIM" ];then 
#    echo "[ERROR] JSONNAME not defined" >> /dev/stderr
#    usage >> /dev/stderr
#    exit 1
#fi

#Setting the ENERGY variable
setEnergy $DATASETPATH


#case ${isMC} in #FIXME
#    1) PDFSYST=1;;
#    0) PDFSYST=0;;
#esac #FIXME

if [ "${TYPE}" != "ALCARERECO" -a "${TYPE}" != "ALCARAW" ];then
    ORIGIN_REMOTE_DIR_BASE=`echo ${ORIGIN_REMOTE_DIR_BASE} | sed 's|alcaraw|alcareco|g'`
#elif [ "${TYPE}" != "alcarereco" ];then
#    echo ${ORIGIN_REMOTE_DIR_BASE}
#    ORIGIN_REMOTE_DIR_BASE=`echo ${ORIGIN_REMOTE_DIR_BASE} | sed 's|sandbox|alcareco|g'`
fi



#JOBINDEX=`cat crab_projects/crab_${DATASETNAME}/crab.log | grep -oh -m1 '[0-9]\{6\}_[0-9]\{6\}'` 
#echo " JOB INDEX $JOBINDEX"

#echo "[INFO] Temporarily set Crab to CRAB2 to allow CAF submission"
#echo "[INFO] You can set yourself back to CRAB3 by running iniCmsEnv.sh again"
#source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh


setStoragePath $STORAGE_ELEMENT $SCHEDULER 
###
###
# make the filelist before parsing the options and arguments
options="-d ${DATASETPATH} -n ${DATASETNAME} -r ${RUNRANGE} --remote_dir ${ORIGIN_REMOTE_DIR_BASE}"
case $TYPE in 
	ALCARERECO)
		options="$options -t ${TAGFILE}"; 
		setUserRemoteDirAlcarereco $ORIGIN_REMOTE_DIR_BASE
		ORIGIN_REMOTE_DIR=${USER_REMOTE_DIR}
		;;
	*)
		TAG=""
		setUserRemoteDirAlcareco $ORIGIN_REMOTE_DIR_BASE
		ORIGIN_REMOTE_DIR=${USER_REMOTE_DIR}
		;;
esac



if [ -z "$USER_REMOTE_DIR_BASE" ];then 
    echo "[ERROR] USER_REMOTE_DIR_BASE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

UI_WORKING_DIR=$UI_WORKING_DIR/${TYPE}/${TAG}/${DATASETNAME}/${RUNRANGE}/${JSONNAME}/${EXTRANAME}

USER_REMOTE_DIR=$USER_REMOTE_DIR_BASE/${ENERGY}/${TYPE}
if [ -n "${TAG}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${TAG}; fi
USER_REMOTE_DIR=$USER_REMOTE_DIR/${DATASETNAME}/${RUNRANGE}
if [ -n "${JSONNAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${JSONNAME}; fi
if [ -n "${EXTRANAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${EXTRANAME}; fi

if [ -z "${CHECK}" ];then
	if [ "${TYPE}" == "ALCARERECO" ];then
		if [ "`cat ntuple_datasets.dat | grep ${DATASETNAME}  | grep ${JSONNAME} | grep $TAG | grep -c $RUNRANGE`" != "0" ];then
			echo "[WARNING] Ntuple for rereco $TAG already done for ${RUNRAGE} ${DATASETNAME}"

			for file in `eos.select ls -l $STORAGE_PATH/$USER_REMOTE_DIR/  | sed '/^d/ d' | awk '{print $9}'`
			do 
				echo "[FILE] $STORAGE_PATH/$USER_REMOTE_DIR/$file"
			done
			exit 0
		fi
#else
#    if [ "`cat ntuple_datasets.dat | grep -v ALCARERECO | grep ${DATASETNAME} | grep ${JSONNAME} | grep -c $RUNRANGE`" != "0" ];then
#	echo "[WARNING] Ntuple for $JSONNAME  already done for ${RUNRANGE} ${DATASETNAME}"
#	exit 0
#    fi
	fi
fi
USER_REMOTE_DIR=$USER_REMOTE_DIR/unmerged


#==============================



###############################


#------------------------------




#${ENERGY}/
#${DATASETNAME}/tmp-${DATASETNAME}-${RUNRANGE}
OUTFILES=`echo $OUTFILES | sed 's|^,||'`

if [ -n "${CREATE}" ];then

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
	    #sample=tempFileList-${DATASETNAME}-${RUNRANGE}-${TAG}
			echo $USER
			echo "makefilelist.sh -f filelist/${TAG} `basename ${FILELIST} .list` $STORAGE_PATH/ /${ORIGIN_REMOTE_DIR}"
	    makefilelist.sh -f "filelist/${TAG}" `basename ${FILELIST} .list` $STORAGE_PATH/${ORIGIN_REMOTE_DIR}  || exit 1
	    #filelistDatasets.sh $options || exit 1
            # remove PUDumper files!
	    if [ -n "$TAG" ];then
			sed -i '/PUDumper/ d' filelist/*/*.list
			sed -i '/ntuple/ d'   filelist/*/*.list
	    else
			sed -i '/PUDumper/ d' filelist/*.list
			sed -i '/ntuple/ d' filelist/*.list
	    fi
	fi
	;;
esac

if [ -n "$FILELIST" ]; then
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

if [ ! -d "tmp" ];then mkdir tmp/; fi
cat > ${crabFile} <<EOF
[CRAB]
scheduler=$SCHEDULER
jobtype=cmssw
#use_server=$USESERVER

[CMSSW]
EOF
case ${ORIGIN_REMOTE_DIR_BASE} in
        database)
        cat >> ${crabFile} <<EOF
total_number_of_lumis = -1
lumis_per_job=${LUMIS_PER_JOBS}
datasetpath=${DATASETPATH}
#dbs_url = phys03
use_dbs3 = 1
EOF
        ;;
        *)
        cat >> ${crabFile} <<EOF
total_number_of_events=${NJOBS}
number_of_jobs=${NJOBS}
datasetpath=None
EOF
	;;
esac

if [ -n "${DEVEL_RELEASE}" ]; then
cat >> ${crabFile} <<EOF
allow_NonProductionCMSSW = 1
EOF
fi

echo "Crab config file is ${crabFile}"
cat >> ${crabFile} <<EOF
runselection=${RUNRANGE}
split_by_run=0
check_user_remote_dir=1
pset=python/alcaSkimming.py
#pycfg_params=type=${TYPE} doTree=${DOTREE} doTreeOnly=1 pdfSyst=${PDFSYST} jsonFile=${JSONFILE} isCrab=1 skim=${SKIM} tagFile=config/reRecoTags/test75x.py isPrivate=$ISPRIVATE
pycfg_params=type=${TYPE} doTree=${DOTREE} doTreeOnly=1 pdfSyst=${PDFSYST} jsonFile=${JSONFILE} isCrab=1 skim=${SKIM} tagFile=${TAGFILE} isPrivate=$ISPRIVATE
get_edm_output=1
output_file=${OUTFILES}


use_parent=0
#lumi_mask=


[LSF]
queue = 1nh
[CAF]
queue = cmscaf1nd
resource = type==SLC6_64

[USER]

ui_working_dir=${UI_WORKING_DIR}
return_data = 0
copy_data = 1

storage_element=$STORAGE_ELEMENT
user_remote_dir=$USER_REMOTE_DIR
#storage_path=$STORAGE_PATH
storage_path=/afs/cern.ch/work/g/gfasanel/CMSSW_7_4_15/src/Calibration/EcalAlCaRecoProducers
thresholdLevel=50
eMail = shervin@cern.ch

[GRID]

rb = HC
rb = CERN
proxy_server = myproxy.cern.ch

EOF


    crab -cfg ${crabFile} -create || exit 1
    if [ -n "$FILELIST" ];then
	  makeArguments.sh -f $FILELIST -u $UI_WORKING_DIR -n $FILE_PER_JOB || exit 1
    fi
    splittedOutputFilesCrabPatch.sh -u $UI_WORKING_DIR
fi

if [ -n "$SUBMIT" -a -z "${CHECK}" ];then
    crab -c ${UI_WORKING_DIR} -submit
    STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TYPE}\t${TAG}\t${JSONNAME}"
    echo -e $STRING >> ntuple_datasets.dat

#else
    #echo "crab -c ${UI_WORKING_DIR} -submit"
fi

OUTFILES=`echo ${OUTFILES} | sed 's|,| |'`

if [ -n "${CHECK}" ];then
    if [ ! -e "${UI_WORKING_DIR}/res/finished" ];then
	#echo $dir >> tmp/$TAG.log 
		echo "[STATUS] Unfinished ${UI_WORKING_DIR}"
		resubmitCrab.sh -u ${UI_WORKING_DIR}
    else
		if [ "${isMC}" == "1" ];then
			OUTFILES="$OUTFILES PUDumper"
		fi
		for file in $OUTFILES
		do
			file=`basename $file .root`
#			echo "FILE $file"
			mergeOutput.sh -u ${UI_WORKING_DIR} -g $file
		done
    fi
#    echo "mergeOutput.sh -u ${UI_WORKING_DIR} -n ${DATASETNAME} -r ${RUNRANGE}"
fi

