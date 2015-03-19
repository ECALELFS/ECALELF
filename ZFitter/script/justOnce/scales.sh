#!/bin/bash
#1000 100000 
#regionsFile=scaleStep2smearing_7
#nEventsPerToy=
toyMCFile=data/validation/toyMC-fitMethod.dat
queue=$1
#nEventsPerToy=factorizedSherpaFixed_DataSeedFixed_smooth_autobin_${queue}
nSmearToy=10
commonCut=Et_25-trigger-noPF
local=y

const=0
alpha=0
alphaConst=C

for regionsFile in scaleStep4smearing_0a #scaleStep2smearing_5 scaleStep2smearing_6 
  do
  dir=test/dato/fitres/fitMethod-toys/scales/${regionsFile}

  
  for scale in  1.00 0.99 1.01 0.98 1.02 0.95 1.05 0.90 1.10
    do
    deltaM=`echo ${scale}-1| bc -l | sed 's|^.|0.|'`
    echo "[[[[[[[[[[[ scale ]]]]]]]]]]] ${scale}"
    baseDir=${dir}/${nEventsPerToy}/${scale}
    mkdir -p $baseDir 
    cat > ${baseDir}/scaleEleFile.dat <<EOF
EB-absEta_0_1-bad        runNumber	0	9999999	${scale}	0
EB-absEta_0_1-gold       runNumber	0	9999999	${scale}	0
EB-absEta_1_1.4442-bad   runNumber	0	9999999	${scale}	0
EB-absEta_1_1.4442-gold  runNumber	0	9999999	${scale}	0
EE-absEta_1.566_2-bad    runNumber	0	9999999	${scale}	0
EE-absEta_1.566_2-gold   runNumber	0	9999999	${scale}	0
EE-absEta_2_2.5-bad      runNumber	0	9999999	${scale}	0
EE-absEta_2_2.5-gold     runNumber	0	9999999	${scale}	0
EOF
    #cat ${baseDir}/scaleEleFile.dat
    #continue
    #
#sed "s|scaleStep2smearing_7|$regionsFile|"
    cat ${toyMCFile} | sed 's|^d|k|;s|^s|d|;s|^k|s|;' | sed '/smearEle/ d;/smaerCat/ d'  > ${baseDir}/invertedToyMC.dat
    if [ ! -e "data/scaleEle/scaleEle_HggRunEtaR9_s2-invertedToyMC-${scale}.root" ];then
	./bin/ZFitter.exe -f ${baseDir}/invertedToyMC.dat --regionsFile=data/regions/${regionsFile}.dat \
	    --commonCut=${commonCut} \
	    --saveRootMacro --noPU \
	    --corrEleType=HggRunEtaR9 --corrEleFile=${baseDir}/scaleEleFile.dat || exit 1 #> ${baseDir}/treeGen.log || exit 1
	mv tmp/scaleEle_HggRunEtaR9_d1-invertedToyMC.root data/scaleEle/scaleEle_HggRunEtaR9_s1-invertedToyMC-${scale}.root || exit 1
	mv tmp/scaleEle_HggRunEtaR9_d2-invertedToyMC.root data/scaleEle/scaleEle_HggRunEtaR9_s2-invertedToyMC-${scale}.root || exit 1
	mv tmp/scaleEle_HggRunEtaR9_d3-invertedToyMC.root data/scaleEle/scaleEle_HggRunEtaR9_s3-invertedToyMC-${scale}.root || exit 1
    fi
    cat > $baseDir/mcToy.txt <<EOF
scale_Et_58_65-absEta_0_1-bad-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_58_65-absEta_1_1.4442-bad-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_55_75-absEta_0_1-gold-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_75-absEta_1_1.4442-gold-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_1-bad-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_1_1.4442-bad-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_55-absEta_0_1-gold-${commonCut} =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_1-bad-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_1_1.4442-bad-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_43_50-absEta_0_1-gold-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_40_50-absEta_1_1.4442-gold-${commonCut} =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_1-bad-${commonCut} =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_1_1.4442-bad-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_35_43-absEta_0_1-gold-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_1-bad-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_1_1.4442-bad-${commonCut} =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_40-absEta_1_1.4442-gold-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_35-absEta_0_1-gold-${commonCut} =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_1-bad-${commonCut} =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_1_1.4442-bad-${commonCut} =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
\Delta m = ${deltaM} +/- 0.0018953 L(-35 - 10) // [GeV/c^{2}]
\sigma_{CB} =  1.4527 +/- 0.0019719 L(0 - 10) // [GeV/c^{2}]
\alpha =  1.0011 C L(0.01 - 5) 
\gamma =  3.3003 C L(0.5 - 100) 
EOF
cat data/regions/$regionsFile.dat | sed "/#/ d;s|^|scale_|;s|$|-${commonCut} = 1.0 +/- 0.005 L(0.85 - 1.15)|" >> $baseDir/mcToy.txt 
cat data/regions/$regionsFile.dat | sed "/#/ d;s|^|constTerm_|;s|$|-${commonCut} = 0.0 +/- 0.005 C L(0.00 - 0.03)|" >> $baseDir/mcToy.txt 
cat data/regions/$regionsFile.dat | sed "/#/ d;s|^|alpha_|;s|$|-${commonCut} = 0.0 +/- 0.005 C L(0.00 - 0.2)|" >> $baseDir/mcToy.txt 

    sed '/scaleEle/ d'  ${toyMCFile} | sed "/^$/d; /^d[2-9]/ d;/^s1/ d;/^s3/ d" > $baseDir/toyMC.dat
    echo "s2	scaleEle_HggRunEtaR9	data/scaleEle/scaleEle_HggRunEtaR9_s2-invertedToyMC-${scale}.root" >> $baseDir/toyMC.dat

    for nToys in 250; 
      do 
      newDir=${baseDir}/${alphaConst}/${nToys}
      mkdir -p $newDir/{mc,dato}/{img,fitres}
      if [ -z "${local}" ];then
	  bsub -q ${queue} -oo ${newDir}/stdout.log -eo ${newDir}/stderr.log -J "$regionsFile-${scale}-$const-$alpha" \
	      "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
       --regionsFile=data/regions/${regionsFile}.dat \
    --commonCut=${commonCut} \
    --outDirFitResMC=$newDir/mc/fitres --outDirFitResData=$newDir/dato/fitres \
    --outDirImgData=$newDir/dato/img --outDirImgMC=$newDir/mc/img --noPU > ${newDir}/log2.log"
      else
	  echo "#============================================================ Toy = $nToys"
# 	  ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
# 	      --regionsFile=data/regions/${regionsFile}.dat \
# 	      --commonCut=${commonCut} \
# 	      --outDirFitResMC=$newDir/mc/fitres --outDirFitResData=$newDir/dato/fitres \
# 	      --outDirImgData=$newDir/dato/img --outDirImgMC=$newDir/mc/img --noPU |tee ${newDir}/log2.log
	  ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
	      --regionsFile=data/regions/${regionsFile}.dat \
	      --commonCut=${commonCut} \
	      --smearerFit --plotOnly --profileOnly --runToy \
	      --outDirFitResMC=$newDir/mc/fitres --outDirFitResData=$newDir/dato/fitres \
	      --initFile=$baseDir/mcToy.txt \
	      --outDirImgData=$newDir/dato/img --outDirImgMC=$newDir/mc/img --noPU |tee ${newDir}/log2.log
	  
      fi
      
# ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
#     --regionsFile=data/regions/${regionsFile}.dat \
#     --commonCut=Et_25-trigger-noPF \
#     --outDirFitResMC=$newDir/mc/fitres --outDirFitResData=$newDir/dato/fitres \
#     --outDirImgData=$newDir/dato/img --outDirImgMC=$newDir/mc/img --noPU
# #--initFile=${baseDir}/${alphaConst}/mcToy.txt 
    done
    wait 
  done
  wait
done



exit 0


for scale in 0.90  0.95  0.98  0.99  1.00  1.01  1.02  1.05  1.10; do ./script/makeTable.sh --regionsFile=data/regions/scaleStep4smearing_0a.dat --outDirFitResData=test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/${scale}/1/dato/fitres/ --outDirFitResMC=test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/${scale}/1/mc/fitres/ --commonCut=Et_25-trigger-noPF | sed "s|^\([!#]\)|ADC_${scale}-\1|" > test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/${scale}/1/table.tex; done

cat test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/*/1/table.tex > test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/table.tex
 ./script/stability.sh -t test/dato/fitres/fitMethod-toys/scales/scaleStep4smearing_0a/table.tex  --column=6 -x ADC -y peak --outDirImgData=tmp/ --allRegions
