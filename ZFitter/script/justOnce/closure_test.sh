#!/bin/bash
# usage ./script/justOnce/closure_test.sh 2nd (1nh)
# usage ./script/justOnce/closure_test.sh (if you want to do in local)
# inspired from ZFitter/script/justOnce/smearing.sh
queue=$1
nSmearToy=1 #it quantifies the over-sampling
commonCut=Et_25-trigger-noPF #used in the category names

if [ -z "${queue}" ];then local=y; fi

regionsFile=scaleStep0

regionDir=test/dato/fitres/toys/${regionsFile}

barrel(){
## scale and sigma changed for Barrel (for EE they are 1)
for scale in  0.98  0.985 0.99 0.995 1.00 1.005 1.01 1.015 1.02
do
    #for constAlpha in  0.00-0.00 0.005-0.00 0.01-0.00 0.015-0.00 0.02-0.00
    for constAlpha in 0.015-0.00
    do
	const=`echo $constAlpha | cut -d '-' -f 1`
	alpha=`echo $constAlpha | cut -d '-' -f 2`
	
	echo "*********const:alpha:scale ${const}:${alpha}:${scale}"
	workDir=${regionDir}/${const}_${alpha}_${scale}/EE/
	mkdir -p ${workDir} 

	cat > ${workDir}init_mcToy.txt <<EOF
scale_EE-invMass_100_2000-Et_25-trigger-noPF =  ${scale} +/- 0.0050000 L(0.96 - 1.04) // [GeV]
#scale_EB-invMass_100_2000-Et_25-trigger-noPF =  ${scale} +/- 0.0050000 L(0.96 - 1.04) // [GeV]
constTerm_EE-invMass_100_2000-Et_25-trigger-noPF =  ${const} +/- 0.030000 L(0 - 0.05)
#constTerm_EB-invMass_100_2000-Et_25-trigger-noPF =  ${const} +/- 0.030000 L(0 - 0.05)
EOF
	cat data/validation/closure.dat > ${regionDir}/toyMC_closure.dat

	for nToys in `seq 1 10`;
	do 
	    toyDir=${workDir}${nToys}/
	    mkdir -p $toyDir 
	    ls $toyDir
	    
	    if [ -z "${local}" ];then
		echo "#============================================================ submitting closure tests job: Toy = $nToys"
		bsub -q ${queue} -oo ${toyDir}/stdout.log -eo ${toyDir}/stderr.log -J "$regionsFile-$const-$alpha-$scale" \
		    "cd /afs/cern.ch/user/g/gfasanel/new_version_ECALELF/CMSSW_7_5_0_pre4/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log" # --plotOnly --profileOnly
	    else
		echo "#============================================================ Closure test in local: Toy = $nToys"
		echo "
		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log" # --plotOnly --profileOnly
		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log

	    fi
	done #toy for
	wait
    done #constAlpha for
    wait
done #scale for
}

endcap(){
##Same thing for EE, with EB fixed to "1"
for scale in  0.98  0.985 0.99 0.995 1.00 1.005 1.01 1.015 1.02
do
    #for constAlpha in  0.00-0.00 0.005-0.00 0.01-0.00 0.015-0.00 0.02-0.00
    for constAlpha in 0.015-0.00
    do
	const=`echo $constAlpha | cut -d '-' -f 1`
	alpha=`echo $constAlpha | cut -d '-' -f 2`
	
	echo "*********const:alpha:scale ${const}:${alpha}:${scale}"
	workDir=${regionDir}/${const}_${alpha}_${scale}/EB/
	mkdir -p ${workDir} 

	cat > ${workDir}init_mcToy.txt <<EOF
#scale_EE-invMass_100_2000-Et_25-trigger-noPF =  ${scale} +/- 0.0050000 L(0.96 - 1.04) // [GeV]
scale_EB-invMass_100_2000-Et_25-trigger-noPF =  ${scale} +/- 0.0050000 L(0.96 - 1.04) // [GeV]
#constTerm_EE-invMass_100_2000-Et_25-trigger-noPF =  ${const} +/- 0.030000 L(0 - 0.05)
constTerm_EB-invMass_100_2000-Et_25-trigger-noPF =  ${const} +/- 0.030000 L(0 - 0.05)
EOF
	cat data/validation/closure.dat > ${regionDir}/toyMC_closure.dat

	for nToys in `seq 1 10`;
	do 
	    toyDir=${workDir}${nToys}/
	    mkdir -p $toyDir 
	    ls $toyDir
	    
	    if [ -z "${local}" ];then
		echo "#============================================================ submitting closure tests job: Toy = $nToys"
		bsub -q ${queue} -oo ${toyDir}/stdout.log -eo ${toyDir}/stderr.log -J "$regionsFile-$const-$alpha-$scale" \
		    "cd /afs/cern.ch/user/g/gfasanel/new_version_ECALELF/CMSSW_7_5_0_pre4/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log" # --plotOnly --profileOnly
	    else
		echo "#============================================================ Closure test in local: Toy = $nToys"
		echo "
		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log" # --plotOnly --profileOnly
		./bin/ZFitter.exe --invMass_var=invMass_SC_regrCorrSemiParV5_ele -f data/validation/closure.dat --regionsFile=data/regions/scaleStep0.dat --smearerFit --outDirFitResData=${toyDir} --noPU --runToy --eventsPerToy=0 --nSmearToy=5 --targetVariable=ptRatio*pt2Sum --targetVariable_min=0.5*0 --targetVariable_max=2*200 --targetVariable_binWidth=0.05*2 --configuration=random --initFile=${workDir}init_mcToy.txt --autoBin > ${toyDir}debug.log

	    fi
	done #toy for
	wait
    done #constAlpha for
    wait
done #scale for
}

barrel
#endcap
