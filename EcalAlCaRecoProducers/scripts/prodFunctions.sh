checkInitECALELF(){
    if [ -z "$ECALELFINIT" ];then
	echo "[ERROR] ECALELF not initialized: execute the following command" >> /dev/stderr
	case $SHELL in
	    *bash)
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
    RELEASE=`DAS.py --query="release dataset=$1"`
    echo "[INFO] Dataset release: $RELEASE"
    echo "       Actual relase: $CMSSW_VERSION"
}


setEnergy(){
    #$1 = DATASETPATH
    case $1 in 
	*Run2012*)
	    #echo "[INFO] Run on 2012 data: ENERGY=8TeV"
	    ENERGY=8TeV
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
    if [ -z "${ENERGY}"  -o -z "${DATASETNAME}" -o -z "${RUNRANGE}" ];then
	echo "[ERROR] `basename $0`: ENERGY or DATASETNAME or RUNRANGE not defined" >> /dev/stderr
	echo "                       ${ENERGY} or ${DATASETNAME} or ${RUNRANGE}" >> /dev/stderr
	exit 1
    fi
    USER_REMOTE_DIR=$1/${ENERGY}/${DATASETNAME}/${RUNRANGE}
}
