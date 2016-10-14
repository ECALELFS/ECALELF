#!/bin/bash
source script/functions.sh
if [[ $1 = "promptReco_Pho_regrCorr" ]]; then
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


if [[ $1 = "" ]]; then
    echo "[ERROR] You must specify the targetVariable"
    exit 0
fi

#Be careful with onlyScale: you must use a NON ZERO SIGMA!

# you should run this script using screen 
# it creates a single job of 50 subjobs (array of jobs)
# Directory organization: 

echo ${outDirData}

 if [ ! -e "${outDirData}/${extension}/fitres" ];then mkdir ${outDirData}/${extension}/fitres -p; fi
 if [ ! -e "${outDirData}/${extension}/img" ];then mkdir ${outDirData}/${extension}/img -p; fi

############# SUBMITTING JOBS ###########################################

 for index in `seq 1 50` #Making directories
 do
     mkdir ${outDirData}/${extension}/${index}/fitres/ -p
     mkdir ${outDirData}/${extension}/${index}/img -p
 done
 
# echo ./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_regrCorrSemiParV5_ele --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --corrEleType=HggRunEtaR9Et --smearEleType=stochastic --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ ${InitFile}

if [[ $1 = "RUN2_200_corr" ]]; then 
#[[ $1 = "RUN2_corr" ]] 
    bsub -q 2nd\
            -oo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stderr.log\
            -J "${region} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;

./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --initFile=${InitFile}  --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ --corrEleType=EtaR9 --smearEleType=stochastic || exit 1";

echo "./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --initFile=${InitFile}  --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ --corrEleType=EtaR9 --smearEleType=stochastic";

elif [[ $1 = "RUN2_200_NOCorr" ]]; then 
# [[ $1 = "RUN2_NOCorr" ]] ||     
     bsub -q 2nd\
            -oo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stderr.log\
            -J "${region} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;

./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --initFile=${InitFile} --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ || exit 1";

echo "./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --initFile=${InitFile} --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/";

elif [[ $1 = "RUN2_corr" ]]; then 
#[[ $1 = "RUN2_corr" ]] 
    bsub -q 1nh\
            -oo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stderr.log\
            -J "${region} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;

./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr  --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ --corrEleType=EtaR9 --smearEleType=stochastic || exit 1";

echo "./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr  --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ --corrEleType=EtaR9 --smearEleType=stochastic";

elif [[ $1 = "RUN2_NOCorr" ]]; then 
# [[ $1 = "RUN2_NOCorr" ]] ||     
     bsub -q 1nh\
            -oo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stdout.log\
            -eo ${outDirData}/${extension}/%I/fitres/${Target}-${region}-stderr.log\
            -J "${region} ${extension}[1-50]"\
            "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a; echo \$CMSSW_VERSION;

./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/ || exit 1";

echo "./bin/ZFitter.exe -f data/validation/${validation_file} --regionsFile=data/regions/${region}.dat --invMass_var=invMass_SC_corr --commonCut=${commonCut} --autoBin --smearerFit --targetVariable=${Target} --targetVariable_min=${Min} --targetVariable_max=${Max} --targetVariable_binWidth=${BinWidth} --configuration=${Conf} --outDirImgData=${outDirData}/${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/${extension}/\$LSB_JOBINDEX/fitres/";

fi

exit 0
#Now, wait the end of your jobs
	
while [ "`bjobs -J \"${region} ${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

echo "job completed!"

##copiarci Likelihood fitter qui di seguito


    