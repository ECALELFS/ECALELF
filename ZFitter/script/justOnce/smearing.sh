#!/bin/bash
#1000 100000 
dir=test/dato/fitres/Hgg_Et-toys
for const in 0.01 0.02 0.005
  do
  for  alpha in 0.00 #0.01 0.03 0.05 0.07
    do
    echo "[[[[[[[[[[[ const:alpha ]]]]]]]]]]] ${const}:${alpha}"
    baseDir=${dir}/${nEventsPerToy}/${const}-${alpha}/
    mkdir -p $baseDir 
    cat > ${baseDir}/smearEleFile.dat <<EOF
EB-absEta_0_1-bad       ${const}    ${alpha}
EB-absEta_0_1-gold      ${const}    ${alpha}
EB-absEta_1_1.4442-bad  ${const}    ${alpha}
EB-absEta_1_1.4442-gold ${const}    ${alpha}
EE-absEta_1.566_2-bad   0       0
EE-absEta_1.566_2-gold  0       0
EE-absEta_2_2.5-bad     0       0
EE-absEta_2_2.5-gold    0       0
EOF
    cat data/validation/toyMC.dat | sed 's|^d|k|;s|^s|d|;s|^k|s|;/smearEle/ d' > ${baseDir}/invertedToyMC.dat
    if [ ! -e "smearEle_HggRunEtaR9_mc-${const}-${alpha}.root" ];then
	./bin/ZFitter.exe -f ${baseDir}/invertedToyMC.dat --regionsFile=data/regions/scaleStep2smearing_7.dat \
	    --commonCut=Et_25-trigger-noPF-EB \
	    --autoNsmear --constTermFix --saveRootMacro --noPU \
	    --smearEleType=HggRunEtaR9 --smearEleFile=${baseDir}/smearEleFile.dat > ${baseDir}/treeGen.log
	mv tmp/smearEle_HggRunEtaR9_mc.root smearEle_HggRunEtaR9_mc-${const}-${alpha}.root
    fi
    cat > $baseDir/mcToy.txt <<EOF
scale_Et_58_65-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_58_65-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_55_75-absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_75-absEta_0.8_1.4442-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_58-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_50_55-absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_45_50-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_43_50-absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_40_50-absEta_0.8_1.4442-gold-Et_25-trigger-noPF-EB =  1.0000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_39_45-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_35_43-absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_33_39-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_40-absEta_0.8_1.4442-gold-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_35-absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  1.000 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
scale_Et_27_33-absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  1.0 +/- 0.0050000 C L(0.95 - 1.05) // [GeV]
constTerm_absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.0005 - 0.02) 
constTerm_absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.0005 - 0.02) 
constTerm_absEta_0.8_1.4442-gold-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.0005 - 0.02) 
constTerm_absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  ${const} +/- 0.030000 L(0.0005 - 0.02) 
alpha_absEta_0_0.8-gold-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 L(0 - 0.2) 
alpha_absEta_0_0.8-bad-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 L(0 - 0.2) 
alpha_absEta_0.8_1.4442-gold-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 L(0 - 0.2) 
alpha_absEta_0.8_1.4442-bad-Et_25-trigger-noPF-EB =  ${alpha} +/- 0.010000 L(0 - 0.2) 
EOF
    sed '/smearEle/ d'  data/validation/toyMC.dat > $baseDir/toyMC.dat
    echo "d	smearEle_HggRunEtaR9	smearEle_HggRunEtaR9_mc-${const}-${alpha}.root" >> $baseDir/toyMC.dat
    continue
    for nToys in `seq 1 200`; 
      do 
      newDir=${baseDir}/${nToys}
      mkdir -p $newDir 
      
      bsub -q 2nd \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch5/src/calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f ${baseDir}/toyMC.dat \
      --regionsFile=data/regions/scaleStep2smearing_7.dat \
	--commonCut=Et_25-trigger-noPF-EB --smearerFit --outDirFitResData=$newDir \
	--autoNsmear --constTermFix --smearerType=profile --noPU \
	--initFile=${baseDir}/mcToy.txt --profileOnly > ${newDir}/log2.log"
    done
    wait 
  done
  wait
done

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
