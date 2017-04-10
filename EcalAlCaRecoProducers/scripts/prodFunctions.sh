checkInitECALELF(){
    if [ -z "$ECALELFINIT" ];then
	echo "[ERROR] ECALELF not initialized: execute the following command" >> /dev/stderr
	case $SHELL in
	    *bash)
			echo "For CAF and crab2 (re-recoes):" >> /dev/stderr
		echo "source $CMSSW_BASE/src/Calibration/initCmsEnv.sh">> /dev/stderr
		echo "For GRID (crab3)" >> /dev/stderr
		echo "source $CMSSW_BASE/src/Calibration/initCmsEnv.sh">> /dev/stderr
		;;
	    *csh)
		echo "source $CMSSW_BASE/src/Calibration/initCmsEnv.csh">> /dev/stderr
		;;
	    *)
		echo "if you use a bash-like shell:">> /dev/stderr
		echo "source $CMSSW_BASE/src/Calibration/initCmsEnv.sh">> /dev/stderr
		echo "if you use a csh-like shell:">> /dev/stderr
		echo "source $CMSSW_BASE/src/Calibration/initCmsEnv.csh" >> /dev/stderr
		;;
	esac
	exit 1
    fi
}

checkInitECALELF

setStoragePath(){
    #$1 = storage_element
    #$2 = scheduler
    #echo "[DEBUG] Setting storage path for $1 with scheduler $2"
    case $1 in
	caf* | T2_CH_CERN)
	    case $2 in
		caf|lsf)
		    STORAGE_ELEMENT=caf.cern.ch	
		    STORAGE_PATH=root://eoscms//eos/cms/store
		    ;;
	    #glite | glidein)
		remoteGlidein|condor)
		    STORAGE_ELEMENT=srm-eoscms.cern.ch
		    STORAGE_PATH=/srm/v2/server?SFN=/eos/cms/store
		#STORAGE_ELEMENT=caf.cern.ch	
		#STORAGE_PATH=root://eoscms//eos/cms/store
		    ;;
	    *)
		    echo "[ERROR] Scheduler $2 for storage_element $1 not implemented" >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    ;;
	T2_IT_Rome)
	    STORAGE_PATH=/srm/managerv2?SFN=/castor/cern.ch
	    ;;
	clusterCERN)
	    STORAGE_PATH=root://pccmsrm27.cern.ch:1094//cms/local
	    ;;
    esac
}


#------------------------------------------------------------
# checking the release
checkRelease(){
    #$1 = DATASETPATH
    RELEASE=`das_client --limit=0 --query="release dataset=$1"`
	m1=`echo $CMSSW_VERSION | cut -d '_' -f 2`
	m2=`echo $CMSSW_VERSION | cut -d '_' -f 3`
	m3=`echo $CMSSW_VERSION | cut -d '_' -f 4`
	mp=`echo $CMSSW_VERSION | cut -d '_' -f 5`
	for release in $RELEASE
	do
		echo "[INFO] Dataset release: $release"
		M1=`echo $release | cut -d '_' -f 2`
		M2=`echo $release | cut -d '_' -f 3`
		M3=`echo $release | cut -d '_' -f 4`
		MP=`echo $release | cut -d '_' -f 5`
		if  (( "${m1}" < "${M1}" )) || (( "${m2}" < "${M2}" )) || (( "${m3}" < "${M3}" )) ; then
			echo "[WARNING `basename $0`] Current release is older than the one used to produce the dataset!" >> /dev/stderr
			echo "                        Current release: $CMSSW_VERSION" >> /dev/stderr
			echo "                        Dataset release: $release" >> /dev/stderr
			exit 1
		fi
	done
    echo "       Actual relase: $CMSSW_VERSION"
}


setEnergy(){
    #$1 = DATASETPATH
    case $1 in
	*Run2016*)
	    #echo "[INFO] Run on 2011 data: ENERGY=7TeV"
	    ENERGY=13TeV
	    ;; 
	*RUN2012* | *Run2012*)
	    #echo "[INFO] Run on 2012 data: ENERGY=8TeV"
	    ENERGY=8TeV
	    ;;
	*Run2015*)
	    #echo "[INFO] Run on 2011 data: ENERGY=7TeV"
	    ENERGY=13TeV
	    ;;
	*Run2016*)
	    #echo "[INFO] Run on 2011 data: ENERGY=7TeV"
	    ENERGY=13TeV
	    ;;
	*Run2011*)
	    #echo "[INFO] Run on 2011 data: ENERGY=7TeV"
	    ENERGY=7TeV
	    ;;
	*Run2010*)
	    #echo "[INFO] Run on 2010 data: ENERGY=7TeV"
	    ENERGY=7TeV
	    ;;
	*Fall11*)
	    ENERGY=7TeV
	    ;;
	*Summer12*)
	    ENERGY=8TeV
	    ;;
	*13TeV*)
	    ENERGY=13TeV
	    ;;
	*8TeV*)
	    ENERGY=8TeV
	    ;;
	*7TeV*)
	    ENERGY=7TeV
	    ;;
	*Summer11*)
	    ENERGY=7TeV
	    ;;
	*RelValZEE_13*)
	    ENERGY=13TeV
	    ;;
	*SingleElectronFlat*)
	    ENERGY=gun
	    ;;
	*sharper*)
	    ENERGY=13TeV
	    ;;
	*)
	    echo "[ERROR] Center of mass energy not determined for $1" >> /dev/stderr
	    echo "        Check implementation in prodFunctions.sh"    >> /dev/stderr
	    exit 1
    esac
}


setUserRemoteDirAlcarereco(){
    #$1=USER_REMOTE_DIR_BASE
    if [ -z "${ENERGY}" -o -z "${TAG}" -o -z "${DATASETNAME}" -o -z "${RUNRANGE}" ];then
	echo "[ERROR] `basename $0`: ENERGY or TAG or DATASETNAME or RUNRANGE not defined" >> /dev/stderr
	echo "                       ${ENERGY} or ${TAG} or ${DATASETNAME} or ${RUNRANGE}" >> /dev/stderr
	exit 1
    fi
    USER_REMOTE_DIR=$1/${ENERGY}/${TAG}/${DATASETNAME}/${RUNRANGE}
}

setUserRemoteDirNtuple(){
    #$1=USER_REMOTE_DIR_BASE
    if [ -z "${ENERGY}" -o -z "${TYPE}" -o -z "${DATASETNAME}" -o -z "${RUNRANGE}" ];then
	echo "[ERROR] `basename $0`: ENERGY or TYPE or DATASETNAME or RUNRANGE not defined" >> /dev/stderr
	echo "                       ${ENERGY} or ${TAG} or ${DATASETNAME} or ${RUNRANGE}" >> /dev/stderr
	exit 1
    fi
    USER_REMOTE_DIR=$1/${ENERGY}/${TYPE}
    if [ -n "${TAG}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${TAG}; fi
    USER_REMOTE_DIR=$USER_REMOTE_DIR/${DATASETNAME}/${RUNRANGE}
    if [ -n "${JSONNAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${JSONNAME}; fi
    if [ -n "${EXTRANAME}" ];then USER_REMOTE_DIR=$USER_REMOTE_DIR/${EXTRANAME}; fi
#    USER_REMOTE_DIR=$USER_REMOTE_DIR/unmerged
}

setUserRemoteDirAlcareco(){
    #$1=USER_REMOTE_DIR_BASE
	if [ "$FROMCRAB3" == 1 ];then
		echo "DATASETPATH $DATASETPATH"
		DATASETPATHSHORT=`echo  $DATASETPATH | awk 'NR==1 {split($1, arr, "[/]"); print arr[2]}' `
		echo "DATASETPATH SHORT $DATASETPATHSHORT"
		
		if [ -z "${ENERGY}"  -o -z "${DATASETNAME}" -o -z "${RUNRANGE}" ];then
			echo "[ERROR] `basename $0`: ENERGY or DATASETNAME or RUNRANGE not defined" >> /dev/stderr
			echo "                       ${ENERGY} or ${DATASETNAME} or ${RUNRANGE}" >> /dev/stderr
			exit 1
		fi
		USER_REMOTE_DIR=$1/$USER/$TYPE/${ENERGY}/${DATASETPATHSHORT}/${DATASETNAME}/${JOBINDEX}/"0000"/
	else
		if [ -z "${ENERGY}"  -o -z "${DATASETNAME}" -o -z "${RUNRANGE}" ];then
			echo "[ERROR] `basename $0`: ENERGY or DATASETNAME or RUNRANGE not defined" >> /dev/stderr
			echo "                       ${ENERGY} or ${DATASETNAME} or ${RUNRANGE}" >> /dev/stderr
			exit 1
		fi
		case $1 in 
			database)
				USER_REMOTE_DIR=$1
				;;
			*)
				USER_REMOTE_DIR=$1/${ENERGY}/${DATASETNAME}/${RUNRANGE}
				;;
		esac
	fi
}



setGT(){
    ismc=$1
    energy=$2
    type=$3
    SQRTS=$ENERGY
		}



checkIfRerecoed(){
# the STORAGE_ELEMENT is not included in the check because EOS can have two different strings if running using lsf/caf or remoteGlidein
	STRING="${RUNRANGE}\t${DATASETPATH}\t${DATASETNAME}\t${STORAGE_ELEMENT}\t${USER_REMOTE_DIR_BASE}\t${TAG}"
	if [ "`grep ${RUNRANGE} alcarereco_datasets.dat | grep ${DATASETPATH} |grep -e \"${DATASETNAME}[[:space:]]\" | grep ${USER_REMOTE_DIR_BASE} | grep ${TAG}$ | wc -l`" == "0" ]; then 
		echo "NotRERECOED"
		return 1; 
	else 
		grep ${RUNRANGE} alcarereco_datasets.dat | grep -e "${DATASETNAME}[[:space:]]" | grep ${DATASETPATH} | grep ${STORAGE_ELEMENT} | grep ${USER_REMOTE_DIR_BASE} | grep ${TAG}$ 
		return 0; 
	fi
	#return is true if not-rerecoed
}

checkVerboseOption(){
	set -- $options
	while [ $# -gt 0 ]
	do
		case $1 in
			-v|--verbose)    VERBOSE=y;;
			(--) shift; break;;
#		(-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
			(*) break;;
		esac
		shift
	done
}