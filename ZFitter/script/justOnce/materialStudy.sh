#!/bin/bash
regionFile=data/regions/absEtaSingleEle.dat
regionFile=data/regions/absEta.dat
regionFile=data/regions/nPV.dat
regionFile=data/regions/materialStudy.dat

invMassList="invMass_SC invMass_SC_regrCorr_ele invMass_SC_regrCorr_pho invMass_SC_corr invMass_rawSC invMass_e5x5 invMass"
invMassList="invMass_rawSC"
for invMass_var in $invMassList
  do
  case $invMass_var in 
      invMass_e5x5)
	  invMass_limits="--invMass_min=50 --invMass_max=100"
	  ;;
      *)
	  invMass_limits=""
	  ;;
  esac
  
  dirMC=test/MC/DYJets-Summer12-START53-NewG4-ZSkim-allRange/190456-203002-13Jul_PromptTS/WP80_PU/$invMass_var
  if [ ! -e $dirMC ];then mkdir $dirMC/fitres $dirMC/img -p; fi
  ./bin/ZFitter.exe --regionsFile ${regionFile} \
      -f data/validation/NewG4.dat \
      --outDirFitResMC $dirMC/fitres/ \
      --outDirImgMC $dirMC/img   --outDirFitResData test/data/test/fitres --outDirImgData test/data/test/img \
      --invMass_var ${invMass_var} ${invMass_limits} || exit 1
  
  dirMC=test/MC/DYJets-Summer12-START53-NewG4-X0Max-ZSkim-allRange/190456-203002-13Jul_PromptTS/WP80_PU/$invMass_var
  if [ ! -e $dirMC ];then mkdir $dirMC/fitres $dirMC/img -p; fi
  ./bin/ZFitter.exe --regionsFile ${regionFile} \
      -f data/validation/NewG4X0Max.dat \
      --outDirFitResMC $dirMC/fitres/ \
      --outDirImgMC $dirMC/img   --outDirFitResData test/data/test/fitres --outDirImgData test/data/test/img \
      --invMass_var ${invMass_var} ${invMass_limits} || exit 1

  dirMC=test/MC/DYJets-Summer12-START53-ZSkim-allRange/190456-203002-13Jul_PromptTS/WP80_PU/$invMass_var
  if [ ! -e $dirMC ];then mkdir $dirMC/fitres $dirMC/img -p; fi
  if [ ! -e test/data/monitoring_2012_53X/WP80_PU/$invMass_var ];then mkdir test/data/monitoring_2012_53X/WP80_PU/$invMass_var/fitres test/data/monitoring_2012_53X/WP80_PU/$invMass_var/img -p; fi

  ./bin/ZFitter.exe --regionsFile ${regionFile} \
      -f data/validation/monitoring_2012_53X.dat \
      --outDirFitResMC $dirMC/fitres/ --outDirImgMC $dirMC/img \
      --outDirFitResData test/data/monitoring_2012_53X/WP80_PU/$invMass_var/fitres \
      --outDirImgData test/data/monitoring_2012_53X/WP80_PU/$invMass_var/img \
      --invMass_var ${invMass_var} ${invMass_limits} || exit 

done


exit 0
DYJets-Summer12-START53-NewG4-ZSkim-allRange
