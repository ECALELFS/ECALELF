#!/bin/bash
echo "------------------------------"
echo $PATH
echo "------------------------------"
echo $USER
echo "------------------------------"
checkVERSION(){
    case $CMSSW_VERSION in
		CMSSW_5_3_21)
			echo "[`basename $0` ERROR] $CMSSW_VERSION (2012 8TeV analysis) is not anymore supported by ECALELF"
			exit 1
			;;
		CMSSW_7_6_3)
			echo "[`basename $0` ERROR] $CMSSW_VERSION (2015 13TeV analysis) is not anymore supported by ECALELF"
			exit 1
			;;
		 CMSSW_8_0_26_patch1)
			echo "[INFO] Installing for $CMSSW_VERSION (2016 13TeV)"
			;;
		*)
			echo "[`basename $0` ERROR] Sorry, $CMSSW_VERSION not configured for ECALELF"
			echo "                      Supported releases are:"
			echo "                       - CMSSW_8_0_24_patch1"
			exit 1
			;;
    esac
}

case $# in
    1)
		echo "[STATUS] Creating $1 CMSSW release working area"
		CMSSW_VERSION=$1
		checkVERSION 
		scram project CMSSW ${CMSSW_VERSION} || exit 1
		cd ${CMSSW_VERSION}/src
		eval `scramv1 runtime -sh`
		;;
	2)
		BRANCH=$2
		echo "[STATUS] Creating $1 CMSSW release working area"
		CMSSW_VERSION=$1
		checkVERSION 
		scram project CMSSW ${CMSSW_VERSION} || exit 1
		cd ${CMSSW_VERSION}/src
		eval `scramv1 runtime -sh`
		;;
	
    *)
		checkVERSION
		;;
esac

export CMSSW_VERSION


# put in the right directory
cd $CMSSW_BASE/src

#########################################################################################
git cms-init

#########################################################################################
echo "[STATUS] Download ECALELF directory"
myDir=Calibration
if [ ! -d "$myDir" ]; then

		echo "[INFO] user=$USER"
		case "$USER" in 
			shervin)
				git clone --single-branch https://shervin@gitlab.cern.ch/shervin/ECALELF.git $myDir  >> setup.log || exit 1 # read-only mode
				;;
			gitlab-runner)
				git clone  https://gitlab.cern.ch/shervin/ECALELF.git $myDir >> setup.log || exit 1 # read-only mode
				;;
		*)
            ### if you are not Shervin download this to have some useful scripts
				git clone  --single-branch https://shervin@gitlab.cern.ch/shervin/ECALELF.git $myDir >> setup.log || exit 1 # read-only mode
				cd $myDir/EcalAlCaRecoProducers/
				git clone  --single-branch https://github.com/ECALELFS/Utilities.git bin
				;;
    esac

fi

cd $CMSSW_BASE/src

#Other package to download:
# - Last stable pattuple code:
case $CMSSW_VERSION in
    CMSSW_8_0_*)
#		git cms-merge-topic 16790  || exit 1
#		git cms-merge-topic ikrav:egm_id_80X_v1 || exit 1
#		git cms-merge-topic shervin86:slewrate || exit 1
		;;
esac

# compile
echo "[INFO] Starting to compile"
case $USER in
    gitlab-runner)
	;;
    *)
	scram b -j16 || {
	    echo "[INFO for USERS] You could get a C++ seg fault: Be persistent! from $CMSSW_BASE/src go for a scram b -j16 again :-)"
	    scram b
	}
	;;
esac

