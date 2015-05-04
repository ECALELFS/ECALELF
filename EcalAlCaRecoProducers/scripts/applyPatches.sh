#!/bin/bash

if [ -z "$CMSSW_VERSION" ];then
    eval `scramv1 runtime -sh`
fi

case $CMSSW_VERSION in
    CMSSW_4_2_*)
#	echo "[STATUS] applying patch for CMSSW_4_2"
	cp src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch src/ElectronRecalibSuperClusterAssociatorSH.cc
	if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	    cd $CMSSW_BASE/src;
	    addpkg RecoEcal/EgammaCoreTools  #V05-08-24
	    cvs update -r V05-08-02  RecoEcal/EgammaCoreTools/src/EcalClusterTools.cc
	    rm RecoEcal/EgammaCoreTools/src/EcalClusterLazyTools.cc
	    cvs update -r V05-08-14  RecoEcal/EgammaCoreTools/src/EcalClusterLazyTools.cc
	    rm RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	    cvs co -r 1.9  RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	    cvs update -r V05-08-14  RecoEcal/EgammaCoreTools/BuildFile.xml
	    cd -
	fi
	if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	    #patch  -p0 < test/EcalClusterLazyTools.h_patch
	    sed -i -f test/EcalClusterLazyTools_h.sed $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	    #patch  -p0 < test/EcalClusterLazyTools.cc_patch
	fi
	
	sed -i 's|#||;s|REGRESSION=3|REGRESSION=2|' ../*/BuildFile.xml
	
	if [ "`grep -c reducedBarrelRecHitCollection $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.h`" == "0" ]; then
	    sed -i '/embedPFCandidate_/ i edm::InputTag reducedEBRecHitCollection;\nedm::InputTag reducedEERecHitCollection;' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.h
	    sed -i 's|[/]*edm::InputTag  reducedEBRecHitCollection(string|//// edm::InputTag  reducedEBRecHitCollection(string|' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc
	    sed -i 's|[/]*edm::InputTag  reducedEERecHitCollection(string|//// edm::InputTag  reducedEERecHitCollection(string|' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc
	    sed -i '/produces<std::vector<Electron> >();/ a reducedEBRecHitCollection = iConfig.getParameter<edm::InputTag>("reducedBarrelRecHitCollection");\nreducedEERecHitCollection = iConfig.getParameter<edm::InputTag>("reducedEndcapRecHitCollection");' $CMSSW_BASE/src/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc

	fi

	
	;;
    CMSSW_4_4_*)
	echo "[STATUS] applying patch for CMSSW_4_4_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > src/ElectronRecalibSuperClusterAssociatorSH.cc
	
	sed -i 's|hadTowOverEm|hadronicOverEm|' ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/src/EGEnergyCorrector.cc
	if [ "`grep -c RecoEcal/EgammaCoreTool ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml`" == "0" ];then
	    echo "<use   name=\"RecoEcal/EgammaCoreTools\"/>" >> ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml
	fi
	if [ "`grep -c CondFormats/EgammaObjects ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml`" == "0" ];then
	    echo "<use   name=\"CondFormats/EgammaObjects\"/>" >> ${CMSSW_BASE}/src/RecoEgamma/EgammaTools/BuildFile.xml
	fi
	patch  -p0 < test/class_def.xml

    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < test/clusterLazyTools.patch
    fi
    sed -i ';s|REGRESSION=3|REGRESSION=1|' ../*/BuildFile.xml
    sed -i 's|#||;s|REGRESSION=3|REGRESSION=2|' ../ZNtupleDumper/BuildFile.xml


	;;
    CMSSW_5_*)
	echo "[STATUS] applying patch for CMSSW_5_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > src/ElectronRecalibSuperClusterAssociatorSH.cc
    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	cd $CMSSW_BASE/src;
	addpkg RecoEcal/EgammaCoreTools  #V05-08-24
	cd -
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	patch  -p0 < test/clusterLazyTools.patch
    fi
    ;;
    CMSSW_6_*)
	echo "[STATUS] applying patch for CMSSW_6_X"
	sed 's|,eleIt->ecalEnergyError()\*(nearestSC.*);|);|' src/ElectronRecalibSuperClusterAssociatorSH.cc_topatch > src/ElectronRecalibSuperClusterAssociatorSH.cc
    if [ ! -e "$CMSSW_BASE/src/RecoEcal/EgammaCoreTools" ];then
	cd $CMSSW_BASE/src;
	addpkg RecoEcal/EgammaCoreTools  #V05-08-24
	cd -
    fi
    if [ "`grep -c getEcalEBRecHitCollection $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h`" == "0" ];then
	echo "[STATUS] patching clusterLazyTools"
	sed -i -f test/EcalClusterLazyTools_h.sed $CMSSW_BASE/src/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	#patch  -p0 < test/clusterLazyTools.patch
    fi
    ;;

    *)
	echo "[ERROR] Patch for release not prepared"
	exit 1
	;;
esac



#echo "[STATUS] ElectronRecalibSuperClusterAssociator replacing"
#cp ElectronRecalibSuperClusterAssociator.cc.tmp ../../src/ElectronRecalibSuperClusterAssociator.cc
#cp ElectronRecalibSuperClusterAssociator.h ../../interface

echo "[STATUS] GsfElectron class patching"
#patch ../../../../Data
sed -i 's|[/]*assert|////assert|' ../../DataFormats/EgammaCandidates/src/GsfElectron.cc 

#echo "[STATUS] Copying new AlCaElectronsProducer.cc"
#cp AlCaElectronsProducer.cc ../../../EcalAlCaRecoProducers/src/


#echo "[STATUS] Compiling EDM plugins "
#if [ "`grep -c EDM_PLUGIN ../../BuildFile.xml`" == "0" ]; then
#    echo "<flags   EDM_PLUGIN=\"1\"/>" >> ../../BuildFile.xml
#fi



