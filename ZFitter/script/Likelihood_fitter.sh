#!/bin/bash
source script/functions.sh
if [[ $1 = "RUN2_200_corr" ]]; then
   validation_file=miniAOD_November2015.dat
   region=scaleStep0_highMass
   commonCut=Et_35-noPF
   outDirData=/afs/cern.ch/work/g/gfasanel/HighMass_200/test/dato
   extension=ptRatio_pt2Sum
   InitFile=init_RUN2_highMass_Corr.txt
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*300
   BinWidth=0.02*6
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "RUN2_200_NOCorr" ]]; then
   validation_file=miniAOD_November2015.dat
   region=scaleStep0_highMass
   commonCut=Et_35-noPF
   outDirData=/afs/cern.ch/work/g/gfasanel/HighMass_200_NOCorr/test/dato
   extension=ptRatio_pt2Sum
   InitFile=init_RUN2_highMass_noCorr.txt
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*300
   BinWidth=0.02*6
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi
if [[ $1 = "RUN2_corr" ]]; then
   validation_file=miniAOD_November2015.dat
   region=scaleStep0_mediumMass
   commonCut=Et_35-noPF
   outDirData=/afs/cern.ch/work/g/gfasanel/HighMass/test/dato
   extension=ptRatio_pt2Sum
   #InitFile=init_RUN2.txt
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*300
   BinWidth=0.02*6
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "RUN2_NOCorr" ]]; then
   validation_file=miniAOD_November2015.dat
   region=scaleStep0_mediumMass
   commonCut=Et_35-noPF
   outDirData=/afs/cern.ch/work/g/gfasanel/HighMass_NOCorr/test/dato
   extension=ptRatio_pt2Sum
   #InitFile=init_RUN2_noCorr.txt
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*300
   BinWidth=0.02*6
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi
if [[ $1 = "ptRatio*pt2Sum" ]]; then
   validation_file=22Jan2012-runDepMCAll_checkMee.dat
   region=scaleStep0
   commonCut=Et_25-trigger-noPF
   outDirData=test/dato
   extension=ptRatio_pt2Sum
   InitFile=""
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*200
   BinWidth=0.05*2
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi
if [[ $1 = "ptRatio*pt2Sum*noCorr*RUN2" ]]; then
   validation_file=reference_25nsReco.dat
   region=scaleStep0
   commonCut=Et_25-noPF
   outDirData=test/dato
   extension=ptRatio_pt2Sum_withoutCorr_RUN2
   InitFile="init_RUN2.txt"
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*200
   BinWidth=0.05*2
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "ptRatio*pt2Sum*noCorr" ]]; then
   validation_file=22Jan2012-runDepMCAll_checkMee.dat
   region=scaleStep0
   commonCut=Et_25-trigger-noPF
   outDirData=test/dato
   extension=ptRatio_pt2Sum_withoutCorr
   InitFile=""
   Target=ptRatio*pt2Sum
   Min=0.5*0
   Max=2*200
   BinWidth=0.05*2
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "ptRatio" ]]; then
   validation_file=22Jan2012-runDepMCAll_checkMee.dat
   region=scaleStep0
   commonCut=Et_25-trigger-noPF
   outDirData=test/dato
   extension=ptRatio
   InitFile=""
   Target=ptRatio
   Min=0.5
   Max=2
   BinWidth=0.05
   Conf=leading
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "ptRatio_random" ]]; then
   validation_file=22Jan2012-runDepMCAll_checkMee.dat
   region=scaleStep0
   commonCut=Et_25-trigger-noPF
   outDirData=test/dato
   extension=ptRatio_random
   InitFile=""
   Target=ptRatio
   Min=0.5
   Max=2
   BinWidth=0.05
   Conf=random
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "ptSum" ]]; then
   validation_file=22Jan2012-runDepMCAll_checkMee.dat
   region=scaleStep0
   commonCut=Et_25-trigger-noPF
   outDirData=test/dato
   extension=ptSum
   InitFile=""
   Target=ptSum
   Min=0
   Max=200
   BinWidth=2
   Conf=leading
   echo ${validation_file}
   echo ${region}
   echo ${extension}
fi

if [[ $1 = "" ]]; then
    echo "[ERROR] You must specify the targetVariable"
    exit 0
fi

rm -r tmp/tmpFile-*.root

#common_cuts="Et_25_trigger_noPF" #for RUN1: notrigger for RUN2
common_cuts=Et_35_noPF

#./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile_${Target}_${Conf}_${region}_${commonCut}.root ${outDirData}/${extension}/*/fitres/outProfile_${Target}_${Conf}_${region}_${commonCut}.root
echo "Adding "${outDirData}/${extension}/*/fitres/outProfile_${extension}_${Conf}_${region}_${commonCut}.root
./script/haddTGraph.sh -o ${outDirData}/${extension}/fitres/outProfile_${Target}_${Conf}_${region}_${common_cuts}.root ${outDirData}/${extension}/*/fitres/outProfile_${extension}_${Conf}_${region}_${common_cuts}.root


###########################Make Likelihood plots###########################33                                                                                                  
echo "[STATUS] Making Likelihood plots"

echo "{" > tmp/fitProfiles_${Target}_${Conf}.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles_${Target}_${Conf}.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles_${Target}_${Conf}.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles_${Target}_${Conf}.C
echo "FitProfile2(\"${outDirData}/${extension}/fitres/outProfile_${Target}_${Conf}_${region}_${common_cuts}.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles_${Target}_${Conf}.C
echo "}" >> tmp/fitProfiles_${Target}_${Conf}.C
root -l -b -q tmp/fitProfiles_${Target}_${Conf}.C

echo "your plot is in ${outDirData}/${extension}/img/"

echo "[STATUS] Publishing plots"

#mv test/dato/${extension}/img/*.png ~/scratch1/www/Pt1Pt2/Likelihood/${extension}/

#CAMBIARE ==>Questo serve solo ad avere i plot carini nel punto di minimo della likelihood                                                                                                                             
#     ./bin/ZFitter.exe -f $outDirData/${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMas\
#s_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/${extension}/img/ --o\
#utDirFitResData=${outDirData}/${extension}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9Et --plotOnl\
#y >debug_plot_only.txt || exit 1 
