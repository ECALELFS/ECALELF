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
#./bin/ZFitter.exe -f data/validation/Moriond17_newRegrFix_LT.dat --regionsFile=data/regions/validation.dat --corrEleType=Moriond1723JanEleScales_Gain1asGain12 --corrEleFile=data/ScalesSmearings/Moriond17_23Jan_ele 

# ./script/GenRootChains.sh -f data/validation/Moriond17_newRegrFix_LT.dat --fitterOption="--smearEleType=Moriond1723JanEleScales" --corrEleType=Moriond17_23Jan_ele_Gain1asGain12
# mv tmp/Moriond17_newRegrFix_LT/ tmp/Moriond17_newRegrFix_LT_Gain1asGain12

# ./script/GenRootChains.sh -f data/validation/Moriond17_newRegrFix_LT.dat --fitterOption="--smearEleType=Moriond1723JanEleScales" --corrEleType=Moriond1723JanEleScales


python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF" "(80,80,100)" invMass_ECAL_ele --noScales --noSmearings  -n EB-noScales_noSmearings || exit 1

# python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF" "(80,80,100)" invMass_ECAL_ele --noScales   -n EB-noScales_withSmearings

# python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF" "(80,80,100)" invMass_ECAL_ele  -n EB-scales_smearings

# python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_50_70" "(80,80,100)" invMass_ECAL_ele  -n EB-Et_50_70

# python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_70_100" "(80,80,100)" invMass_ECAL_ele  -n EB-Et_70_100

# python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_100_200" "(80,80,100)" invMass_ECAL_ele  -n EB-Et_100_200

#python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_200_400" "(20,80,100)" invMass_ECAL_ele  -n EB-Et_200_400

#python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-Et_400_800" "(20,80,100)" invMass_ECAL_ele  -n EB-Et_400_800

python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-EtSingleEle_200_400" "(40,80,100)" invMass_ECAL_ele  -n EB-EtSingleEle_200_400

python macro/standardDataMC.py -d tmp/Moriond17_newRegrFix_LT/d_chain.root,data -s tmp/Moriond17_newRegrFix_LT/s_chain.root,MC "EB-eleID_loose25nsRun22016Moriond-Et_25-isEle-noPF-EtSingleEle_400_800" "(20,80,100)" invMass_ECAL_ele  -n EB-EtSingleEle_400_800







