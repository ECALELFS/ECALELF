#!/bin/bash
source script/functions.sh
#tag_name=""
commonCut=isEle-Et_25
selection=loose25nsRun22016Moriond
invMass_var=invMass_ECAL_ele
invMass_min=65
invMass_max=115
configFile=data/validation/monitoring_2016.dat
runRangesFile=data/runRanges/monitoringRun2016-v2.dat
baseDir=testNew
#extraOptions="--anyVarBranches=timeSeedSC"

# VALIDATION=y
# STABILITY=y
# SLIDES=y
#STEP4=y

usage(){
    echo "`basename $0` [options] [what to do]" 
    echo "----- optional paramters"
    echo " -f arg (=${configFile})"
#    echo " --puName arg             "
    echo " --runRangesFile arg (=${runRangesFile})  run ranges for stability plots"
    echo " --selection arg (=${selection})     "
    echo " --invMass_var arg (=${invMass_var})"
    echo " --baseDir arg (=${baseDir})  base directory for outputs"
    echo " --commonCut arg (=${commonCut})"
	echo "----- what to do"
    echo " --validation: run the validation only, not the history and etaScale fits"
    echo " --stability:  run the stability fits only"
    echo " --etaScale:   run the fits in |eta| only"
    echo " --R9Ele:      run the fits in bin of R9 only"
    echo " --onlyTable:  do not run the fits, recreate only the tables and the stability plots"
    echo " --systematics all/pu      "
    echo " --refreg"
    echo " --slides            "
    echo " --test"
    echo " --period RUN2012A, RUN2012AB, RUN2012C, RUN2012D"
#    echo " --cruijff"
	echo " --noPU"
}

desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help,runRangesFile:,selection:,invMass_var:,puName:,baseDir:,validation,stability,etaScale,systematics:,slides,onlyTable,test,commonCut:,period:,cruijff,refreg,R9Ele,noPU -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
        -f) configFile=$2; shift;;
        --invMass_var) invMass_var=$2; echo "[OPTION] invMass_var = ${invMass_var}"; shift;;
	--puName) puName=$2; shift;;
	--noPU) NOPU="yes";;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--validation)  VALIDATION=y;;
	--stability)   STABILITY=y;;
	--etaScale)    ETA=y;;
	--R9Ele)       R9ELE=y;;
	--refreg)      REFREG=y;;
	--systematics) SYSTEMATICS=$2; shift;;
	--slides)      SLIDES=y;;
	--onlyTable)   ONLYTABLE=y;;
	--test)        TEST=y;;
	--selection)   selection=$2; echo "[OPTION] Selection=${selection}"; shift;;
	--commonCut)   commonCut=$2; echo "[OPTION] commonCut=${commonCut}"; shift;;
	--period) PERIOD=$2; shift;
	    case $PERIOD in
		RUN2012A)
		    ;;
		RUN2012B)
		    ;;
		RUN2012AB)
		    ;;
		RUN2012C)
		    ;;
		RUN2012D)
		    ;;
		*)
		    echo "[ERROR] period not defined. Only RUN2012A, RUN2012B, RUN2012AB, RUN2012C" >> /dev/stderr
		    exit 1
		    ;;
	    esac
	    period="--period=$PERIOD"
	    ;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

if [ -z "${VALIDATION}" -a -z "${STABILITY}" -a -z "${SLIDES}" -a -z "${SYSTEMATICS}" -a -z "${ETA}" -a -z "${REFREG}" -a -z "${R9ELE}" ];then
    # if no option selected, run all the sequence
    VALIDATION=y
    STABILITY=y
    ETA=y
    SLIDES=y
fi

if [ -n "${TEST}" ];then
    unset VALIDATION
    unset STABILITY
    unset ETA
    unset SLIDES
fi

if [ ! -d "tmp" ];then mkdir tmp; fi

# file with ntuples
if [ -z "${configFile}" -a -z "${TEST}" ];then
    echo "[ERROR] configFile not specified" >> /dev/stderr
    exit 1
elif  [ -z "${configFile}" -a -n "${TEST}" ];then
    configFile=data/validation/test.dat
fi

if [ ! -r "${configFile}" ];then
    echo "[ERROR] configFile ${configFile} not found or not readable" >> /dev/stderr
    exit 1
fi


afsPU=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/ECALELF/puFiles

## pileup reweight name
if [ -z "${NOPU}" ];then
	puName ${configFile}
	echo "PUName: $puName"
	extraOptions="${extraOptions} --noPU"
fi

mcName ${configFile}
echo "mcName: ${mcName}"

#################### NAMING OUTPUT FOLDERS

if [ "`grep -c rereco ${configFile}`" != "0" ];then
    TAG=`basename ${configFile} .dat`
	rereco=`echo $configFile | sed 's|.*rereco/||;s|/.*.dat||'`
    outDirData=$baseDir/dato/rereco/${rereco}/`basename ${configFile} .dat`/${selection}/${invMass_var}
else
    outDirData=$baseDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
fi

outDirMC=$baseDir/MC/${mcName}/${puName}/${selection}/${invMass_var}


signalTypeName="anyVar"
outDirData=${outDirData}/${signalTypeName}
outDirMC=${outDirMC}/${signalTypeName}
outDirTable=${outDirData}/table


mkdir -p ${outDirTable}/${PERIOD} 

if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi


if [ -n "$VALIDATION" ];then
    regionFile=data/regions/validation.dat
	if [ -z "${ONLYTABLE}" ];then
		./bin/ZFitter.exe -f ${configFile} --anyVar --regionsFile ${regionFile}  --invMass_var ${invMass_var} \
			--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
			--commonCut=${commonCut}  --selection=${selection} \
			--outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img ${extraOptions} > ${outDirData}/log/validation.log|| exit 1
	fi


#    ./script/makeTable2.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
#	 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres ${tableCruijffOption} \
#	 	>  ${outDirTable}/$PERIOD/monitoring_summary-${invMass_var}-${selection}-${commonCut}.dat || exit 1
fi

##################################################
if [ -n "$STABILITY" ];then
    regionFile=data/regions/stability.dat
    #mkdir ${outDirData}/{fitres,img}/{floating,invMass_range,invMass_range2} -p

    xVar=runNumber
    if [ -z "$PERIOD" ];then
		if [ -z "${ONLYTABLE}" ];then
			./bin/ZFitter.exe --anyVar -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
				${extraOptions} \
				$updateOnly --commonCut=${commonCut} \
				--invMass_var ${invMass_var} --selection=${selection} \
				--invMass_min=${invMass_min} --invMass_max=${invMass_max} \
				--outDirFitResMC=${outDirMC}/fitres \
				--outDirImgMC=${outDirMC}/img  \
				--outDirFitResData=${outDirData}/fitres --outDirImgData=${outDirData}/img \
				> ${outDirData}/log/monitoring_stability.log || exit 1

		fi
		#./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} --runRangesFile ${runRangesFile} \
		#    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres/floating ${tableCruijffOption} \
		#    >  ${outDirTable}/monitoring_stability_floating-${invMass_var}-${selection}.tex || exit 1
#		./script/makeTable2.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
#	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
#			>  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}-${commonCut}.dat || exit 1
    fi

###################### Make stability plots
	imgDir=${outDirData}/img/stability/$xVar/$PERIOD
    if [ ! -d ${imgDir} ];then
		mkdir -p ${imgDir}
    fi

    ./script/stability2.sh -t  ${outDirData}/fitres/${invMass_var}.dat 
#		--outDirImgData ${imgDir} -x $xVar -y peak $xMin $xMax || exit 1

fi


if [ -n "$ETA" ];then
    regionFile=data/regions/absEta.dat
    xVar=absEta
	if [ -z "${ONLYTABLE}" ];then
		./bin/ZFitter.exe --anyVar -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
			${extraOptions} \
			$updateOnly --commonCut=${commonCut} \
			--invMass_var ${invMass_var} --selection=${selection} \
			--invMass_min=${invMass_min} --invMass_max=${invMass_max} \
			--outDirFitResMC=${outDirMC}/fitres \
			--outDirImgMC=${outDirMC}/img  \
			--outDirFitResData=${outDirData}/fitres --outDirImgData=${outDirData}/img \
			> ${outDirData}/log/absEta.log || exit 1

		fi
	imgDir=${outDirData}/img/stability/$xVar/$PERIOD
    if [ ! -d ${imgDir} ];then
		mkdir -p ${imgDir}
    fi
	
    ./script/stability2.sh -t  ${outDirData}/fitres/${invMass_var}.dat --outDirImgData=$imgDir -x $xVar
    
fi    

if [ -n "$R9ELE" ];then
    regionFile=data/regions/R9Ele.dat
    xVar=R9Ele
    tableFile=${outDirTable}/R9Ele-${invMass_var}-${selection}-${commonCut}.tex
    if [ -z "${ONLYTABLE}" ];then
	./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
	    ${extraOptions} \
	    $updateOnly --invMass_var ${invMass_var} --commonCut=${commonCut} --selection=${selection} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/R9Ele.log || exit 1
    fi
    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	${tableCruijffOption} >  ${tableFile} || exit 1

    if [ ! -d ${outDirData}/img/stability/$xVar/$PERIOD ];then
	mkdir -p ${outDirData}/img/stability/$xVar/$PERIOD
    fi

    ./script/stability.sh -t ${tableFile} \
	--outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y peak $xMin $xMax || exit 1
    ./script/stability.sh -t ${tableFile} \
	--outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y scaledWidth $xMin $xMax || exit 1
    
fi    

if [ -n "${REFREG}" ];then
    regionFile=data/regions/refReg.dat
    tableFile=${outDirTable}/RefReg-${invMass_var}-${selection}-${commonCut}.tex
    if [ -z "${ONLYTABLE}" ];then
	./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
	    ${extraOptions} \
	    $updateOnly --invMass_var ${invMass_var} --commonCut=${commonCut} --selection=${selection} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/RefReg.log || exit 1
    fi
    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	${tableCruijffOption} >  ${tableFile} || exit 1
    
fi    


if [ -n "${TEST}" ];then
    regionFile=data/regions/test.dat
if [ -z "${ONLYTABLE}" ];then
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --invMass_var ${invMass_var} \
	${extraOptions} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	--commonCut=${commonCut}  --selection=${selection} --invMass_min=${invMass_min} --invMass_max=${invMass_max} \
	--outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/test.log|| exit 1
fi


    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres ${tableCruijffOption} \
	>  ${outDirTable}/$PERIOD/test-${invMass_var}-${selection}-${commonCut}.tex || exit 1
fi

if [ -n "$SYSTEMATICS" ];then
    #list of systematics for ECAL scale:
    # etaScale
    # time stability -> stability
    # pileup EB, pileup EE
    # 

#     regionFile=data/regions/test.dat
#     tableFile=${outDirTable}/alphaSM-${invMass_var}-${selection}.tex
#     if [ -z "${ONLYTABLE}" ];then
# 	./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
# 	    ${extraOptions} \
# 	    $updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} \
# 	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
# 	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/alphaSM.log || exit 1
#     fi
#     ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
# 	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres ${tableCruijffOption} \
# 	> ${tableFile}  || exit 1
#     exit 0

    regionFile=data/regions/systematics.dat
    tableFile=${outDirTable}/systematics-${invMass_var}-${selection}-${commonCut}.tex
    if [ -z "${ONLYTABLE}" ];then
	./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
	    ${extraOptions} \
	    $updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
	    --corrEleType=HggRunEtaR9Et \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/systematics.log || exit 1
    fi
    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres ${tableCruijffOption} \
	> ${tableFile}  || exit 1
    
    for xVar in nPV
      do
      if [ ! -e "${outDirData}/img/stability/$xVar/$PERIOD" ];then
	  mkdir ${outDirData}/img/stability/$xVar/$PERIOD/ -p
      fi
      ./script/stability.sh -t ${tableFile} \
	  --outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y peak --column 51 $xMin $xMax || exit 1
      ./script/stability.sh -t ${tableFile} \
	  --outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y scaledWidth --column 51 $xMin $xMax || exit 1
    done
    
    if [ "${SYSTEMATICS}" == "all" -o "${SYSTEMATICS}" == "fitMethod" ];then
	regionFile=data/regions/validation.dat
	
	mkdir ${outDirData}/{fitres,img}/{floating,invMass_range,invMass_range2, floating_invMass_range, floating_invMass_range2} -p
	mkdir ${outDirMC}/{fitres,img}/{invMass_range,invMass_range2,binning1} -p
	
	####### fit type =0: floating tails
	if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres/floating \
		--outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img/floating \
		--fit_type_value=0 > ${outDirData}/log/fit_systematics_floating.log
	fi
	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres/floating ${tableCruijffOption} \
	    >  ${outDirTable}/fit_systematics_floating-${invMass_var}-${selection}-${commonCut}.tex || exit 1
	
	####### fit mass range: [70-110]
	if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres/invMass_range --outDirFitResData=${outDirData}/fitres/invMass_range \
		--outDirImgMC=${outDirMC}/img/invMass_range    --outDirImgData=${outDirData}/img/invMass_range \
		--invMass_min=70 --invMass_max=110 > ${outDirData}/log/fit_systematics_invMass_range.log
	fi
	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres/invMass_range \
	    >  ${outDirTable}/fit_systematics_invMass_range-${invMass_var}-${selection}-${commonCut}.tex || exit 1
	
	####### fit mass range: [75-105]
	if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres/invMass_range_2 --outDirFitResData=${outDirData}/fitres/invMass_range2 \
		--outDirImgMC=${outDirMC}/img/invMass_range2    --outDirImgData=${outDirData}/img/invMass_range2 \
		--invMass_min=75 --invMass_max=105 > ${outDirData}/log/fit_systematics_invMass_range2.log
	fi
# 	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
# 	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres/invMass_range2 \
# 	    >  ${outDirTable}/fit_systematics_invMass_range2-${invMass_var}-${selection}.tex || exit 1

	####### fit mass range: [70-110]
	if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres/invMass_range \
		--outDirFitResData=${outDirData}/fitres/floating_invMass_range \
		--outDirImgMC=${outDirMC}/img/invMass_range \
		--outDirImgData=${outDirData}/img/floating_invMass_range \
		--fit_type_value=0 --invMass_min=70 --invMass_max=110 > ${outDirData}/log/fit_systematics_floating_invMass_range.log
	fi
	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
	    --outDirFitResMC=${outDirMC}/fitres/invMass_range --outDirFitResData=${outDirData}/fitres/invMass_range \
	    >  ${outDirTable}/fit_systematics_floating_invMass_range-${invMass_var}-${selection}-${commonCut}.tex || exit 1
	
	####### fit mass range: [75-105]
	if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres/invMass_range2 \
		--outDirFitResData=${outDirData}/fitres/floating_invMass_range2 \
		--outDirImgMC=${outDirMC}/img/invMass_range2 \
		--outDirImgData=${outDirData}/img/floating_invMass_range2 \
		--fit_type_value=0 --invMass_min=75 --invMass_max=105 > ${outDirData}/log/fit_systematics_floating_invMass_range2.log
	fi
# 	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
# 	    --outDirFitResMC=${outDirMC}/fitres/invMass_range2 --outDirFitResData=${outDirData}/fitres/invMass_range2 \
# 	    >  ${outDirTable}/fit_systematics_floating_invMass_range2-${invMass_var}-${selection}.tex || exit 1
	
	####### binning1: 0.75
	#if [ -z "${ONLYTABLE}" ];then
	    ./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
		${extraOptions} \
		$updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
		--outDirFitResMC=${outDirMC}/fitres/binning1 \
		--outDirFitResData=${outDirData}/fitres/binning1 \
		--outDirImgMC=${outDirMC}/img/binning1 \
		--outDirImgData=${outDirData}/img/binning1 \
		--fit_type_value=0 --invMass_binWidth=0.75 > ${outDirData}/log/fit_systematics_binning1.log
	#fi
	./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
	    --outDirFitResMC=${outDirMC}/fitres/binning1 --outDirFitResData=${outDirData}/fitres/binning1 \
	    >  ${outDirTable}/fit_systematics_binning1-${invMass_var}-${selection}.tex || exit 1

	######## the same with the cruijff function
	if [ -z "${signalType}" ]; then
	    ./script/monitoring_validation.sh $@ --cruijff --validation
	fi
    fi
fi    

if [ -n "$SYSTEMATICSD" ];then
    xVar=invMassRelSigma_SC_regrCorr_ele
    regionFile=data/regions/invMassRelSigma_SC_regrCorr_ele.dat
    if [ ! -e "${outDirData}/img/stability/$xVar/$PERIOD/" ];then
	mkdir ${outDirData}/img/stability/$xVar/$PERIOD/ -p
    fi
    if [ -z "${ONLYTABLE}" ];then
	./bin/ZFitter.exe ${otherOptions} -f ${configFile} --regionsFile ${regionFile}  \
	    ${extraOptions} \
	    $updateOnly --invMass_var ${invMass_var} 	--commonCut=${commonCut} --selection=${selection} \
	    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/systematics.log || exit 1
    fi
    tableFile=${outDirTable}/systematics-${invMass_var}-${selection}.tex
    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} ${tableCruijffOption} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	>  ${tableFile} || exit 1

    ./script/stability.sh -t ${tableFile} \
	--outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y peak $xMin $xMax || exit 1
    ./script/stability.sh -t ${tableFile} \
	--outDirImgData ${outDirData}/img/stability/$xVar/$PERIOD/ -x $xVar -y scaledWidth $xMin $xMax || exit 1

fi    

if [ -n "$SLIDES" ];then
    echo "[STATUS] Making slides"
    dirData=`dirname $outDirData` # remove the invariant mass subdir
    dirData=`dirname $dirData` # remove the selection subdir
    dirMC=`dirname $outDirMC` # remove the invariant mass subdir
    dirMC=`dirname $dirMC` # remove the selection subdir


    if [ -z "${rereco}" ];then
	./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation $period
    else
	./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type rereco --tag=$TAG $period #--rereco ${rereco}
    fi

fi

