#!/bin/bash

#tag_name=""
commonCut=Et_25
selection=WP80_PU
invMass_var=invMass_SC_regrCorr_ele
configFile=data/validation/monitoring_2012_53X.dat

runRangesFile=data/runRanges/monitoring.dat
baseDir=test
xVar=runNumber
updateOnly="--updateOnly"

# VALIDATION=y
# STABILITY=y
# SLIDES=y

usage(){
    echo "`basename $0` [options]" 
    echo "----- optional paramters"
    echo " -f arg (=${configFile})"
#    echo " --puName arg             "
    echo " --runRangesFile arg (=${runRangesFile})  run ranges for stability plots"
    echo " --selection arg (=${selection})     "
    echo " --invMass_var arg (=${invMass_var})"
    echo " --validation        "
    echo " --stability         "
    echo " --slides            "
    echo " --baseDir arg (=${baseDir})  base directory for outputs"
    echo " --rereco arg  name of the tag used fro the rereco"
}

desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help,runRangesFile:,selection:,invMass_var:,puName:,baseDir:,rereco:,validation,stability,slides -- "$@")
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
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--rereco) rereco=$2; echo "[OPTION] rereco = $rereco"; shift;;
	--validation) VALIDATION=y;;
	--stability)  STABILITY=y;;
	--slides)     SLIDES=y;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

if [ -z "${VALIDATION}" -a -z "${STABILITY}" -a -z "${SLIDES}" ];then
    # if no option selected, run all the sequence
    VALIDATION=y
    STABILITY=y
    SLIDES=y
fi

if [ ! -d "tmp" ];then mkdir tmp; fi

# file with ntuples
if [ -z "${configFile}" ];then
    echo "[ERROR] configFile not specified" >> /dev/stderr
    exit 1
fi

case ${selection} in
    WP80_PU)
        ;;
    WP90_PU)
	;;
    *)
        exit 1
        ;;
esac



if [ ! -r "${configFile}" ];then
    echo "[ERROR] configFile ${configFile} not found or not readable" >> /dev/stderr
    exit 1
else

    ## pileup reweight name
    if [ -z "$puName" ];then
	puCount=`grep -v '#' ${configFile}  | grep  'pileupHist' | grep '^d' | sed -r 's| +|\t|g' | cut -f 3 |wc -l`
	if [ "${puCount}" != "1" ];then
	    echo "[ERROR] No or too mani pileupHist files for data"
	    exit 1
	fi
	puFile=`grep -v '#' ${configFile}  | grep  'pileupHist' | grep '^d' |  sed -r 's| +|\t|g' | cut -f 3`
	#echo $puFile
	puName=`basename $puFile .root | sed 's|\..*||'`
	#echo $puName
    fi

    ## MC name
    if [ -z "${mcName}" ];then
	mcCount=`grep -v '#' ${configFile}  | grep  'selected' | grep '^s' |  sed -r 's| +|\t|g' | cut -f 3 |wc -l`
	if [ "${mcCount}" != "1" ];then
	    echo "[ERROR] No or too mani MC files to extract the mcName"
	    exit 1
	fi
	mcFile=`grep -v '#' ${configFile}  | grep  'selected' | grep '^s' |  sed -r 's| +|\t|g' | cut -f 3`
	mcName=`basename $mcFile .root | sed 's|\..*||'`
#	echo $mcName
    fi
fi

if [ -n "${rereco}" ];then
    outDirData=$baseDir/data/rereco/${rereco}/`basename ${configFile} .dat`/${selection}/${invMass_var}
else
    outDirData=$baseDir/data/`basename ${configFile} .dat`/${selection}/${invMass_var}
fi
outDirMC=$baseDir/MC/${mcName}/${puName}/${selection}/${invMass_var}
outDirTable=${outDirData}/table


if [ ! -e "${outDirTable}/${selection}/${invMass_var}" ];then 
    mkdir -p ${outDirTable}/${selection}/${invMass_var}; 
fi


if [ ! -e "${outDirMC}/fitres" ];then mkdir ${outDirMC}/fitres -p; fi
if [ ! -e "${outDirMC}/img" ];then mkdir ${outDirMC}/img -p; fi
if [ ! -e "${outDirData}/table" ];then mkdir ${outDirData}/table -p; fi
if [ ! -e "${outDirData}/fitres" ];then mkdir ${outDirData}/fitres -p; fi
if [ ! -e "${outDirData}/img" ];then mkdir ${outDirData}/img -p; fi
if [ ! -e "${outDirData}/log" ];then mkdir ${outDirData}/log -p; fi

# keep track of the MC used to take the tail parameter for data
echo "$outDirMC" > $outDirData/whichMC.txt

if [ -n "$VALIDATION" ];then
#    regionFile=data/regions/validation.dat
#    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --invMass_var ${invMass_var} \
#	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
#	--outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/validation.log|| exit 1

    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	>  ${outDirTable}/monitoring_summary-${invMass_var}-${selection}.tex || exit 1
fi

##################################################
if [ -n "$STABILITY" ];then
regionFile=data/regions/stability.dat
#./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
#    $updateOnly --invMass_var ${invMass_var} \
#    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
#    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img > ${outDirData}/log/stability.log || exit 1

#./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
#    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
#    >  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex || exit 1



###################### Make stability plots
if [ ! -d ${outDirData}/img/stability/$xVar ];then
    mkdir -p ${outDirData}/img/stability/$xVar
fi

./script/stability.sh -t  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex \
    --outDirImgData ${outDirData}/img/stability/$xVar/ -x $xVar -y peak || exit 1
./script/stability.sh -t  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex \
    --outDirImgData ${outDirData}/img/stability/$xVar/ -x $xVar -y scaledWidth || exit 1
fi



if [ -n "$SLIDES" ];then
    echo "[STATUS] Making slides"
    dirData=`dirname $outDirData` # remove the invariant mass subdir
    dirData=`dirname $dirData` # remove the selection subdir
    dirMC=`dirname $outDirMC` # remove the invariant mass subdir
    dirMC=`dirname $dirMC` # remove the selection subdir
    
    ./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation
fi

