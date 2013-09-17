#!/bin/bash
###
###grep "DEBUG" test/dato/smearscan.log  | awk '{print $2,$3,$4,$7,$17}' | awk '{if($2!=old){print line; old=$2;line=$0}else{line=$0}}'| awk '($3>1){print $0}' | less

#regionsFile=scaleStep2smearing_7
queue=$1
nEventsPerToy=factorizedNew_${queue}
#nEventsPerToy=factorized
if [ -z "${queue}" ];then local=y; fi

for regionsFile in scaleStep2smearing_8 #scaleStep2smearing_5 scaleStep2smearing_6 
  do
dir=test/dato/fitres/Hgg_Et-toys/${regionsFile}
#alphaConst=C


for constAlpha in  0.01-0.00 0.000-0.07 0.007-0.05 #0.0005 0.02 0.005
  do
  const=`echo $constAlpha | cut -d '-' -f 1`
  alpha=`echo $constAlpha | cut -d '-' -f 2`
  #for  alpha in 0.00 # 0.05 #0.10 #0.15 0.00 #0.03 0.07
  #  do
    echo "[[[[[[[[[[[ const:alpha ]]]]]]]]]]] ${const}:${alpha}"
    baseDir=${dir}/${nEventsPerToy}/${const}-${alpha}/
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
scale_Et_58_65-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_58_65-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_55_75-absEta_0_1-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_75-absEta_1_1.4442-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_55-absEta_0_1-gold-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_43_50-absEta_0_1-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_40_50-absEta_1_1.4442-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_35_43-absEta_0_1-gold-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_40-absEta_1_1.4442-gold-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_35-absEta_0_1-gold-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_1-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
constTerm_absEta_0_1-gold-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.000 - 0.02) 
constTerm_absEta_0_1-bad-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.000 - 0.02) 
constTerm_absEta_1_1.4442-gold-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.000 - 0.02) 
constTerm_absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.000 - 0.02) 
alpha_absEta_0_1-gold-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.2) 
alpha_absEta_0_1-bad-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.2) 
alpha_absEta_1_1.4442-gold-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.2) 
alpha_absEta_1_1.4442-bad-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 ${alphaConst} L(0 - 0.2) 
EOF

    #sed '/smearEle/ d'  
    cat data/validation/toyMC-smearingMethod.dat > ${baseDir}/toyMC.dat #| sed "s|scaleStep2smearing_7|$regionsFile|" > $baseDir/toyMC.dat
#    echo "s1	smearEle_HggRunEtaR9	smearEle_HggRunEtaR9_s1-invertedToyMC-${const}-${alpha}.root" >> $baseDir/toyMC.dat
#    echo "s2	smearEle_HggRunEtaR9	smearEle_HggRunEtaR9_s2-invertedToyMC-${const}-${alpha}.root" >> $baseDir/toyMC.dat

    for nToys in `seq 2 200`; 
      do 
      newDir=${baseDir}/${alphaConst}/${nToys}
      mkdir -p $newDir 
      ls $newDir

      if [ -z "${local}" ];then
	  bsub -q ${queue} -oo ${newDir}/stdout.log -eo ${newDir}/stderr.log -J "$regionsFile-$const-$alpha" \
	      "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
      --regionsFile=data/regions/${regionsFile}.dat \
	--commonCut=Et_25-trigger-noPF-EB --smearerFit --outDirFitResData=$newDir \
        --constTermFix --alphaGoldFix --smearerType=profile --noPU \
	--initFile=${baseDir}/${alphaConst}/mcToy.txt --profileOnly --runToy > ${newDir}/log2.log"
      else
	  echo "#============================================================ Toy = $nToys"
        ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
 	   --regionsFile=data/regions/${regionsFile}.dat \
 	   --commonCut=Et_25-trigger-noPF-EB --smearerFit --outDirFitResData=$newDir \
 	   --constTermFix --smearerType=profile --noPU --alphaGoldFix \
 	   --initFile=${baseDir}/${alphaConst}/mcToy.txt --profileOnly --runToy  |tee ${newDir}/log2.log
      fi
    done
    wait 
  done
  wait
done

#done

exit 0
  
#!/bin/bash
#1000 100000 
dir=test/dato/fitres/Hgg_Et-toys
for nToys in `seq 4 5`; 
  do 
  echo "[[[[[[[[[[[ nToys ]]]]]]]]]]] $nToys"
  for nEventsPerToy in 0 10000 1000000
    do
    echo "[[[[[[[[[[[ nEventsPerToy ]]]]]]]]]]] ${nEventsPerToy}"
    for  alpha in 0.00 0.01 0.03 0.05 0.07
      do
      for const in 0.005 0.01 0.02 0.001
	do
	echo "[[[[[[[[[[[ const:alpha ]]]]]]]]]]] ${const}:${alpha}"
	newDir=${dir}/${nEventsPerToy}/${const}-${alpha}/${nToys}
	mkdir -p $newDir 
	cat > $newDir/mcToy.txt <<EOF
scale_Et_58_65-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
constTerm_absEta_0_0.8-bad-EB-Et_25 =  ${const} +/- 0.030000 L(0.0005 - 0.05) 
alpha_absEta_0_0.8-bad-EB-Et_25 =  ${alpha} +/- 0.010000 L(0 - 0.2) 
EOF
	( nice -19 ./bin/ZFitter.exe --smearerFit --autoNsmear --commonCut="EB-Et_25" -f data/validation/Winter13-powheg.dat --regionsFile=data/regions/test.dat --constTermFix --smearerType=profile --runToy --nToys=2 --initFile=${newDir}/mcToy.txt --eventsPerToy=${nEventsPerToy} --outDirFitResData=$newDir &> ${newDir}/log.log ) &
      done
      wait 
    done
    wait 
  done  
  wait 
done


#!/bin/bash
#1000 100000 
dir=test/dato/fitres/Hgg_Et-toys
for nToys in `seq 4 5`; 
  do 
  echo "[[[[[[[[[[[ nToys ]]]]]]]]]]] $nToys"
  for nEventsPerToy in 0 10000 1000000
    do
    echo "[[[[[[[[[[[ nEventsPerToy ]]]]]]]]]]] ${nEventsPerToy}"
    for  alpha in 0.00 0.01 0.03 0.05 0.07
      do
      for const in 0.005 0.01 0.02 0.001
	do
	echo "[[[[[[[[[[[ const:alpha ]]]]]]]]]]] ${const}:${alpha}"
	newDir=${dir}/${nEventsPerToy}/${const}-${alpha}/${nToys}
	mkdir -p $newDir 
	cat > $newDir/mcToy.txt <<EOF
scale_Et_58_65-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_0.8-bad-EB-Et_25 =  1.0000 +/- 0.0050000 L(0.95 - 1.05) // [GeV]
constTerm_absEta_0_0.8-bad-EB-Et_25 =  ${const} +/- 0.030000 L(0.0005 - 0.05) 
alpha_absEta_0_0.8-bad-EB-Et_25 =  ${alpha} +/- 0.010000 L(0 - 0.2) 
EOF
	( nice -19 ./bin/ZFitter.exe --smearerFit --autoNsmear --commonCut="EB-Et_25" -f data/validation/Winter13-powheg.dat --regionsFile=data/regions/test.dat --constTermFix --smearerType=profile --runToy --nToys=2 --initFile=${newDir}/mcToy.txt --eventsPerToy=${nEventsPerToy} --outDirFitResData=$newDir &> ${newDir}/log.log ) &
      done
      wait 
    done
    wait 
  done  
  wait 
done
