#!/bin/bash

outDirMC="./output/MC"
outDirData="./output/Data"

mkdir -p ${outDirMC}
mkdir -p ${outDirData}

if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi

./bin/ZFitter.exe -f data/validation/EoverPcalibration_2016.dat --regionsFile=./data/regions/validation.dat  --invMass_var="invMass_SC" \
		  --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
		  --outDirImgMC=${outDirMC}/img   --outDirImgData=${outDirData}/img \
		  --imgFormat=pdf \
		  $FLOATTAILSOPT > ${outDirData}/log/validation.log ||  exit 1
#--commonCut=${commonCut} \
#--selection=${selection} \
#$NOPUOPT \
