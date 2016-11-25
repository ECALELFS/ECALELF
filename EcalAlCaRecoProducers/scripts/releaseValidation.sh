#!/bin/bash
bold=$(tput bold)
normal=$(tput sgr0)

function testStep(){
	stepIndex=$1
	stepString=$2
	stepLog=$3
	stepCommand=$4

	if [ -e "tmp/releaseValidation/$stepIndex" ]; then
		echo  "[`basename $0`] $stepString OK "
		return 0
	fi
	if [ ! -e "tmp/releaseValidation/$stepLog.log" ];then
		echo -n "[`basename $0`] $stepString ... "
		$stepCommand &> tmp/releaseValidation/$stepLog.err || {
			echo "${bold}ERROR${normal}"
			echo "See tmp/releaseValidation/$stepLog"
			exit 1
		}
		mv tmp/releaseValidation/$stepLog.{err,log}
		echo "OK"
	else
		echo "[`basename $0`] $stepString already done and OK"
	fi
	return 1
}
	
############################################################
# script to make sure that the new release is not breaking things
############################################################

echo "[`basename $0`] CMSSW RELEASE: $CMSSW_VERSION"
if [ ! -d "tmp/releaseValidation" ];then mkdir tmp/releaseValidation -p; fi

case $CMSSW_VERSION in
	CMSSW_8_0_*)
		fileMINIAOD=/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1/00000/008099CA-5501-E611-9AAE-24BE05BDCEF1.root
		;;
	*)
		echo "CMSSW RELEASE not supported" >> /dev/stderr
		exit 1
		;;
esac


############################################################
# production in local of ntuples from MINIAOD
testStep 1 "Testing local production of ntuples from MINIAODSIM" ntuple_miniaodsim "cmsRun python/alcaSkimming.py tagFile=config/reRecoTags/80X_mcRun2_asymptotic_2016_miniAODv2.py type=MINIAODNTUPLE maxEvents=100 doTree=1 files=$fileMINIAOD" || {

python test/dumpNtuple.py 1> tmp/ntuple_miniaodsim.dump 2> tmp/releaseValidation/ntuple_miniaodsim_2.log || {
	echo "${bold}ERROR${normal}"
	echo "See tmp/releaseValidation/ntuple_miniaodsim_2.log" 
	exit 1
}

echo -n "[`basename $0`] Checking difference in dump of ntuple content ... "
diff -q {tmp,test}/ntuple_miniaodsim.dump > /dev/null || {
	echo "${bold}ERROR${normal}"
	echo "{tmp,test}/ntuple_miniaodsim.dump are different" 
	echo "you can use"
	echo "wdiff -n {tmp,test}/ntuple_miniaodsim.dump | colordiff  | less -RS"
	exit 1
}
echo "OK"
touch tmp/releaseValidation/1
}

exit 0


