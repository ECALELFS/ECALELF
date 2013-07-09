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
