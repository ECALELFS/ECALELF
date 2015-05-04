#!/bin/bash

checkVERSION(){
    case $CMSSW_VERSION in
	CMSSW_5_3_9_patch3)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV): only for run dependent MC"
	    ;;
	CMSSW_5_3_7_patch5)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV)"
	    ;;
	CMSSW_4_2_8_patch7)
	    echo "[INFO] Installing for $CMSSW_VERSION (2011 7TeV)"
	;;
	CMSSW_4_4_5_patch2)
	    echo "[INFO] Installing for $CMSSW_VERSION (2011 7TeV)"
	    ;;
	CMSSW_6_1_*)
	    echo "[INFO] Installing for $CMSSW_VERSION (2012 8TeV)"
	    echo "[WARING] Development release, ECALELF not yet configured properly"
	    ;;
	*)
	    echo "[ERROR] Sorry, $CMSSW_VERSION not configured for ECALELF"
	    echo "        Be sure that you don't want 4_2_8_patch7 or CMSSW_4_4_5_patch2 or 5_3_7_patch5 or CMSSW_6_1_X"
	    
	    exit 1
	    ;;
    esac
}

case $# in
    1)
	echo "[STATUS] Creating $1 CMSSW release working area"
	CMSSW_VERSION=$1
	checkVERSION 
	scram project CMSSW ${CMSSW_VERSION}
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
cvs co -r V01-00-07 DPGAnalysis/Skims/python/WElectronSkim_cff.py  >> setup.log || exit 1
cvs co -r V01-00-07 DPGAnalysis/Skims/python/skim_detstatus_cfg.py >> setup.log || exit 1
cvs co -r V01-00-07 DPGAnalysis/Skims/python/skim_detstatus_cfi.py >> setup.log || exit 1

#########################################################################################
echo "[STATUS] Patch GsfElectrons for ECALELF rereco"
addpkg DataFormats/EgammaCandidates >> setup.log || exit 1
sed -i 's|[/]*assert|////assert|' DataFormats/EgammaCandidates/src/GsfElectron.cc 

#########################################################################################
echo "[STATUS] Download ECALELF directory"
if [ ! -d Calibration ];then
    git clone git@github.com:ECALELFS/ECALELF.git # >> ../setup.log || exit 1 #setup.sh #ZFitter 
fi
cd Calibration

cd ALCARAW_RECO/

### if you are not Shervin download this to have some useful scripts
if [ "$USER" != "shervin" ];then
svn co svn+ssh://svn.cern.ch/reps/analysis/trunk/bin bin
# Please be sure to add this directory to you default PATH variable
# for bash
#PATH=$PATH:$CMSSW_BASE/src/calibration/ALCARAW_RECO/bin
# for tcsh
#setenv PATH $PATH":$CMSSW_BASE/src//calibration/SANDBOX/bin" # (I think)
###
echo "[WARNING] Remember to do this after all cmsenv:"
echo "# for bash
PATH=$PATH:$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin
# for tcsh
setenv PATH $PATH\":$CMSSW_BASE/src/Calibration/ALCARAW_RECO/bin\" 
"

fi

cd $CMSSW_BASE/src

#Other package to download:
# - Last stable pattuple code:
case $CMSSW_VERSION in
    CMSSW_6_*)
	addpkg DataFormats/PatCandidates V06-05-13          >> setup.log || exit 1
	addpkg PhysicsTools/PatAlgos     V08-10-34          >> setup.log || exit 1
	addpkg PhysicsTools/PatUtils     V03-09-28          >> setup.log || exit 1
	addpkg RecoMET/METProducers      V03-03-17-00-01    >> setup.log || exit 1

###### - Electron regression 5_3_X
	#cvs co -r V09-00-01      RecoEgamma/EgammaTools     >> setup.log  
	addpkg RecoEgamma/EgammaTools                                >> setup.log || exit 1
	rm RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc           >> setup.log || exit 1
	rm RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h	     >> setup.log || exit 1
	cvs co RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc	     >> setup.log || exit 1
	cvs co RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h  >> setup.log || exit 1
#	cvs co -r V00-00-30-00 -d EGamma/EGammaAnalysisTools UserCode/EGamma/EGammaAnalysisTools  
#	cvs co -r Moriond_2013_V01-1    EgammaAnalysis/ElectronTools
###### - Regression from Josh 5_3_X
#	cvs co -r regressionMay18b RecoEgamma/EgammaTools
	#checkdeps -a
	cp /afs/cern.ch/user/b/bendavid/cmspublic/regweights52xV3/*.root Calibration/EleNewEnergiesProducer/data/

	echo "[STATUS] applying patch for CMSSW_6_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc
    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	addpkg RecoEcal/EgammaCoreTools  #V05-08-24
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	echo "[STATUS] patching clusterLazyTools"
	sed -i -f Calibration/ALCARAW_RECO/test/EcalClusterLazyTools_h.sed $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	#patch  -p0 < test/clusterLazyTools.patch
    fi
    patch  -p0 < Calibration/ALCARAW_RECO/test/class_def.xml.patch

	;;
    CMSSW_5_*)
	#addpkg  PhysicsTools/PatAlgos V08-10-05 >> setup.log
###### - Electron regression 5_3_X
	addpkg DataFormats/PatCandidates   V06-05-06-10 >> setup.log || exit 1
	addpkg PhysicsTools/PatAlgos       V08-09-56 >> setup.log || exit 1
	addpkg PhysicsTools/PatUtils       V03-09-28 >> setup.log || exit 1
	addpkg DataFormats/CaloRecHit      V02-05-11 >> setup.log || exit 1
	addpkg DataFormats/StdDictionaries V00-02-14 >> setup.log || exit 1
	addpkg FWCore/GuiBrowsers          V00-00-70 >> setup.log || exit 1
	addpkg RecoMET/METProducers        V03-03-12-02 >> setup.log || exit 1
	cvs co -r V09-00-01      RecoEgamma/EgammaTools     >> setup.log  
#	cvs co -r V00-00-02    EgammaAnalysis/ElectronTools
	cvs co -r MD-21Apr2013-test-2    EgammaAnalysis/ElectronTools
	patch -p0 < Calibration/ALCARAW_RECO/test/electronRegression.patch

	cd EgammaAnalysis/ElectronTools/data/
	cat download.url | xargs wget 
	cd -
###### - Regression from Josh 5_3_X
#	cvs co -r regressionMay18b RecoEgamma/EgammaTools
	#checkdeps -a
	cp /afs/cern.ch/user/b/bendavid/cmspublic/regweights52xV3/*.root Calibration/EleNewEnergiesProducer/data/

	echo "[STATUS] applying patch for CMSSW_5_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc
    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	addpkg RecoEcal/EgammaCoreTools  #V05-08-24
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < Calibration/ALCARAW_RECO/test/clusterLazyTools.patch
    fi
    patch  -p0 < Calibration/ALCARAW_RECO/test/class_def.xml.patch

	;;
    CMSSW_4_4*)
	echo "[STATUS] Last PAT version" 
###### - PAT
	addpkg DataFormats/PatCandidates V06-04-39    >> setup.log || exit 1
	addpkg PhysicsTools/PatAlgos     V08-07-52    >> setup.log || exit 1
	addpkg PhysicsTools/PatUtils     V03-09-18-01 >> setup.log || exit 1
	addpkg CommonTools/ParticleFlow  V00-03-05-01 >> setup.log || exit 1
	addpkg FWCore/GuiBrowsers        V00-00-70    >> setup.log || exit 1
	addpkg DataFormats/CaloRecHit      V02-05-11 >> setup.log || exit 1
	echo "[STATUS] Egamma regression packages"
###### - Egamma regression
	addpkg CondFormats/DataRecord V06-23-01                 >> setup.log || exit 1
	cvs co -r V01-02-13      CondFormats/EcalObjects        >> setup.log || exit 1                 
	cvs co -r V00-04-00      CondFormats/EgammaObjects      >> setup.log || exit 1
	cvs co -r V05-08-24      RecoEcal/EgammaCoreTools       >> setup.log || exit 1                 
	cvs co -r V08-11-10-02   RecoEgamma/EgammaTools	        >> setup.log || exit 1
	cvs co -r V00-00-02-44X    EgammaAnalysis/ElectronTools >> setup.log || exit 1
	patch -p0 < Calibration/ALCARAW_RECO/test/electronRegression.patch
	cd EgammaAnalysis/ElectronTools/data/
	cat download.url | xargs wget || exit 1
	cd -
	echo "[STATUS] Josh's regression"
###### - Regression from Josh 4_4_X
# #	cvs co -r V00-02-04 CondFormats/EgammaObjects
# 	cvs co -r regression_Dec3d RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc
# 	cvs co -r regression_Dec3d RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h
# 	cvs co -r regression_Dec3d RecoEgamma/EgammaTools/interface/PhotonFix.h
# 	cvs co -r regression_Dec3d RecoEgamma/EgammaTools/src/PhotonFix.cc
# 	cvs co -r V00-03-01 CondFormats/EgammaObjects/interface/GBRWrapper.h
# 	cvs co -r V00-03-01 CondFormats/EgammaObjects/src/GBRWrapper.cc

# 	cvs co -r regression44x_Dec3 CondFormats/DataRecord

	cvs co RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc       >> setup.log || exit 1
	cvs co RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h  >> setup.log || exit 1

	echo "[STATUS] applying patch for CMSSW_4_4_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc
	
	sed -i 's|hadTowOverEm|hadronicOverEm|' ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc
	if [ "`grep -c RecoEcal/EgammaCoreTool ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml`" == "0" ];then
	    echo "<use   name=\"RecoEcal/EgammaCoreTools\"/>" >> ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml
	fi
	if [ "`grep -c CondFormats/EgammaObjects ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml`" == "0" ];then
	    echo "<use   name=\"CondFormats/EgammaObjects\"/>" >> ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml
	fi
	patch  -p0 < Calibration/ALCARAW_RECO/test/class_def.xml.patch

    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < Calibration/ALCARAW_RECO/test/clusterLazyTools.patch
    fi
    sed -i ';s|REGRESSION=3|REGRESSION=1|' Calibration/*/BuildFile.xml
    sed -i 's|#||;s|REGRESSION=3|REGRESSION=2|' Calibration/ZNtupleDumper/BuildFile.xml

	
	;;
    CMSSW_4_2*)
	addpkg DataFormats/PatCandidates V06-04-19-05
	addpkg PhysicsTools/PatAlgos V08-06-58
	addpkg CommonTools/ParticleFlow
	cvs co CommonTools/ParticleFlow/python/pfNoPileUpIso_cff.py
###### - Regression from Josh 4_2_X
	cvs co -r regression_Dec3d RecoEgamma/EgammaTools # on Josh's instructions
	#cvs co -r regressionMay18b RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc
	#cvs co -r regressionMay18b RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h
	#sed -i 's|hcalOverEcalBc|hcalOverEcal|;s|hadTowOverEm|hadronicOverEm|' RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc
        ##cvs co -r V00-02-04 CondFormats/EgammaObjects # on Josh's instructions
	cvs co -r V00-03-01 CondFormats/EgammaObjects #still compatible
	
	cvs co -r regression42x_Dec3 CondFormats/DataRecord
	
	cp Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch Calibration/ALCARAW_RECO/src/ElectronRecalibSuperClusterAssociatorSH.cc
	if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	    addpkg RecoEcal/EgammaCoreTools  #V05-08-24
	    cvs update -r V05-08-02  RecoEcal/EgammaCoreTools/src/EcalClusterTools.cc
	    rm RecoEcal/EgammaCoreTools/src/EcalClusterLazyTools.cc
	    cvs update -r V05-08-14  RecoEcal/EgammaCoreTools/src/EcalClusterLazyTools.cc
	    rm RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	    cvs co -r 1.9  RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	    cvs update -r V05-08-14  RecoEcal/EgammaCoreTools/BuildFile.xml
	fi
	if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	    sed -i -f Calibration/ALCARAW_RECO/test/EcalClusterLazyTools_h.sed $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	fi
	
	sed -i 's|#||;s|REGRESSION=3|REGRESSION=2|' Calibration/*/BuildFile.xml
	
	if [ "`grep -c reducedBarrelRecHitCollection $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.h`" == "0" ]; then
	    sed -i '/embedPFCandidate_/ i edm::InputTag reducedEBRecHitCollection;\nedm::InputTag reducedEERecHitCollection;' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.h
	    sed -i 's|[/]*edm::InputTag  reducedEBRecHitCollection(string|//// edm::InputTag  reducedEBRecHitCollection(string|' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc
	    sed -i 's|[/]*edm::InputTag  reducedEERecHitCollection(string|//// edm::InputTag  reducedEERecHitCollection(string|' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc
	    sed -i '/produces<std::vector<Electron> >();/ a reducedEBRecHitCollection = iConfig.getParameter<edm::InputTag>("reducedBarrelRecHitCollection");\nreducedEERecHitCollection = iConfig.getParameter<edm::InputTag>("reducedEndcapRecHitCollection");' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc

	fi
	
	sed -i '/patSequence/ {s|+[ ]*eleRegressionEnergy||}' Calibration/ZNtupleDumper/python/patSequence_cff.py
	sed -i '/electronRegressionEnergyProducer_cfi/ d;/eleRegressionEnergy/ d' Calibration/ZNtupleDumper/python/patSequence_cff.py

	patch  -p0 < Calibration/ALCARAW_RECO/test/class_def.xml.patch


	;;
esac



# compile
scram b -j8

for file in `find -name '*.url'`; 
  do 
  if [ "`basename $file`" == "copy.url" ];then 
      #echo $file; 
      for cpFile in `cat $file`; 
	do 
	cp -u $cpFile `dirname $file`; 
      done; 
  fi; 
  if [ "`basename $file`" == "download.url" ];then 
      echo $file; 
      for cpFile in `cat $file`; 
	do 
	cvs co -l -d `dirname $file` $cpFile
      done; 
  fi; 
  
done

####-------->  Each time you set your environment!
#PATH=$PATH:$CMSSW_BASE/Calibration/ALCARAW_RECO/bin
#Previous command MUST be executed every time you log in 

