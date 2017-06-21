#!/bin/bash

#Create the EleIDSF branch and add it to the .dat file if not there yet
#./bin/ZFitter.exe -f data/validation/Moriond17_newRegrFix_LT.dat --regionsFile=data/regions/validation.dat --saveAddBranchTree --addBranch=EleIDSF_loose25nsRun22016Moriond

#Create the smearEle branch and add it to the .dat file if not there yet
#./bin/ZFitter.exe -f data/validation/Moriond17_newRegrFix_LT.dat --regionsFile=data/regions/validation.dat --smearEleType=Moriond1723JanEleScales --smearEleFile=data/ScalesSmearings/Moriond17_23Jan_ele 

#Create the scaleEle branch and add it to the .dat file if not there yet
#./bin/ZFitter.exe -f data/validation/Moriond17_newRegrFix_LT.dat --regionsFile=data/regions/validation.dat --corrEleType=Moriond1723JanEleScales --corrEleFile=data/ScalesSmearings/Moriond17_23Jan_ele 

#Create a .dat file with Gain1 corrections set to the same value as the Gain12 corrections
#sed '/gainEle_1\t/ d; /gainEle_12/ {p; s|gainEle_12|gainEle_1|}' data/ScalesSmearings/Moriond17_23Jan_ele_scales.dat  > data/ScalesSmearings/Moriond17_23Jan_ele_Gain1asGain12_scales.dat

#Create the scaleEle branch and add it to the .dat file if not there yet
#./bin/ZFitter.exe -f data/validation/Moriond17_newRegrFix_LT.dat --regionsFile=data/regions/validation.dat --corrEleType=Moriond1723JanEleScales_Gain1asGain12 --corrEleFile=data/ScalesSmearings/Moriond17_23Jan_ele_Gain1asGain12

#./script/GenRootChains.sh -f data/validation/Moriond17_newRegrFix_LT.dat --fitterOption="--smearEleType=Moriond1723JanEleScales" --corrEleType=Moriond1723JanEleScales_Gain1asGain12 --outDir tmp/Moriond17_newRegrFix_LT_Gain1asGain12

#./script/GenRootChains.sh -f data/validation/Moriond17_newRegrFix_LT.dat --fitterOption="--smearEleType=Moriond1723JanEleScales" --corrEleType=Moriond1723JanEleScales

for sample in Moriond17_newRegrFix_LT Moriond17_newRegrFix_LT_Gain1asGain12
#for sample in Moriond17_newRegrFix_LT
#for sample in Moriond17_newRegrFix_LT_Gain1asGain12
do
	plotsdir=plots/jetmetcheck/$sample
	mkdir -p $plotsdir
	branchname='invMass_ECAL_ele'
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF"                      "(80,80,100)"  "${branchname}"  --ratio .5,1.5 --noScales  --noSmearings  -n  EB_noScales_noSmearings        --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF"                      "(80,80,100)"  "${branchname}"  --ratio .5,1.5 --noScales                 -n  EB_noScales_withSmearings     --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF"                      "(80,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_scales_smearings           --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_50_70"             "(80,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_Et_50_70                   --plotdir=$plotsdir  &
	#wait $!
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_70_100"            "(80,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_Et_70_100                  --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_100_200"           "(80,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_Et_100_200                 --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_200_400"           "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_Et_200_400                 --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_400_800"           "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_Et_400_800                 --plotdir=$plotsdir  &
	#wait $!
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-EtSingleEle_200_400"  "(40,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_EtSingleEle_200_400        --plotdir=$plotsdir  &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-EtSingleEle_400_800"  "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_EtSingleEle_400_800        --plotdir=$plotsdir  &
	
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-gainEle_1"            "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_gainEle_1        --plotdir=$plotsdir &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-gainEle_12"           "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_gainEle_12       --plotdir=$plotsdir &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-gainEleSingleEle_1"   "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_gainEleSingleEle_1        --plotdir=$plotsdir &
	python  macro/standardDataMC.py  -d  tmp/$sample/d_chain.root,data  -s  tmp/$sample/s_chain.root,MC  "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-gainEleSingleEle_12"  "(20,80,100)"  "${branchname}"  --ratio .5,1.5                            -n  EB_gainEleSingleEle_12       --plotdir=$plotsdir &
	wait $!

done
wait $!
