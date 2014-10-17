#!/bin/bash

checkVERSION(){
    case $CMSSW_VERSION in
	CMSSW_5_3_7_patch6)
	    echo "[ERROR] $CMSSW_VERSION now deprecated, please use the CMSSW_5_3_14_patch2"
	    exit 1
	    ;;
	CMSSW_5_3_14_patch2)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV)"
	    ;;
	CMSSW_7_0_*)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV)"
	    ;;
	CMSSW_7_2_0_pre7)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV)"
	    ;;
	*)
	    echo "[ERROR] Sorry, $CMSSW_VERSION not configured for ECALELF"
	    echo "        Be sure that you don't want 4_2_8_patch7 or CMSSW_4_4_5_patch2 or 5_3_14_patch2 or CMSSW_7_0_*"
	    
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
    *)
	checkVERSION
	;;
esac

export CMSSW_VERSION


# put in the right directory
cd $CMSSW_BASE/src

#########################################################################################
echo "[STATUS] Download of the skims"
# last WSkim version
git-cms-addpkg DataFormats/EgammaCandidates  #>> setup.log || exit 1

#########################################################################################
echo "[STATUS] Patch GsfElectrons for ECALELF rereco"
sed -i 's|[/]*assert|////assert|' DataFormats/EgammaCandidates/src/GsfElectron.cc 

#########################################################################################
echo "[STATUS] Download ECALELF directory"
myDir=Calibration
if [ ! -d "$myDir" ];then
    case "$USER" in 
	shervin)
	    git clone git@github.com:ECALELFS/ECALELF.git $myDir  >> setup.log || exit 1 # read-only mode
	    ;;
	hengne)
	    git clone git@github.com:hengne/ECALELF.git $myDir >> setup.log || exit 1 # read-only mode
	    cd $myDir/ALCARAW_RECO/
	    git clone https://github.com/hengne/Utilities.git bin
	    ;;
	lbrianza)
	    git clone git@github.com:lbrianza/ECALELF.git $myDir >> setup.log || exit 1 # read-only mode		
	    cd $myDir/
	    case $CMSSW_VERSION in
		CMSSW_7_2_0_pre7)
		    echo "[INFO] Checkout: download for 720_pre7"
		    git checkout devel_7_2_0_pre7
		    ;;
	    esac
	    ;;
	*)
            ### if you are not Shervin download this to have some useful scripts
	    git clone https://github.com/ECALELFS/ECALELF.git $myDir >> setup.log || exit 1 # read-only mode
	    cd $myDir/ALCARAW_RECO/
	    git clone https://github.com/ECALELFS/Utilities.git bin
	    ;;
    esac
fi

cd $CMSSW_BASE/src

#Other package to download:
# - Last stable pattuple code:
case $CMSSW_VERSION in
    CMSSW_5_*)
	git clone https://github.com/cms-cvs-history/DPGAnalysis-Skims DPGAnalysis/Skims  >> setup.log || exit 1
	cd DPGAnalysis/Skims/
	git checkout DPGAnalysis-Skims-V01-00-07  >> setup.log || exit 1
	cd -

	#addpkg  PhysicsTools/PatAlgos V08-10-05 >> setup.log
###### - Electron regression 5_3_X
	git clone https://github.com/cms-cvs-history/DataFormats-PatCandidates DataFormats/PatCandidates >> setup.log || exit 1
        cd DataFormats/PatCandidates >> setup.log || exit 1
        git checkout DataFormats-PatCandidates-V06-05-06-10 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/PhysicsTools-PatAlgos PhysicsTools/PatAlgos  >> setup.log || exit 1
        cd PhysicsTools/PatAlgos >> setup.log || exit 1
        git checkout PhysicsTools-PatAlgos-V08-09-56 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/PhysicsTools-PatUtils PhysicsTools/PatUtils  >> setup.log || exit 1
        cd PhysicsTools/PatUtils >> setup.log || exit 1
        git checkout PhysicsTools-PatUtils-V03-09-28 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/DataFormats-CaloRecHit DataFormats/CaloRecHit  >> setup.log || exit 1
        cd DataFormats/CaloRecHit >> setup.log || exit 1
        git checkout DataFormats-CaloRecHit-V02-05-11 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/DataFormats-StdDictionaries DataFormats/StdDictionaries  >> setup.log || exit 1
        cd DataFormats/StdDictionaries >> setup.log || exit 1
        git checkout DataFormats-StdDictionaries-V00-02-14 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/FWCore-GuiBrowsers FWCore/GuiBrowsers  >> setup.log || exit 1
        cd FWCore/GuiBrowsers >> setup.log || exit 1
        git checkout FWCore-GuiBrowsers-V00-00-70 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/RecoMET-METProducers RecoMET/METProducers  >> setup.log || exit 1
        cd RecoMET/METProducers >> setup.log || exit 1
        git checkout RecoMET-METProducers-V03-03-12-02 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	git clone https://github.com/cms-cvs-history/RecoEgamma-EgammaTools RecoEgamma/EgammaTools  >> setup.log || exit 1
        cd RecoEgamma/EgammaTools >> setup.log || exit 1
        git checkout RecoEgamma-EgammaTools-V09-00-01 >> setup.log || exit 1
        cd - >> setup.log || exit 1
        git clone https://github.com/cms-analysis/EgammaAnalysis-ElectronTools EgammaAnalysis/ElectronTools >> setup.log || exit 1
        cd EgammaAnalysis/ElectronTools  >> setup.log || exit 1
	git checkout EgammaAnalysis-ElectronTools-MD-21Apr2013-test-2 >> setup.log || exit 1
        cd - >> setup.log || exit 1
	patch -p0 < $myDir/ALCARAW_RECO/test/electronRegression.patch >> setup.log || exit 1

	cd EgammaAnalysis/ElectronTools/data/ >> setup.log || exit 1
	cat download.url | xargs wget  >> setup.log || exit 1
	cd - >> setup.log || exit 1
###### New Josh regression
	mkdir HiggsAnalysis/
	cd HiggsAnalysis/
	git clone -b legacyCompatibility https://github.com/bendavid/GBRLikelihood.git 
	git clone -b hggpaperV6 https://github.com/bendavid/GBRLikelihoodEGTools.git
	cd -
	mv GBRLikelihoodEGTools/data/*.root $myDir/EleNewEnergiesProducer/data/

	echo "[STATUS] applying patch for CMSSW_5_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc


    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
        git-cms-addpkg RecoEcal/EgammaCoreTools >> setup.log || exit 1
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < $myDir/ALCARAW_RECO/test/clusterLazyTools.patch >> setup.log || exit 1
    fi
    #patch  -p0 < $myDir/ALCARAW_RECO/test/class_def.xml.patch >> setup.log || exit 1

	cp /afs/cern.ch/user/b/bendavid/cmspublic/regweights52xV3/*.root $myDir/EleNewEnergiesProducer/data/ >> setup.log || exit 1

	# for pdfSystematics
	scram setup lhapdffull
	eval `scramv1 runtime -sh`
	git-cms-addpkg ElectroWeakAnalysis/Utilities
	scram b -j16

	;;
    CMSSW_7_2_0_pre7)
	git-cms-addpkg EgammaAnalysis/ElectronTools >> setup.log || exit 1
	patch -p1 < $myDir/ALCARAW_RECO/test/electronRegression700.patch >> setup.log || exit 1

	cd EgammaAnalysis/ElectronTools/data/ >> setup.log || exit 1
	cat download.url | grep '.root' | xargs wget  >> setup.log || exit 1
	cd - >> setup.log || exit 1

###### New Josh regression
	mkdir HiggsAnalysis/
	cd HiggsAnalysis/
	git clone -b legacyCompatibility https://github.com/bendavid/GBRLikelihood.git 
	git clone -b hggpaperV6 https://github.com/bendavid/GBRLikelihoodEGTools.git
	cd -
	mv GBRLikelihoodEGTools/data/*.root $myDir/EleNewEnergiesProducer/data/

	sed -i 's|REGRESSION=3|REGRESSION=4|' Calibration/*/BuildFile.xml
	echo "<Flags CppDefines=\"CMSSW_7_0_X\"/>" >> $myDir/EleSelectionProducers/BuildFile.xml

	echo "[STATUS] applying patch for CMSSW_5_X and following"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc

	echo "[STATUS] applying patch for CMSSW_7_2_0_pre7"
	patch -p1 < $myDir/ALCARAW_RECO/test/EGEnergyCorrectorSemiParm.h.patch >> setup.log || exit 1
	patch -p1 < $myDir/ALCARAW_RECO/test/EGEnergyCorrectorTraditional.h.patch >> setup.log || exit 1
	patch -p1 < $myDir/ALCARAW_RECO/test/EGEnergyAnalyzerSemiParm.cc.patch >> setup.log || exit 1

    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
        git-cms-addpkg RecoEcal/EgammaCoreTools >> setup.log || exit 1
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < $myDir/ALCARAW_RECO/test/clusterLazyTools.patch >> setup.log || exit 1
    fi

    cp /afs/cern.ch/user/b/bendavid/cmspublic/regweights52xV3/*.root $myDir/EleNewEnergiesProducer/data/ >> setup.log || exit 1

	;;

    CMSSW_7_*)
	git-cms-addpkg EgammaAnalysis/ElectronTools >> setup.log || exit 1
	patch -p1 < $myDir/ALCARAW_RECO/test/electronRegression700.patch >> setup.log || exit 1

	cd EgammaAnalysis/ElectronTools/data/ >> setup.log || exit 1
	cat download.url | grep '.root' | xargs wget  >> setup.log || exit 1
	cd - >> setup.log || exit 1

###### New Josh regression
	mkdir HiggsAnalysis/
	cd HiggsAnalysis/
	git clone -b legacyCompatibility https://github.com/bendavid/GBRLikelihood.git 
	git clone -b hggpaperV6 https://github.com/bendavid/GBRLikelihoodEGTools.git
	cd -
	mv GBRLikelihoodEGTools/data/*.root $myDir/EleNewEnergiesProducer/data/

	sed -i 's|REGRESSION=3|REGRESSION=4|' Calibration/*/BuildFile.xml
	echo "<Flags CppDefines=\"CMSSW_7_0_X\"/>" >> $myDir/EleSelectionProducers/BuildFile.xml

	echo "[STATUS] applying patch for CMSSW_5_X and following"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > $myDir/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc
	

    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
        git-cms-addpkg RecoEcal/EgammaCoreTools >> setup.log || exit 1
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < $myDir/ALCARAW_RECO/test/clusterLazyTools.patch >> setup.log || exit 1
    fi

    cp /afs/cern.ch/user/b/bendavid/cmspublic/regweights52xV3/*.root $myDir/EleNewEnergiesProducer/data/ >> setup.log || exit 1



esac



# compile
scram b -j16

# for file in `find -name '*.url'`; 
#   do 
#   if [ "`basename $file`" == "copy.url" ];then 
#       #echo $file; 
#       for cpFile in `cat $file`; 
# 	do 
# 	cp -u $cpFile `dirname $file`; 
#       done; 
#   fi; 
#   if [ "`basename $file`" == "download.url" ];then 
#       echo $file; 
#       for cpFile in `cat $file`; 
# 	do 
# 	cvs co -l -d `dirname $file` $cpFile
#       done; 
#   fi; 
  
# done

####-------->  Each time you set your environment!
#PATH=$PATH:$CMSSW_BASE/$myDir/ALCARAW_RECO/bin
#Previous command MUST be executed every time you log in 

