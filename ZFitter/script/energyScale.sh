#!/bin/bash
source script/functions.sh
# 
# energy scale derived in different steps:
# - time dependence (step1)
# - material dependence  (step2)
# - closure test (step3)

index=
commonCut=Et_20-trigger-noPF
selection=loose
invMass_var=invMass_SC_regrCorrSemiParV5_pho
baseDir=test
updateOnly="--updateOnly" # --profileOnly --initFile=init.txt"
#updateOnly=""
###########################################################
regionFileStep1=data/regions/scaleStep1.dat

regionFileStep2EB=data/regions/scaleStep2smearing_1.dat
regionFileStep2EE=data/regions/scaleStep2smearing_2.dat

regionFileStep4EB=data/regions/scaleStep4smearing_1.dat
regionFileStep4EE=data/regions/scaleStep4smearing_2.dat

regionFileStep5EB=data/regions/scaleStep2smearing_9.dat
regionFileStep5EE=data/regions/scaleStep2smearing_10.dat
outFileStep1=step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat
outFileStep2=step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
#############################################################
usage(){
    echo "`basename $0` [options]" 
    echo " -f arg            config file"
#    echo " --regionFile arg             "
    echo " --runRangesFile arg           "
    echo " --selection arg (=$selection)"
    echo " --invMass_var arg"
    echo " --commonCut arg (=$commonCut)"
    echo " --step arg: 1, 2, 2fit, 3, 3weight, slide, 3stability, syst, 1-2,1-3,1-3stability and all ordered combination"
    echo " --index arg"
}
#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,regionFile:,runRangesFile:,selection:,invMass_var:,step:,baseDir:,commonCut:,index: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-f) configFile=$2; shift;;
	-s|--step) STEP=$2; shift;;
	--invMass_var) invMass_var=$2; echo "[OPTION] invMass_var = ${invMass_var}"; shift;;
	--index) index=$2; shift;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;; 
	--selection) selection=$2; echo "[OPTION] selection = ${selection}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--commonCut) commonCut=$2; echo "[OPTION] commonCut = $commonCut"; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

    if [ ! -d "tmp" ];then mkdir tmp; fi

# file with ntuples
if [ -z "${configFile}" ];then
    echo "[ERROR] configFile not specified" >> /dev/stderr
    exit 1
fi

if [ -z "${runRangesFile}" ];then
    echo "[ERROR] runRangesFile not specified" >> /dev/stderr
    exit 1
fi

case ${selection} in
    WP80PU)
        ;;
    WP90PU)
	;;
    loose)
	;;
    medium)
	;;
    tight)
	;;
    *)
	echo "[ERROR] Selection ${selection} not configured" >> /dev/stderr
        exit 1
        ;;
esac



case ${STEP} in
    help) 
	echo "List of steps:"
	echo "   - 1: stability vs time (runNumber x eta)"
	echo "   - 2: eta x R9 (smearing method)"
#	echo "   - 2fit: eta x R9 (fit method)"
	echo "   - 3: closure test of 2: eta x R9 (fit method)"
	echo "   - 4: closure test of 2: eta x R9 (smearing method), for final histograms and profiles"
	echo "   - 5: plot and profile only in Et categories with smearings from step4"
	echo "   - 6: smearings from step4, scales in Et x eta x R9 categories"
	echo "   - 7: eta x R9 x Et, scale and smearings"
	;;
    1)	STEP1=y;;
    2) 	STEP2=y;;
    3) 	STEP3=y;; #SLIDE=y;;
    4)  STEP4=y;;
    4weight) STEP4=y; extension=weight;;
    4medium) STEP4=y; extension=medium;;
    4tight)  STEP4=y; extension=tight;;
    5)  STEP5=y;;
    6)  STEP6=y;;
    8)  STEP8=y;;
    9)  STEP9=y;;
    10)  STEP10=y;;
    7)  STEP7=y;;
    1-2) STEP1=y; STEP2=y;;
    1-3) STEP1=y; STEP2=y; STEP3=y; SLIDE=y;;
    2-3) STEP2=y; STEP3=y; SLIDE=y;;
    1-3stability) STEP1=y; STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    2-3stability) STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    slide) SLIDE=y;;
    3stability) STEP3Stability=y;;
    1stability) STEP1Stability=y;;
	syst) SYSTEMATICS=y;;
    3weight) STEP3WEIGHT=y; STEP3=y; extension=weight;;
    2fit) STEP2FIT=y;;
    1-2fit) STEP1=y; STEP2FIT=y;;
    all) STEP1stability=y; STEP1=y; STEP2FIT=y; STEP3=y; STEP3Stability=y; STEP4=y; SLIDE=y;;
    *)
	exit 1
	;;
esac


## pileup reweight name
puName ${configFile}
echo "PUName: $puName"

mcName ${configFile}
echo "mcName: ${mcName}"

if [ "${invMass_var}" == "invMass_regrCorr_egamma" ];then
    outDirMC=$baseDir/MCodd/${mcName}/${puName}/${selection}/${invMass_var}
    isOdd="--isOddMC"
else
    outDirMC=$baseDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
fi
outDirData=$baseDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
outDirTable=${outDirData}/table

outDirTable=${outDirData}/table

#if [ ! -e "${outDirTable}/${selection}/${invMass_var}" ];then mkdir -p ${outDirTable}/${selection}/${invMass_var}; fi

if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi

if [ "${extension}" == "weight" ];then
    tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
    for tag in $tags
      do
      if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c r9Weight`" == "0" ];then
	  echo "[STATUS] Adding r9weight trees to $tag"
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile data/regions/scaleStep2smearing_1.dat  \
	      --saveRootMacro --r9WeightFile=data/r9weights/R9weights_CMSSW53X_RD1.root \
	      --smearerFit
	  mv tmp/r9Weight_${tag}-`basename $configFile .dat`.root data/r9weights/ || exit 1
	  echo -e "$tag\tr9Weight\tdata/r9weights/r9Weight_${tag}-`basename $configFile .dat`.root" >> $configFile
      fi
    done
fi

if [ -n "${STEP1}" ];then
    # runNumber x eta with fitMethod
    regionFile=data/regions/scaleStep1.dat
    outFile=step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat
    if [ ! -e "${outDirTable}/${outFile}" ];then
	
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
	if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
	if [ ! -e "${outDirData}/step1" ];then mkdir ${outDirData}/step1 -p; fi

	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile}  \
	    $isOdd $updateOnly --selection=${selection}  --invMass_var ${invMass_var} --commonCut $commonCut \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img \
	    > ${outDirData}/log/`basename ${outFile} .dat`.log || exit 1
	
	./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    >  ${outDirTable}/`basename ${outFile} .dat`.tex
	
	./script/tex2txt.sh ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.tex | awk -F "\t" -f awk/recalibOutput.awk |grep -v '^%' > ${outFile}

	#save root files with step1 corrections
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	    --saveRootMacro --corrEleType HggRunEta \
	    --corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat || exit 1
	
	mv tmp/scaleEle_HggRunEta_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step1/    
	echo "[STATUS] Step 1 done"
    else 
	echo "[STATUS] Step 1 already done" 
    fi
fi



if [ -n "${STEP2FIT}" ];then
    #eta x R9 with fit method
    regionFile=data/regions/scaleStep2.dat
    outFile=step2fit-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFile}" ];then
   
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/step2/fitres" ];then mkdir ${outDirData}/step2/fitres -p; fi
	if [ ! -e "${outDirData}/step2/img" ];then mkdir ${outDirData}/step2/img -p; fi

	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEta\t${outDirData}/step1/scaleEle_HggRunEta_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step2/`basename $configFile`
	
	./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile`  --regionsFile ${regionFile}   \
	    $isOdd  $updateOnly --selection=${selection} --invMass_var ${invMass_var} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
	    --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img \
	    > ${outDirData}/log/`basename $outFile .dat`.log || exit 1
	
	./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
	    > ${outDirTable}/`basename $outFile .dat`.tex || exit 1
	
	cut -d '&' -f 1-5   ${outDirTable}/${outFile} |  sed "s|-$commonCut||" > tmp/s.tex
	./script/CalcResCorrection.sh tmp/s.tex > tmp/res_corr.dat

	./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat \
	    --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat \
	    | grep -v '#' > ${outDirTable}/${outFile}
#    ./script/makeCorrVsRunNumber.py -l -c --file2=res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |sed -f sed/tex.sed > ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex

	cat ${outDirTable}/${outFile} | cut -f 1,3-6 | awk '{printf("%s\t%s - %s\t%s\t%s \n", $1,$2,$3, $4, $5)}' | sed -r 's|\t|\t\&\t|g;' | sed -f sed/tex.sed | sed 's|R9|\\RNINE|' > ${outDirTable}/correctionNote-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	
    fi
fi



if [ -n "${STEP2}" ];then
    #eta x R9 with smearing method
    regionFileEB=${regionFileStep2EB}
    regionFileEE=${regionFileStep2EE}
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    regionFile=$regionFileEB
    outFile=step2${extension}-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFileStep1}" ];then
	echo "[ERROR] Impossible to run step2 without step1" >> /dev/stderr
	#exit 1
    fi
    
    if [ ! -e "${outDirTable}/${outFile}" ];then
	
	if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
	if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
	if [ ! -e "${outDirData}/step2${extension}/fitres" ];then mkdir ${outDirData}/step2${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step2${extension}/img" ];then mkdir ${outDirData}/step2${extension}/img -p; fi
	
	if [ -e "${outDirTable}/params-step2-${commonCut}.txt" ];then 
	    initFile="--initFile=${outDirTable}/params-step2-${commonCut}.txt"; 
	else 
	    initFile=""
	fi
	
	tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
	for tag in $tags
	  do
	  if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEB}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_${basenameEB}_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_${basenameEB}\tdata/smearerCat/smearerCat_${basenameEB}_${tag}-`basename $configFile .dat`.root" >> $configFile

	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEE}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_${basenameEE}_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_${basenameEE}\tdata/smearerCat/smearerCat_${basenameEE}_${tag}-`basename $configFile .dat`.root" >> $configFile

	  fi
	done
	
	# save the corrections in root files
	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEta\t${outDirData}/step1/scaleEle_HggRunEta_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step2/`basename $configFile`
	
# 	for index in `seq 1 50`
# 	  do
# 	  mkdir ${outDirData}/step2/${index}/fitres/ -p 
# 	  mkdir ${outDirData}/step2/${index}/img -p 
# 	  bsub -q 2nd \
# 	      -oo ${outDirData}/step2/${index}/fitres/`basename ${outFile} .dat`-stdout.log \
# 	      -eo ${outDirData}/step2/${index}/fitres/`basename ${outFile} .dat`-stderr.log \
# 	      -J "${basenameEB} step2" \
# 	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
# ./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/${index}/img/ --outDirFitResData=${outDirData}/step2/${index}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEta || exit 1; touch ${outDirData}/step2/${index}/${basenameEB}-done"
# 	done

      # 	for index in `seq 1 50`
       # 	  do
bsub -q 2nd \
    -oo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
    -eo ${outDirData}/step2/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
    -J "${basenameEE} step2[1-50]" \
    "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/fitres/ -p; mkdir ${outDirData}/step2/\$LSB_JOBINDEX/img -p; ./bin/ZFitter.exe -f $outDirData/step2/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} --selection=${selection} --outDirFitResMC=${outDirMC}/fitres --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step2/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEta || exit 1; touch ${outDirData}/step2/\$LSB_JOBINDEX/${basenameEE}-done"

    
while [ "`bjobs -J \"${basenameEB} step2\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 1m; done
while [ "`bjobs -J \"${basenameEE} step2\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 1m; done

	./script/haddTGraph.sh -o ${outDirData}/step2/fitres/outProfile-scaleStep2smearing_1-${commonCut}.root ${outDirData}/step2/*/fitres/outProfile-scaleStep2smearing_1-${commonCut}.root
	./script/haddTGraph.sh -o ${outDirData}/step2/fitres/outProfile-scaleStep2smearing_2-${commonCut}.root ${outDirData}/step2/*/fitres/outProfile-scaleStep2smearing_2-${commonCut}.root

    
	######################################################33
	echo "{" > tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
	echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C.C+\");" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step2/fitres/outProfile-scaleStep2smearing_1-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "FitProfile2(\"${outDirData}/step2/fitres/outProfile-scaleStep2smearing_2-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
	echo "}" >> tmp/fitProfiles.C
	root -l -b -q tmp/fitProfiles.C

	grep scale ${outDirData}/step2/img/outProfile-${basenameEB}-${commonCut}-FitResult.config | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' > tmp/res_corr.dat
	
	grep scale ${outDirData}/step2/img/outProfile-${basenameEE}-${commonCut}-FitResult.config | grep -v absEta_0_1 | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' >> tmp/res_corr.dat
#
	sed -i 's|^EB-||;s|^EE-||' ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat
	./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |grep -v '#' > ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
	./script/makeCorrVsRunNumber.py -l -c --file2=tmp/res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |sed -f sed/tex.sed > ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	cp ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	cp ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
	
#      cat ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat | cut -f 1,3-6 | awk '{printf("%s\t%s - %s\t%s\t%s \n", $1,$2,$3, $4, $5)}' | sed -r 's|\t|\t\&\t|g;' | sed -f sed/tex.sed | sed 's|R9|\\RNINE|' > ${outDirTable}/correctionNote-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	
	#save root files with step1 corrections
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	    --saveRootMacro --corrEleType HggRunEtaR9 \
	    --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat || exit 1
	
	mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/    
    fi
	#save root files with step1 corrections
	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	    --saveRootMacro --corrEleType HggRunEtaR9 \
	    --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat || exit 1
	
	mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/    
   
fi



if [ -n "${STEP3}" ];then
    regionFile=data/regions/scaleStep3.dat
    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step3/fitres" ];then mkdir ${outDirData}/step3/fitres -p; fi
    if [ ! -e "${outDirData}/step3/img" ];then mkdir ${outDirData}/step3/img -p; fi


    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step3/img \
	--corrEleType HggRunEtaR9 --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat > ${outDirData}/log/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    
    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1

fi


if [ -n "${STEP4}" ];then
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi

    #eta x R9 with smearing method
    regionFileEB=${regionFileStep4EB}
    regionFileEE=data/regions/scaleStep4smearing_2.dat
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    regionFile=$regionFileEB
    outFile=$outDirTable/step4${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
	echo "[ERROR] Impossible to run step4 without step2" >> /dev/stderr
	#exit 1
    fi

    if [ -e "${outDirTable}/params-step4-${commonCut}.txt" ];then 
	initFile="--initFile=${outDirTable}/params-step4-${commonCut}.txt"; 
    else 
	initFile=""
    fi

    if [ ! -e "${outFile}" ];then
	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
	if [ ! -e "${outDirData}/step4${extension}/fitres" ];then mkdir ${outDirData}/step4${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step4${extension}/img" ];then    mkdir ${outDirData}/step4${extension}/img -p; fi
	
	
	if [ "${extension}" == "weight" ];then
	    updateOnly="$updateOnly --useR9weight"
	fi
	tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
	for tag in $tags
	  do
	  if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEB}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEB .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEE}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEE .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	  fi
	done
	
	# save the corrections in root files
	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step4${extension}/`basename $configFile`

	for index in `seq 1 50`
	  do
	  mkdir ${outDirData}/step4${extension}/${index}/fitres/ -p 
	  mkdir ${outDirData}/step4${extension}/${index}/img -p 
	done

	bsub -q 2nd \
	    -oo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
	    -eo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
	    -J "${basenameEB} step4${extension}[1-50]" \
	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step4${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"

	bsub -q 2nd \
	    -oo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
	    -eo ${outDirData}/step4${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
	    -J "${basenameEE} step4${extension}[1-50]" \
	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step4${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step4${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"

#    fi
    while [ "`bjobs -J \"${basenameEB} step4${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
    while [ "`bjobs -J \"${basenameEE} step4${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

    ./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
    ./script/haddTGraph.sh -o ${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step4${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root

    
	######################################################33
    echo "{" > tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step4${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "}" >> tmp/fitProfiles.C
    root -l -b -q tmp/fitProfiles.C
    fi
    
    cat ${outDirData}/step4${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult.config > ${outFile}
    grep -v absEta_0_1 ${outDirData}/step4${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult.config >> ${outFile}

    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}

    ./bin/ZFitter.exe -f $outDirData/step4${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step4${extension}/img/ --outDirFitResData=${outDirData}/step4${extension}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  || exit 1

#     ./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  || exit 1

fi


if [ -n "${STEP3WEIGHT}" ];then
    regionFile=data/regions/scaleStep3.dat
    if [ ! -e "${outDirMC}/r9weight/fitres" ];then mkdir ${outDirMC}/r9weight/fitres -p; fi
    if [ ! -e "${outDirMC}/r9weight/img" ];then mkdir ${outDirMC}/r9weight/img -p; fi
    if [ ! -e "${outDirData}/step3_r9weight/fitres" ];then mkdir ${outDirData}/step3_r9weight/fitres -p; fi
    if [ ! -e "${outDirData}/step3_r9weight/img" ];then mkdir ${outDirData}/step3_r9weight/img -p; fi


    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/r9weight/fitres --outDirFitResData=${outDirData}/step3_r9weight/fitres \
 	--outDirImgMC=${outDirMC}/r9weight/img --outDirImgData=${outDirData}/step3_r9weight/img \
	--corrEleType HggRunEtaR9 --r9WeightFile=R9Weights.root \
	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
	> ${outDirData}/log/step3_r9weight-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    
    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3_r9weight/fitres > ${outDirTable}/step3_r9weight-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1

fi


if [ -n "${STEP5}" ];then
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi

    #eta x R9 with smearing method
    regionFileEB=${regionFileStep5EB}
    regionFileEE=${regionFileStep5EE}
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    outFile=step5${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
	echo "[ERROR] Impossible to run step4 without step2" >> /dev/stderr
	#exit 1
    fi

    if [ ! -e "${outDirTable}/${outFile}" ];then
	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
	if [ ! -e "${outDirData}/step5${extension}/fitres" ];then mkdir ${outDirData}/step5${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step5${extension}/img" ];then    mkdir ${outDirData}/step5${extension}/img -p; fi
	
	if [ -e "${outDirTable}/params-step5-${commonCut}.txt" ];then 
	    initFile="--initFile=${outDirTable}/params-step5-${commonCut}.txt"; 
	else 
	    echo "[WARNING] init file ${outDirTable}/params-step5-${commonCut}.txt not found" >> /dev/stderr
	    echo "          creating file from results of step4" >> /dev/stderr
	    echo "[WARNING] init file ${outDirTable}/params-step5-${commonCut}.txt not found" >> /dev/stdout
	    echo "          creating file from results of step4" >> /dev/stdout

	    grep constTerm ${outDirData}/step4/img/outProfile-`basename ${regionFileStep4EB} .dat`-${commonCut}-FitResult.config |sed -r 's|L|C L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' > ${outDirTable}/params-step5-${commonCut}.txt
	    grep constTerm ${outDirData}/step4/img/outProfile-`basename ${regionFileStep4EB} .dat`-${commonCut}-FitResult.config |sed -r 's|L|C L|;s|constTerm|alpha|;s|= [0-9]+.+/- [.0-9]+ |= 1.0000 +/- 0.0100 |;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${outDirTable}/params-step5-${commonCut}.txt
	    
	    grep constTerm ${outDirData}/step4/img/outProfile-`basename ${regionFileStep4EE} .dat`-${commonCut}-FitResult.config |grep -v absEta_0_1 |sed -r 's|L|C L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' >> ${outDirTable}/params-step5-${commonCut}.txt
	    grep constTerm ${outDirData}/step4/img/outProfile-`basename ${regionFileStep4EE} .dat`-${commonCut}-FitResult.config |grep -v absEta_0_1 |sed -r 's|L|C L|;s|constTerm|alpha|;s|= [0-9]+.+/- [.0-9]+ |= 1.0000 +/- 0.0100 |;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${outDirTable}/params-step5-${commonCut}.txt
	    initFile="--initFile=${outDirTable}/params-step5-${commonCut}.txt"; 
	    cat ${outDirTable}/params-step5-${commonCut}.txt
	    exit 0
	fi
	
	if [ "${extension}" == "weight" ];then
	    updateOnly="$updateOnly --useR9weight"
	fi
	tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
	for tag in $tags
	  do
	  if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEB}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEB .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEE}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEE .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	  fi
	done
	
	# save the corrections in root files
	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step5${extension}/`basename $configFile`

	for index in `seq 1 50`
	  do
	  mkdir ${outDirData}/step5${extension}/${index}/fitres/ -p 
	  mkdir ${outDirData}/step5${extension}/${index}/img -p 
	done

	bsub -q 2nd \
	    -oo ${outDirData}/step5${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
	    -eo ${outDirData}/step5${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
	    -J "${basenameEB} step5${extension}[1-50]" \
	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step5${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step5${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step5${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 --profileOnly --plotOnly || exit 1; touch ${outDirData}/step5${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"

	bsub -q 2nd \
	    -oo ${outDirData}/step5${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
	    -eo ${outDirData}/step5${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
	    -J "${basenameEE} step5${extension}[1-50]" \
	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step5${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step5${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step5${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 --profileOnly --plotOnly || exit 1; touch ${outDirData}/step5${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"

    fi
    while [ "`bjobs -J \"${basenameEB} step5${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
    while [ "`bjobs -J \"${basenameEE} step5${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

    ./script/haddTGraph.sh -o ${outDirData}/step5${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step5${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
    ./script/haddTGraph.sh -o ${outDirData}/step5${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step5${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root

    
	######################################################33
    echo "{" > tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step5${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step5${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "}" >> tmp/fitProfiles.C
    root -l -b -q tmp/fitProfiles.C


fi

if [ -n "${STEP6}" ];then
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi

    #eta x R9 with smearing method
    regionFileEB=${regionFileStep5EB}
    regionFileEE=${regionFileStep5EE}
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    outFile=step6${extension}-${invMass_var}-${newSelection}-${commonCut}-FitConfig.dat

    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
	echo "[ERROR] Impossible to run step4 without step2" >> /dev/stderr
	#exit 1
    fi

    if [ ! -e "${outDirTable}/${outFile}" ];then
	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
	if [ ! -e "${outDirData}/step6${extension}/fitres" ];then mkdir ${outDirData}/step6${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step6${extension}/img" ];then    mkdir ${outDirData}/step6${extension}/img -p; fi

	if [ -e "${outDirTable}/params-step5-${commonCut}.txt" ];then 
	    initFile="--initFile=${outDirTable}/params-step5-${commonCut}.txt"; 
	else 
	    echo "[ERROR] ${outDirTable}/params-step5-${commonCut}.txt init file not found" >> /dev/stderr
	    exit 1
	fi
	
	if [ "${extension}" == "weight" ];then
	    updateOnly="$updateOnly --useR9weight"
	fi
	tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
	for tag in $tags
	  do
	  if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEB}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEB .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEE}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEE .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	  fi
	done
	
	# save the corrections in root files
	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step6${extension}/`basename $configFile`

	for index in `seq 1 50`
	  do
	  mkdir ${outDirData}/step6${extension}/${index}/fitres/ -p 
	  mkdir ${outDirData}/step6${extension}/${index}/img -p 
	done

# 	bsub -q 2nd \
# 	    -oo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
# 	    -eo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
# 	    -J "${basenameEB} step6${extension}[1-50]" \
# 	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
# ./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step6${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"

# 	bsub -q 2nd \
# 	    -oo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
# 	    -eo ${outDirData}/step6${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
# 	    -J "${basenameEE} step6${extension}[1-50]" \
# 	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
# ./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step6${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"

    fi
    while [ "`bjobs -J \"${basenameEB} step6${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
    while [ "`bjobs -J \"${basenameEE} step6${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

    ./script/haddTGraph.sh -o ${outDirData}/step6${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step6${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
    ./script/haddTGraph.sh -o ${outDirData}/step6${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step6${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root

    
	######################################################33
    echo "{" > tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step6${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step6${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "}" >> tmp/fitProfiles.C
#    root -l -b -q tmp/fitProfiles.C

    cat ${outDirData}/step6${extension}/img/outProfile-${basenameEB}-${commonCut}-FitResult.config ${outDirData}/step6${extension}/img/outProfile-${basenameEE}-${commonCut}-FitResult.config > ${outFile}

    cat "`echo $initFile | sed 's|.*=||'`" |grep "C L" >>  ${outFile}

    ./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin --initFile=${outFile} corrEleType=HggRunEtaR9 --plotOnly  || exit 1

#     ./bin/ZFitter.exe -f $outDirData/step6${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step6${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step6${extension}/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin --initFile=${outFile} --corrEleType=HggRunEtaR9 --plotOnly  || exit 1


fi

if [ -n "${STEP7}" ];then
    if [ "${extension}" == "medium" -o "${extension}" == "tight" ];then
	newSelection=${extension}
    else
	newSelection=${selection}
    fi

    #eta x R9 with smearing method
    regionFileEB=${regionFileStep5EB}
    regionFileEE=${regionFileStep5EE}
    basenameEB=`basename $regionFileEB .dat`
    basenameEE=`basename $regionFileEE .dat`
    outFile=step6${extension}-${invMass_var}-${newSelection}-${commonCut}-HggRunEtaR9.dat

    if [ ! -e "${outDirTable}/${outFileStep2}" ];then
	echo "[ERROR] Impossible to run step4 without step2" >> /dev/stderr
	#exit 1
    fi

    if [ ! -e "${outDirTable}/${outFile}" ];then
	if [ ! -e "${outDirMC}/${extension}/fitres" ];then mkdir ${outDirMC}/${extension}/fitres -p; fi
	if [ ! -e "${outDirMC}/${extension}/img" ];then    mkdir ${outDirMC}/${extension}/img -p; fi
	if [ ! -e "${outDirData}/step7${extension}/fitres" ];then mkdir ${outDirData}/step7${extension}/fitres -p; fi
	if [ ! -e "${outDirData}/step7${extension}/img" ];then    mkdir ${outDirData}/step7${extension}/img -p; fi

	if [ -e "${outDirTable}/params-step7-${commonCut}.txt" ];then 
	    initFile="--initFile=${outDirTable}/params-step7-${commonCut}.txt"; 
	else 
	    echo "[WARNING] init file ${outDirTable}/params-step7-${commonCut}.txt not found" >> /dev/stderr
	    echo "          creating file from results of step6" >> /dev/stderr
	    echo "[WARNING] init file ${outDirTable}/params-step7-${commonCut}.txt not found" >> /dev/stdout
	    echo "          creating file from results of step6" >> /dev/stdout

	    grep scale ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EB} .dat`-${commonCut}-FitResult.config |sed -r 's|C L|L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0.98 - 1.02)|' > ${outDirTable}/params-step7-${commonCut}.txt
	    grep constTerm ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EB} .dat`-${commonCut}-FitResult.config |sed -r 's|C L|L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' >> ${outDirTable}/params-step7-${commonCut}.txt
	    grep constTerm ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EB} .dat`-${commonCut}-FitResult.config |sed -r 's|C L|L|;s|constTerm|alpha|;s|= [0-9]+.+/- [.0-9]+ |= 1.0000 +/- 0.0100 |;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${outDirTable}/params-step7-${commonCut}.txt

	    grep scale ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EE} .dat`-${commonCut}-FitResult.config |grep -v absEta_0_1 |sed -r 's|C L|L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0.98 - 1.02)|' >> ${outDirTable}/params-step7-${commonCut}.txt
	    grep constTerm ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EE} .dat`-${commonCut}-FitResult.config |grep -v absEta_0_1 |sed -r 's|L|C L|;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.03)|' >> ${outDirTable}/params-step7-${commonCut}.txt
	    grep constTerm ${outDirData}/step6/img/outProfile-`basename ${regionFileStep5EE} .dat`-${commonCut}-FitResult.config |grep -v absEta_0_1 |sed -r 's|L|C L|;s|constTerm|alpha|;s|= [0-9]+.+/- [.0-9]+ |= 1.0000 +/- 0.0100 |;s|\([.0-9]*[ ]+-[ ]+[.0-9]+\)|(0 - 0.20)|' >> ${outDirTable}/params-step7-${commonCut}.txt
	    initFile="--initFile=${outDirTable}/params-step7-${commonCut}.txt"; 
	    cat ${outDirTable}/params-step7-${commonCut}.txt
	    exit 0

	    echo "[ERROR] ${outDirTable}/params-step7-${commonCut}.txt init file not found" >> /dev/stderr
	    exit 1
	fi
	
	if [ "${extension}" == "weight" ];then
	    updateOnly="$updateOnly --useR9weight"
	fi
	tags=`grep -v '#' $configFile | sed -r 's|[ ]+|\t|g; s|[\t]+|\t|g' | cut -f 1  | sort | uniq | grep [s,d][1-9]`
	for tag in $tags
	  do
	  if [ "`grep -v '#' $configFile | grep \"^$tag\" | cut -f 2 | grep -c smearerCat`" == "0" ];then
	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEB}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEB .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEB .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	      ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFileEE}  \
		  --saveRootMacro \
		  --addBranch=smearerCat  --smearerFit
	      mv tmp/smearerCat_${tag}-`basename $configFile .dat`.root data/smearerCat/data/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root || exit 1
	      echo -e "$tag\tsmearerCat_`basename $regionFileEE .dat`\tdata/smearerCat/smearerCat_`basename $regionFileEE .dat`_${tag}-`basename $configFile .dat`.root" >> $configFile

	  fi
	done
	
	# save the corrections in root files
	# this way I do not reproduce the ntuples with the corrections any time
	cat $configFile \
	    | sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step7${extension}/`basename $configFile`

	for index in `seq 1 50`
	  do
	  mkdir ${outDirData}/step7${extension}/${index}/fitres/ -p 
	  mkdir ${outDirData}/step7${extension}/${index}/img -p 
	done

# 	bsub -q 2nd \
# 	    -oo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stdout.log \
# 	    -eo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEB}-stderr.log \
# 	    -J "${basenameEB} step7${extension}[1-50]" \
# 	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
# ./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEB} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step7${extension}/\$LSB_JOBINDEX/`basename $regionFileEB .dat`-done"

# 	bsub -q 2nd \
# 	    -oo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stdout.log \
# 	    -eo ${outDirData}/step7${extension}/%I/fitres/`basename ${outFile} .dat`-${basenameEE}-stderr.log \
# 	    -J "${basenameEE} step7${extension}[1-50]" \
# 	      "cd $PWD; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
# ./bin/ZFitter.exe -f $outDirData/step7${extension}/`basename $configFile` --regionsFile ${regionFileEE} $isOdd $updateOnly --selection=${newSelection}  --invMass_var ${invMass_var} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/${extension}/fitres --outDirImgMC=${outDirMC}/${extension}/img --outDirImgData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/img/ --outDirFitResData=${outDirData}/step7${extension}/\$LSB_JOBINDEX/fitres --constTermFix  --smearerFit  --smearingEt --autoNsmear --autoBin ${initFile}  --corrEleType=HggRunEtaR9 || exit 1; touch ${outDirData}/step7${extension}/\$LSB_JOBINDEX/`basename $regionFileEE .dat`-done"

     fi
    while [ "`bjobs -J \"${basenameEB} step7${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done
    while [ "`bjobs -J \"${basenameEE} step7${extension}\" | grep -v JOBID | grep -v found | wc -l`" != "0" ]; do /bin/sleep 2m; done

    ./script/haddTGraph.sh -o ${outDirData}/step7${extension}/fitres/outProfile-$basenameEB-${commonCut}.root ${outDirData}/step7${extension}/*/fitres/outProfile-$basenameEB-${commonCut}.root
    ./script/haddTGraph.sh -o ${outDirData}/step7${extension}/fitres/outProfile-$basenameEE-${commonCut}.root ${outDirData}/step7${extension}/*/fitres/outProfile-$basenameEE-${commonCut}.root

    
	######################################################33
    echo "{" > tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
    echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C+\");" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step7${extension}/fitres/outProfile-$basenameEB-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "FitProfile2(\"${outDirData}/step7${extension}/fitres/outProfile-$basenameEE-${commonCut}.root\",\"\",\"\",true,true,false);" >> tmp/fitProfiles.C
    echo "}" >> tmp/fitProfiles.C
    root -l -b -q tmp/fitProfiles.C


fi

exit 0
# if [ -n "${STEP5}" ];then
#     # traditional resolution categories, Et bin categorization, only profile and histograms

#     if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
#     if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
#     if [ ! -e "${outDirData}/step5/fitres/$index" ];then mkdir ${outDirData}/step5/fitres/$index -p; fi
#     if [ ! -e "${outDirData}/step5/img/$index" ];then mkdir ${outDirData}/step5/img/$index -p; fi

#     #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
#     regionFile=data/regions/scaleStep2smearing_9.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step5/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step5/img \
# 	--constTermFix  --plotOnly \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
# 	--initFile=${outDirData}/table/step5-scaleStep2smearing_9-${commonCut}.txt \
# 	--addBranch=smearerCat > ${outDirData}/log/step5-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

# #    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
#     exit 0

#     regionFile=data/regions/scaleStep2smearing_10.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step5/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step5/img \
# 	--constTermFix  \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
# 	--initFile=${outDirData}/table/step5-scaleStep2smearing_10-${commonCut}.txt \
# 	--addBranch=smearerCat > ${outDirData}/log/step5-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

# fi


# if [ -n "${STEP6}" ];then
#     # traditional resolution categories, only scales in Et bins

#     if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
#     if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
#     if [ ! -e "${outDirData}/step6/fitres/$index" ];then mkdir ${outDirData}/step6/fitres/$index -p; fi
#     if [ ! -e "${outDirData}/step6/img/$index" ];then mkdir ${outDirData}/step6/img/$index -p; fi

#     #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
#     regionFile=data/regions/scaleStep2smearing_9.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut}-R9p_0.8 \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step6/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step6/img \
# 	--constTermFix  \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
# 	--initFile=${outDirData}/table/step6-scaleStep2smearing_9-${commonCut}-R9p_0.8.txt \
# 	--addBranch=smearerCat > ${outDirData}/log/step6-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

# #    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
#     exit 0

#     regionFile=data/regions/scaleStep2smearing_10.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step6/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step6/img \
# 	--constTermFix  \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
# 	--initFile=${outDirData}/table/step6-scaleStep2smearing_10-${commonCut}.txt \
# 	--addBranch=smearerCat > ${outDirData}/log/step6-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

# fi


if [ -n "${STEP6}" ];then
    # closure test: traditional resolution categories, only scales in Et bins

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step6/fitres/$index" ];then mkdir ${outDirData}/step6/fitres/$index -p; fi
    if [ ! -e "${outDirData}/step6/img/$index" ];then mkdir ${outDirData}/step6/img/$index -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step6/`basename $configFile`


    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_9.dat
    initFile=${outDirData}/table/step6-scaleStep2smearing_9-${commonCut}-R9p_0.8.txt
    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq | grep -v '#' > $outDirData/step6/`basename $configFile`
    tags=`grep -v '#' $configFile | cut -f 1  | sort | uniq | grep d[1-9]`

    for tag in $tags
      do
      if [ ! -e "${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" ];then
	  ls ${outDirData}/step2/ #scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	      --saveRootMacro \
	      --corrEleType HggRunEtaR9 \
	      --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat 
	  mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/
      fi
    done

    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi

    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step6/${index}/fitres/ -p 
      mkdir ${outDirData}/step6/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step6/${index}/fitres/step6-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-${index}-stdout.log -eo ${outDirData}/step6/${index}/fitres/step6-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-${index}-stderr.log -J "`basename $regionFile .dat` step6" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step6/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut}-R9p_0.8  \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step6/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step6/${index}/img/ \
	--constTermFix  \
	--smearerFit  --autoNsmear \
	--initFile=${initFile} \
	--addBranch=smearerCat || exit 1"
    done

    #########################
    regionFile=data/regions/scaleStep2smearing_10.dat
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi
	
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step6/${index}/fitres/ -p 
      mkdir ${outDirData}/step6/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step6/${index}/fitres/step6-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step6/${index}/fitres/step6-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step6" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step6/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut}-R9p_0.8  \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step6/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step6/${index}/img/ \
	--constTermFix  \
	--smearerFit  --autoNsmear \
	--initFile=${initFile} \
	--addBranch=smearerCat || exit 1"
    done
fi

if [ -n "${STEP7}" ];then
    # closure test: traditional resolution categories, only scales in Et bins
    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step7/fitres/$index" ];then mkdir ${outDirData}/step7/fitres/$index -p; fi
    if [ ! -e "${outDirData}/step7/img/$index" ];then mkdir ${outDirData}/step7/img/$index -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step7/`basename $configFile`


    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_9.dat
    tags=`grep -v '#' $configFile | cut -f 1  | sort | uniq | grep d[1-9]`

    for tag in $tags
      do
      if [ ! -e "${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" ];then
	  ls ${outDirData}/step2/ #scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	      --saveRootMacro \
	      --corrEleType HggRunEtaR9 \
	      --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat 
	  mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/
      fi
    done

    if [ ! -e "${outDirData}/table/step5-scaleStep2smearing_9-${commonCut}.txt" ];then
	echo "[ERROR] File ${outDirData}/table/step5-scaleStep2smearing_9-${commonCut}.txt not found" >> /dev/stderr
	exit 1
    fi
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step7/${index}/fitres/ -p 
      mkdir ${outDirData}/step7/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step7/${index}/fitres/step7-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step7/${index}/fitres/step7-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step7" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step7/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step7/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step7/${index}/img/ \
	--constTermFix  \
	--smearerFit  --autoNsmear \
	--initFile=${outDirData}/table/step5-scaleStep2smearing_9-${commonCut}.txt \
	--addBranch=smearerCat || exit 1"
    done

    #########################
    regionFile=data/regions/scaleStep2smearing_10.dat
    initFile=${outDirData}/table/step5-scaleStep2smearing_9-${commonCut}.txt
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi
	
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step7/${index}/fitres/ -p 
      mkdir ${outDirData}/step7/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step7/${index}/fitres/step7-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step7/${index}/fitres/step7-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step7" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step7/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step7/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step7/${index}/img/ \
	--constTermFix  --smearerFit  --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1"
    done
fi


if [ -n "${STEP9}" ];then
    # stochastic term resolution categories,  scales in Et bins

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step9/fitres/$index" ];then mkdir ${outDirData}/step9/fitres/$index -p; fi
    if [ ! -e "${outDirData}/step9/img/$index" ];then mkdir ${outDirData}/step9/img/$index -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step9/`basename $configFile`


    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_9.dat
    tags=`grep -v '#' $configFile | cut -f 1  | sort | uniq | grep d[1-9]`
    initFile=${outDirData}/table/step9-scaleStep2smearing_9-${commonCut}.txt
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi

    for tag in $tags
      do
      if [ ! -e "${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" ];then
	  ls ${outDirData}/step2/ #scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	      --saveRootMacro \
	      --corrEleType HggRunEtaR9 \
	      --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat 
	  mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/
      fi
    done

    for index in `seq 141 141`
      do
      mkdir ${outDirData}/step9/${index}/fitres/ -p 
      mkdir ${outDirData}/step9/${index}/img -p 
#      bsub -q 2nd -oo ${outDirData}/step9/${index}/fitres/step9-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step9/${index}/fitres/step9-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step9" \
#	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step9/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step9/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step9/${index}/img/ \
        --profileOnly --plotOnly \
	--constTermFix  --smearerFit  --smearingEt --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1 #"
    done
    exit 0
    #########################
    regionFile=data/regions/scaleStep2smearing_10.dat
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi
	
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step9/${index}/fitres/ -p 
      mkdir ${outDirData}/step9/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step9/${index}/fitres/step9-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step9/${index}/fitres/step9-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step9" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step9/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step9/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step9/${index}/img/ \
	--constTermFix  --smearingEt --smearerFit  --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1"
    done
fi


if [ -n "${STEP10}" ];then
    # stochastic term resolution categories,  scales in Et bins, only histos 

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step10/fitres/$index" ];then mkdir ${outDirData}/step10/fitres/$index -p; fi
    if [ ! -e "${outDirData}/step10/img/$index" ];then mkdir ${outDirData}/step10/img/$index -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step10/`basename $configFile`


    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_9.dat
    tags=`grep -v '#' $configFile | cut -f 1  | sort | uniq | grep d[1-9]`
    initFile=${outDirData}/table/step10-scaleStep2smearing_9-${commonCut}.txt
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi

    for tag in $tags
      do
      if [ ! -e "${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" ];then
	  ls ${outDirData}/step2/ #scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	      --saveRootMacro \
	      --corrEleType HggRunEtaR9 \
	      --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat 
	  mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/
      fi
    done

    for index in `seq 141 141`
      do
      mkdir ${outDirData}/step10/${index}/fitres/ -p 
      mkdir ${outDirData}/step10/${index}/img -p 
#      bsub -q 2nd -oo ${outDirData}/step10/${index}/fitres/step10-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step10/${index}/fitres/step10-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step10" \
#	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; 
./bin/ZFitter.exe -f $outDirData/step10/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step10/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step10/${index}/img/ \
        --plotOnly \
	--constTermFix  --smearerFit  --smearingEt --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1 #"
    done
    exit 0
    #########################
    regionFile=data/regions/scaleStep2smearing_10.dat
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi
	
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step10/${index}/fitres/ -p 
      mkdir ${outDirData}/step10/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step10/${index}/fitres/step10-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step10/${index}/fitres/step10-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step10" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step10/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step10/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step10/${index}/img/ \
	--constTermFix  --smearingEt --smearerFit  --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1"
    done
fi

if [ -n "${STEP8}" ];then
    # stochastic term resolution categories,  scales in Et bins

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step8/fitres/$index" ];then mkdir ${outDirData}/step8/fitres/$index -p; fi
    if [ ! -e "${outDirData}/step8/img/$index" ];then mkdir ${outDirData}/step8/img/$index -p; fi

    cat $configFile \
	| sed "/selected/{p; s|^\(d[1-9]\)\tselected.*|\1\tscaleEle_HggRunEtaR9\t${outDirData}/step2/scaleEle_HggRunEtaR9_\1-`basename $configFile .dat`.root|}" | sort | uniq > $outDirData/step8/`basename $configFile`


    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_9.dat
    tags=`grep -v '#' $configFile | cut -f 1  | sort | uniq | grep d[1-9]`
    initFile=${outDirData}/table/step8-scaleStep2smearing_9-${commonCut}.txt
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi

    for tag in $tags
      do
      if [ ! -e "${outDirData}/step2/scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root" ];then
	  ls ${outDirData}/step2/ #scaleEle_HggRunEtaR9_${tag}-`basename $configFile .dat`.root
	  ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	      --saveRootMacro \
	      --corrEleType HggRunEtaR9 \
	      --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat 
	  mv tmp/scaleEle_HggRunEtaR9_[s,d][1-9]-`basename $configFile .dat`.root ${outDirData}/step2/
      fi
    done

    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step8/${index}/fitres/ -p 
      mkdir ${outDirData}/step8/${index}/img -p 
      bsub -q 2nw -oo ${outDirData}/step8/${index}/fitres/step8-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step8/${index}/fitres/step8-scaleStep2smearing_9-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step8" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step8/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step8/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step8/${index}/img/ \
        --profileOnly --plotOnly \
	--constTermFix  --smearerFit  --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1"
    done

    #########################
    regionFile=data/regions/scaleStep2smearing_10.dat
    if [ ! -e "${initFile}" ];then
	echo "[ERROR] File ${initFile}" >> /dev/stderr
	exit 1
    fi
	
    for index in `seq 1 30`
      do
      mkdir ${outDirData}/step8/${index}/fitres/ -p 
      mkdir ${outDirData}/step8/${index}/img -p 
      bsub -q 2nd -oo ${outDirData}/step8/${index}/fitres/step8-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stdout.log -eo ${outDirData}/step8/${index}/fitres/step8-scaleStep2smearing_10-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9-stderr.log -J "`basename $regionFile .dat` step8" \
	  "cd /afs/cern.ch/user/s/shervin/scratch1/calib/CMSSW_5_3_7_patch6/src/Calibration/ZFitter; eval \`scramv1 runtime -sh\`; uname -a;  echo \$CMSSW_VERSION; ./bin/ZFitter.exe -f $outDirData/step8/`basename $configFile` --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step8/${index}/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step8/${index}/img/ \
	--constTermFix  --smearerFit  --autoNsmear --initFile=${initFile} --addBranch=smearerCat || exit 1"
    done
fi


if [ -n "${SYSTEMATICS}" ];then
    regionFile=data/regions/systematics.dat
    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step3/fitres" ];then mkdir ${outDirData}/step3/fitres -p; fi
    if [ ! -e "${outDirData}/step3/img" ];then mkdir ${outDirData}/step3/img -p; fi

    
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step3/img \
 	--corrEleType HggRunEtaR9 --corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat > ${outDirData}/log/systematics-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    
    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/systematics-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1

    for xVar in nPV
      do
      if [ ! -e "${outDirData}/step3/img/stability/$xVar/$PERIOD" ];then
	  mkdir ${outDirData}/step3/img/stability/$xVar/$PERIOD/ -p
      fi
      ./script/stability.sh -t ${tableFile} \
	  --outDirImgData ${outDirData}/step3/img/stability/$xVar/$PERIOD/ -x $xVar -y peak --column 51 $xMin $xMax || exit 1
#      ./script/stability.sh -t ${tableFile} \
#	  --outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y scaledWidth --column 51 $xMin $xMax || exit 1
    done

fi

if [ -n "${STEP1Stability}" ];then
##################################################
    regionFile=data/regions/stability.dat
    #runRangesFile=data/runRanges/monitoring.dat

    if [ ! -e "${outDirData}/step1/fitres" ];then mkdir ${outDirData}/step1/fitres -p; fi
    if [ ! -e "${outDirData}/step1/img" ];then mkdir ${outDirData}/step1/img -p; fi
 
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
	$updateOnly --invMass_var ${invMass_var} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step1/fitres \
	--corrEleType HggRunEta \
	--corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat \
	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step1/img > ${outDirData}/log/step1-stability.log || exit 1

    ./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step1/fitres \
	>  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex || exit 1
    

###################### Make stability plots
### TODO: make a stability plot with old stabilities and new stabilities superimposed
    
    xVar=runNumber
    if [ ! -d ${outDirData}/step1/img/stability/$xVar ];then
	mkdir -p ${outDirData}/step1/img/stability/$xVar
    fi

    ./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	--outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y peak || exit 1
    ./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	--outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y scaledWidth || exit 1
    ./script/stability.sh -t  ${outDirTable}/step1_stability-${invMass_var}-${selection}.tex \
	--outDirImgData ${outDirData}/step1/img/stability/$xVar/ -x $xVar -y peak --allRegions || exit 1
    
    
fi

if [ -n "${STEP3Stability}" ];then
##################################################
    regionFile=data/regions/stability.dat
    runRangesFile=data/runRanges/monitoring.dat
#    runRangesFile=data/runRanges/monitoring_2011.dat
    
    if [ ! -e "${outDirData}/step3/fitres" ];then mkdir ${outDirData}/step3/fitres -p; fi
    if [ ! -e "${outDirData}/step3/img" ];then mkdir ${outDirData}/step3/img -p; fi
 
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
	$updateOnly --invMass_var ${invMass_var} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres \
	--corrEleType HggRunEtaR9 \
	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step3/img > ${outDirData}/log/step3-stability.log || exit 1

    ./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres \
	>  ${outDirTable}/step3_stability-${invMass_var}-${selection}.tex || exit 1
    

###################### Make stability plots
### TODO: make a stability plot with old stabilities and new stabilities superimposed
    
    xVar=runNumber
    if [ ! -d ${outDirData}/step3/img/stability/$xVar ];then
	mkdir -p ${outDirData}/step3/img/stability/$xVar
    fi

    ./script/stability.sh -t  ${outDirTable}/step3_stability-${invMass_var}-${selection}.tex \
	--outDirImgData ${outDirData}/step3/img/stability/$xVar/ -x $xVar -y peak || exit 1
    ./script/stability.sh -t  ${outDirTable}/step3_stability-${invMass_var}-${selection}.tex \
	--outDirImgData ${outDirData}/step3/img/stability/$xVar/ -x $xVar -y scaledWidth || exit 1
    
    
fi

if [ -n "$SLIDE" ];then
    echo "[STATUS] Making slides"
    dirData=`dirname $outDirData` # remove the invariant mass subdir
    dirData=`dirname $dirData` # remove the selection subdir
    dirMC=`dirname $outDirMC` # remove the invariant mass subdir
    dirMC=`dirname $dirMC` # remove the selection subdir
    
    ./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type scales
fi

#### put the slides for the stablity closure test
