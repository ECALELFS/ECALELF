#!/bin/bash
###
###grep "DEBUG" test/dato/smearscan.log  | awk '{print $2,$3,$4,$7,$17}' | awk '{if($2!=old){print line; old=$2;line=$0}else{line=$0}}'| awk '($3>1){print $0}' | less

#regionsFile=scaleStep2smearing_7
queue=$1
nEventsPerToy=factorizedSherpaFixed_DataSeedFixed_smooth_autobin_smearingEt_${queue}
nSmearToy=30
commonCut=Et_20-trigger-noPF

#nEventsPerToy=factorized
if [ -z "${queue}" ];then local=y; fi

for regionsFile in scaleStep2smearing_1 #scaleStep2smearing_9 #scaleStep2smearing_1 scaleStep2smearing_2 #
  do
  dir=test/dato/fitres/toys/${regionsFile}
#alphaConst=C

for scale in 1.00 1.01 0.99 1.02 0.98 1.05 0.95 
do
for constAlpha in  0.02-0.00 0.04-0.00 0.01-0.00 0.03-0.00 0.005-0.00 0.035-0.25 0.007-0.05 0.01-0.00 0.000-0.07 # #  #0.0005 0.02 0.005
  do
  const=`echo $constAlpha | cut -d '-' -f 1`
  alpha=`echo $constAlpha | cut -d '-' -f 2`
  #for  alpha in 0.00 # 0.05 #0.10 #0.15 0.00 #0.03 0.07
  #  do
    echo "[[[[[[[[[[[ const:alpha ]]]]]]]]]]] ${const}:${alpha}"
    baseDir=${dir}/${nEventsPerToy}/${const}-${alpha}-${scale}/
    mkdir -p $baseDir 
    cat > ${baseDir}/smearEleFile.dat <<EOF
EB-absEta_0_1-bad       ${const}    ${alpha}
EB-absEta_0_1-gold      ${const}    ${alpha}
EB-absEta_1_1.4442-bad  ${const}    ${alpha}
EB-absEta_1_1.4442-gold ${const}    ${alpha}
EE-absEta_1.566_2-bad   ${const}	${alpha}
EE-absEta_1.566_2-gold  ${const}	${alpha}
EE-absEta_2_2.5-bad     ${const}	${alpha}
EE-absEta_2_2.5-gold    ${const}	${alpha}
EOF
#| #sed 's|^d|k|;s|^s|d|;s|^k|s|;/smearEle/ d;/smaerCat/ d' | sed "s|scaleStep2smearing_7|$regionsFile|" > 
    cat data/validation/toyMC-smearingMethod.dat > ${baseDir}/invertedToyMC.dat
#     if [ ! -e "smearEle_HggRunEtaR9_s1-invertedToyMC-${const}-${alpha}.root" ];then
# 	./bin/ZFitter.exe -f ${baseDir}/invertedToyMC.dat --regionsFile=data/regions/${regionsFile}.dat \
# 	    --commonCut=Et_25-trigger-noPF \
# 	    --autoNsmear --constTermFix --saveRootMacro --noPU \
# 	    --smearEleType=HggRunEtaR9 --smearEleFile=${baseDir}/smearEleFile.dat #> ${baseDir}/treeGen.log || exit 1
# 	mv tmp/smearEle_HggRunEtaR9_s1-invertedToyMC.root smearEle_HggRunEtaR9_s1-invertedToyMC-${const}-${alpha}.root || exit 1
# 	mv tmp/smearEle_HggRunEtaR9_s2-invertedToyMC.root smearEle_HggRunEtaR9_s2-invertedToyMC-${const}-${alpha}.root || exit 1
#     fi


#     if [ ! -e "smearerCat_stdPowheg-${regionsFile}.root" -o ! -e "smearerCat_stdMadgraph-${regionsFile}.root" ];then
# 	echo "[STATUS] Generating smearerCat for data and mc!"
# 	./bin/ZFitter.exe -f data/validation/toyMC2.dat --regionsFile=data/regions/${regionsFile}.dat \
# 	    --commonCut=Et_25-trigger-noPF-EB \
# 	    --autoNsmear --constTermFix --saveRootMacro --noPU \
# 	    --addBranch smearerCat > ${baseDir}/smearerCatGen.log || exit 1
# 	mv tmp/smearerCat_mc.root smearerCat_stdPowheg-${regionsFile}.root || exit 1
# 	mv tmp/smearerCat_data.root smearerCat_stdMadgraph-${regionsFile}.root || exit 1
#     fi


#     if [ ! -e "smearerCat-22Jan-stdMC_data-${regionsFile}.root" -o ! -e "smearerCat-22Jan-stdMC_mc-${regionsFile}.root" ];then
# 	echo "[STATUS] Generating smearerCat for data and mc!"
# 	./bin/ZFitter.exe -f data/validation/toyMC2.dat --regionsFile=data/regions/${regionsFile}.dat \
# 	    --commonCut=Et_25-trigger-noPF-EB \
# 	    --autoNsmear --constTermFix --saveRootMacro --noPU \
# 	    --addBranch smearerCat > ${baseDir}/smearerCatGen.log || exit 1
# 	mv tmp/smearerCat_mc.root smearerCat-22Jan-stdMC_mc-${regionsFile}.root || exit 1
# 	mv tmp/smearerCat_data.root smearerCat-22Jan-stdMC_data-${regionsFile}.root || exit 1
#     fi

    cat > $baseDir/${alphaConst}/mcToy.txt <<EOF
scale_absEta_1_1.4442-gold-${commonCut} 	=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_1_1.4442-bad-${commonCut} 	=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_0_1-gold-${commonCut} 		=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_0_1-bad-${commonCut} 		=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_1.566_2-gold-${commonCut} 	=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_1.566_2-bad-${commonCut} 	=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_2_2.5-gold-${commonCut} 	=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_absEta_2_2.5-bad-${commonCut} 		=  ${scale} +/- 0.0050000 L(0.9 - 1.1) // [GeV]
scale_Et_58_65-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_58_65-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_55_75-absEta_0_1-gold-${commonCut} =      ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_50_75-absEta_1_1.4442-gold-${commonCut} = ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_50_55-absEta_0_1-gold-${commonCut} =      ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_43_50-absEta_0_1-gold-${commonCut} =      ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_40_50-absEta_1_1.4442-gold-${commonCut} = ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_35_43-absEta_0_1-gold-${commonCut} =      ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_27_40-absEta_1_1.4442-gold-${commonCut} = ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_27_35-absEta_0_1-gold-${commonCut} =      ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_1-bad-${commonCut} =       ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_1_1.4442-bad-${commonCut} =  ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_20_33-absEta_1_1.4442-bad-${commonCut}  = ${scale} +/- 0.005  L(0.95 - 1.05) // [GeV]
scale_Et_20_33-absEta_0_1-bad-${commonCut} 		= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_20_35-absEta_0_1-gold-${commonCut} 		= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_20_40-absEta_1_1.4442-gold-${commonCut} 	= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_50_100-absEta_1_1.4442-gold-${commonCut} 	= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_55_100-absEta_0_1-gold-${commonCut} 		= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_58_100-absEta_0_1-bad-${commonCut} 		= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
scale_Et_58_100-absEta_1_1.4442-bad-${commonCut} 	= ${scale} +/- 0.005 L(0.95 - 1.05) // [GeV]
constTerm_absEta_0_1-gold-${commonCut} 		=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_0_1-bad-${commonCut} 		=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_1_1.4442-gold-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_1_1.4442-bad-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
alpha_absEta_0_1-gold-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_0_1-bad-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_1_1.4442-gold-${commonCut} 	=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_1_1.4442-bad-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
constTerm_absEta_1.566_2-gold-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_1.566_2-bad-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_2_2.5-gold-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
constTerm_absEta_2_2.5-bad-${commonCut} 	=  ${const} +/- 0.030000 L(0.000 - 0.10) 
alpha_absEta_1.566_2-gold-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_1.566_2-bad-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_2_2.5-gold-${commonCut} 	        =  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
alpha_absEta_2_2.5-bad-${commonCut} 		=  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.5) 
EOF

    #sed '/smearEle/ d'  
    cat data/validation/toyMC-smearingMethod.dat > ${baseDir}/toyMC.dat #| sed "s|scaleStep2smearing_7|$regionsFile|" > $baseDir/toyMC.dat
#    echo "s1	smearEle_HggRunEtaR9	smearEle_HggRunEtaR9_s1-invertedToyMC-${const}-${alpha}.root" >> $baseDir/toyMC.dat
#    echo "s2	smearEle_HggRunEtaR9	smearEle_HggRunEtaR9_s2-invertedToyMC-${const}-${alpha}.root" >> $baseDir/toyMC.dat

    for nToys in `seq 1 100`; 
      do 
      newDir=${baseDir}/${alphaConst}/${nSmearToy}/${nToys}/
      mkdir -p $newDir 
      ls $newDir

      if [ -z "${local}" ];then
	  bsub -q ${queue} -oo ${newDir}/stdout.log -eo ${newDir}/stderr.log -J "$regionsFile-$const-$alpha" \
	      "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
      --regionsFile=data/regions/${regionsFile}.dat \
	--commonCut=${commonCut} --smearerFit --outDirFitResData=$newDir \
        --constTermFix --alphaGoldFix --smearerType=profile --noPU --smearingEt \
	--initFile=${baseDir}/${alphaConst}/mcToy.txt --profileOnly --plotOnly --runToy --nSmearToy=${nSmearToy} \
        --autoBin        > ${newDir}/log2.log"
      else
	  echo "#============================================================ Toy = $nToys"
	  ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
	      --regionsFile=data/regions/${regionsFile}.dat \
	      --commonCut=${commonCut} --smearerFit --outDirFitResData=$newDir \
	      --constTermFix --smearerType=profile --noPU  --smearingEt \
	      --initFile=${baseDir}/${alphaConst}/mcToy.txt \
	      --plotOnly --profileOnly --runToy --eventsPerToy=0 --nSmearToy=${nSmearToy} --autoBin |tee ${newDir}/nSmearToy_${nSmearToy}.log
	  exit 0
      fi
    done
    wait 
  done
  wait
done

done

exit 0



for file in  test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_9/factorizedSherpaFixed_DataSeedFixed_smooth_cmscaf1nd/0.01-0.00/15/*/log2.log; do grep DUMP $file > `dirname $file`/dumpNLL.dat; done

for index in `seq 2 50`; do sed -i "s|\[DUMP NLL\]|$index\t |" test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_9/factorizedSherpaFixed_DataSeedFixed_smooth_cmscaf1nd/0.01-0.00/15/$index/dumpNLL.dat; done

cat test/dato/fitres/Hgg_Et-toys/scaleStep2smearing_9/factorizedSherpaFixed_DataSeedFixed_smooth_cmscaf1nd/0.01-0.00/15/*/dumpNLL.dat > dumpNLL.dat

cat  dumpNLL.dat  | awk -F '\t' '{cat[$2]+=$3;cat2[$2]+=$3*$3; n[$2]++;};END{for(i in cat){print i, n[i], cat[i]/n[i], cat2[i]/n[i]-cat[i]/n[i]*cat[i]/n[i]}}' | awk '($5 > 5){print $0}' 
