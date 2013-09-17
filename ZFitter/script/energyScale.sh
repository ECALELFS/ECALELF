#!/bin/bash
source script/functions.sh
# 
# energy scale derived in different steps:
# - time dependence (step1)
# - material dependence  (step2)
# - closure test (step3)

commonCut=Et_25-trigger-noPF
selection=loose
invMass_var=invMass_SC_regrCorr_pho
baseDir=test
updateOnly="--updateOnly" # --profileOnly --initFile=init.txt"
#updateOnly=""

usage(){
    echo "`basename $0` [options]" 
    echo " -f arg            config file"
#    echo " --regionFile arg             "
    echo " --runRangesFile arg           "
    echo " --selection arg (=$selection)"
    echo " --invMass_var arg"
    echo " --commonCut arg (=$commonCut)"
    echo " --step arg: 1, 2, 2fit, 3, 3weight, slide, 3stability, syst, 1-2,1-3,1-3stability and all ordered combination"
}
#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,regionFile:,runRangesFile:,selection:,invMass_var:,step:,baseDir:,commonCut: -- "$@")
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
    *)
	echo "[ERROR] Selection ${selection} not configured" >> /dev/stderr
        exit 1
        ;;
esac



case ${STEP} in
    1)	STEP1=y;;
    2) 	STEP2=y;;
    3) 	STEP3=y;; #SLIDE=y;;
    4)  STEP4=y;;
    1-2) STEP1=y; STEP2=y;;
    1-3) STEP1=y; STEP2=y; STEP3=y; SLIDE=y;;
    2-3) STEP2=y; STEP3=y; SLIDE=y;;
    1-3stability) STEP1=y; STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    2-3stability) STEP2=y; STEP3=y; STEP3Stability=y; SLIDE=y;;
    slide) SLIDE=y;;
    3stability) STEP3Stability=y;;
    1stability) STEP1Stability=y;;
	syst) SYSTEMATICS=y;;
    3weight) STEP3WEIGHT=y;;
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

if [ -n "${STEP1}" ];then
regionFile=data/regions/scaleStep1.dat


    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
    if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
    if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
    if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi

	./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile}  \
	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut $commonCut \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	--outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log//step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.log || exit 1
    
    ./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut ${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	>  ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.tex
    
    ./script/tex2txt.sh ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.tex | awk -F "\t" -f awk/recalibOutput.awk |grep -v '^%' > ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat
fi

echo "DONE WITH STEP 1 ========================="

if [ -n "${STEP2FIT}" ];then

    regionFile=data/regions/scaleStep2.dat

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step2/fitres" ];then mkdir ${outDirData}/step2/fitres -p; fi
    if [ ! -e "${outDirData}/step2/img" ];then mkdir ${outDirData}/step2/img -p; fi


   ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
	$isOdd  $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
	 --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
	 --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img \
	 --corrEleType HggRunEta --corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat > ${outDirData}/log/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
   
     ./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres > ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
     cut -d '&' -f 1-5   ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex |  sed "s|-$commonCut||" > tmp/s.tex
     ./script/CalcResCorrection.sh tmp/s.tex > tmp/res_corr.dat

     ./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |grep -v '#' > ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
#    ./script/makeCorrVsRunNumber.py -l -c --file2=res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |sed -f sed/tex.sed > ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex

     cat ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat | cut -f 1,3-6 | awk '{printf("%s\t%s - %s\t%s\t%s \n", $1,$2,$3, $4, $5)}' | sed -r 's|\t|\t\&\t|g;' | sed -f sed/tex.sed | sed 's|R9|\\RNINE|' > ${outDirTable}/correctionNote-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
    
fi



if [ -n "${STEP2}" ];then

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step2/fitres" ];then mkdir ${outDirData}/step2/fitres -p; fi
    if [ ! -e "${outDirData}/step2/img" ];then mkdir ${outDirData}/step2/img -p; fi

    if [ ! -e "${outDirData}/step2_fixed/fitres" ];then mkdir ${outDirData}/step2_fixed/fitres -p; fi
    if [ ! -e "${outDirData}/step2_fixed/img" ];then mkdir ${outDirData}/step2_fixed/img -p; fi

    if [ ! -e "${outDirData}/step2_0.8bad/fitres" ];then mkdir ${outDirData}/step2_0.8bad/fitres -p; fi
    if [ ! -e "${outDirData}/step2_0.8bad/img" ];   then mkdir ${outDirData}/step2_0.8bad/img -p; fi

    
    #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
    regionFile=data/regions/scaleStep2smearing_8.dat
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut}-R9p_0.8 \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2_0.8bad/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2_0.8bad/img \
	--smearerFit  --autoNsmear --constTermFix --alphaGoldFix \
	--corrEleType HggRunEta \
	--corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat \
        --initFile=${outDirData}/step2/fitres/params-scaleStep2smearing_8-${commonCut}-R9p_0.8.txt \
	--addBranch=smearerCat > ${outDirData}/log/step20.8badsmearing_8-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

    exit 0
    regionFile=data/regions/scaleStep2smearing_8.dat
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  \
	--smearerFit --autoNsmear  \
        $isOdd  $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	 --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
 	 --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img \
	--addBranch=smearerCat --constTermFix \
 	 --corrEleType HggRunEta --corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat > ${outDirData}/log/step2smearing_8-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log  


    exit 0
    regionFile=data/regions/scaleStep2smearing_1.dat
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --smearerFit --autoNsmear  \
        $isOdd  $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	 --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
 	 --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img \
 	 --corrEleType HggRunEta --corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat > ${outDirData}/log/step2smearing_1-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log  

#     cp ${outDirData}/step2/fitres/params-${regionFile}-${commonCut}.txt ${outDirData}/step2/fitres/step2smearing_1.txt
#     cp ${outDirData}/step2/fitres/histos-${regionFile}-${commonCut}.root ${outDirData}/step2/fitres/histos_step2smearing_1.root
#     cp ${outDirData}/step2/fitres/outProfile-${regionFile}-${commonCut}.root ${outDirData}/step2/fitres/outProfile_step2smearing_1.root
#     sed -i 's|L|C L|;s|C C L|C L|' ${outDirData}/step2/fitres/params.txt

    grep scale ${outDirData}/step2/fitres/params-`basename ${regionFile} .dat`-${commonCut}.txt | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' > tmp/res_corr.dat

    regionFile=data/regions/scaleStep2smearing_2.dat
   ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --smearerFit  --autoNsmear  \
       $isOdd  $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
       --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step2/fitres \
       --outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step2/img \
       --initFile=${outDirData}/step2/fitres/params-scaleStep2smearing_1-${commonCut}.txt \
       --corrEleType HggRunEta --corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat > tmp/step2smearing_2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log  
   
#   cp ${outDirData}/step2/fitres/params.txt ${outDirData}/step2/fitres/step2smearing_2.txt
#   cp ${outDirData}/step2/fitres/histos.root ${outDirData}/step2/fitres/histos_step2smearing_2.root
#   cp ${outDirData}/step2/fitres/outProfile.root ${outDirData}/step2/fitres/outProfile_step2smearing_2.root

    grep scale ${outDirData}/step2/fitres/params-`basename ${regionFile} .dat`-${commonCut}.txt | grep EE | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' >> tmp/res_corr.dat

     ./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |grep -v '#' > ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat
    ./script/makeCorrVsRunNumber.py -l -c --file2=tmp/res_corr.dat --file1=${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat |sed -f sed/tex.sed > ${outDirTable}/step2smearing-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex

#      cat ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat | cut -f 1,3-6 | awk '{printf("%s\t%s - %s\t%s\t%s \n", $1,$2,$3, $4, $5)}' | sed -r 's|\t|\t\&\t|g;' | sed -f sed/tex.sed | sed 's|R9|\\RNINE|' > ${outDirTable}/correctionNote-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
    
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

    if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
    if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
    if [ ! -e "${outDirData}/step4/fitres" ];then mkdir ${outDirData}/step4/fitres -p; fi
    if [ ! -e "${outDirData}/step4/img" ];then mkdir ${outDirData}/step4/img -p; fi

    if [ ! -e "${outDirData}/step4_fixed/fitres" ];then mkdir ${outDirData}/step4_fixed/fitres -p; fi
    if [ ! -e "${outDirData}/step4_fixed/img" ];then mkdir ${outDirData}/step4_fixed/img -p; fi

    if [ ! -e "${outDirData}/step4_constOnly/fitres" ];then mkdir ${outDirData}/step4_constOnly/fitres -p; fi
    if [ ! -e "${outDirData}/step4_constOnly/img" ];then mkdir ${outDirData}/step4_constOnly/img -p; fi

#     #${outDirData}/step2/fitres/initFile-scaleStep2smearing_8-${commonCut}.txt \
#     regionFile=data/regions/scaleStep2smearing_8.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step4_fixed/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step4_fixed/img \
# 	--smearerFit  --autoNsmear --constTermFix --alphaGoldFix \
# 	--corrEleType HggRunEta \
# 	--corrEleFile ${outDirTable}/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.dat \
# 	--plotOnly  --initFile=${outDirData}/step2/fitres/params-scaleStep2smearing_8-${commonCut}.txt \
# 	--addBranch=smearerCat > ${outDirData}/log/step4fixedplotOnlysmearing_8-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1

    #./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
#	exit 0
#     regionFile=data/regions/scaleStep4smearing_0.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step4/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step4/img \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
# 	--profileOnly --initFile=${outDirData}/step2/img/outProfile-scaleStep2smearing-${commonCut}-FitResult.config \
# 	> ${outDirData}/log/step4_0-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log  || exit 1
    
#     regionFile=data/regions/scaleStep4smearing_0a.dat
#     ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
#  	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
#  	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step4/fitres \
#  	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step4/img \
# 	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
# 	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat  \
# 	--profileOnly --initFile=${outDirData}/step2/img/outProfile-scaleStep2smearing-${commonCut}-FitResult.config \
# 	> ${outDirData}/log/step4smearing_0a-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    
    regionFile=data/regions/scaleStep4smearing_1.dat
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step4/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step4/img \
	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
	--plotOnly --initFile=${outDirData}/step2/img/outProfile-scaleStep2smearing-${commonCut}-FitResult.config \
	--addBranch=smearerCat \
	> ${outDirData}/log/step4smearing_1-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    #./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
    
#     #    cp ${outDirData}/step4/fitres/params.txt ${outDirData}/step4/fitres/step4smearing_1.txt
#     #    cp ${outDirData}/step4/fitres/histos.root ${outDirData}/step4/fitres/histos_step4smearing_1.root
#    #    cp ${outDirData}/step4/fitres/outProfile.root ${outDirData}/step4/fitres/outProfile_step4smearing_1.root
    regionFile=data/regions/scaleStep4smearing_2.dat
    #	--initFile=${outDirData}/step4/fitres/params.txt \

    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}   \
 	$isOdd $updateOnly --invMass_var ${invMass_var} --commonCut ${commonCut} \
 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step4/fitres \
 	--outDirImgMC=${outDirMC}/img --outDirImgData=${outDirData}/step4/img \
	--smearerFit  --autoNsmear --corrEleType HggRunEtaR9 \
	--corrEleFile ${outDirTable}/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.dat \
	--plotOnly --initFile=${outDirData}/step2/img/outProfile-scaleStep2smearing-${commonCut}-FitResult.config \
	--addBranch=smearerCat \
	> ${outDirData}/log/step4smearing_2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.log || exit 1
    #./script/makeTable.sh --regionsFile ${regionFile} --commonCut ${commonCut} --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/step3/fitres > ${outDirTable}/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex || exit 1
 #   cp ${outDirData}/step4/fitres/params.txt ${outDirData}/step4/fitres/step4smearing_2.txt
 #   cp ${outDirData}/step4/fitres/histos.root ${outDirData}/step4/fitres/histos_step4smearing_2.root
 #   cp ${outDirData}/step4/fitres/outProfile.root ${outDirData}/step4/fitres/outProfile_step4smearing_2.root

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
