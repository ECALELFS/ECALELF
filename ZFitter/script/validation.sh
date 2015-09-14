#!/bin/bash

#tag_name=""
commonCut=Et_25
selection=WP80_PU
invMass_var=invMass_SC_regrCorr_ele
configFile=data/validation/monitoring_2012_53X.dat 
FLOATTAILSTEXT="Fixed Tail" 
runRangesFile=data/runRanges/monitoring.dat 
baseDir=test
xVar=runNumber
pdateOnly="--updateOnly"
GITTAG="lcorpe:topic-quickrereco-lcsh-fix-rebase" #should eventually get this automatically from file or option
GLOBALTAG="74X-lcdataRun2-lcPrompt-lcv0" #should eventually get this automatically from file or option

# VALIDATION=y
# STABILITY=y
# SLIDES=y

usage(){
    echo "`basename $0` [options]" 
    echo "----- optional paramters"
    echo " -f arg (=${configFile})"
#    echo " --puName arg             "
    echo " --dataName arg             "
    echo " --runRangesFile arg (=${runRangesFile})  run ranges for stability plots"
    echo " --selection arg (=${selection})     "
    echo " --invMass_var arg (=${invMass_var})"
    echo " --validation        "
    echo " --stability         "
    echo " --slides            "
    echo " --noPU            "
    echo " --floatTails            "
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
if ! options=$(getopt -u -o hf: -l help,runRangesFile:,selection:,invMass_var:,puName:,dataName:,baseDir:,rereco:,validation,stability,slides,noPU,floatTails -- "$@")
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
	--dataName) dataName=$2; shift;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--rereco) rereco=$2; echo "[OPTION] rereco = $rereco"; shift;;
	--validation) VALIDATION=y;;
	--stability)  STABILITY=y;;
	--slides)     SLIDES=y;;
	--noPU)     NOPUOPT="--noPU"; NOPU="noPU";;
	--floatTails)    FLOATTAILSOPT="--fit_type_value=0"; FLOATTAILS="floatingTail"; FLOATTAILSTEXT="Floating Tail";;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

if [ -n "$NOPU" ];then
  baseDir="$baseDir-$NOPU" 
fi
if [ -n "$FLOATTAILS" ];then
  baseDir="$baseDir-$FLOATTAILS" 
fi


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
    outDirData=$baseDir/dato/rereco/${rereco}/`basename ${configFile} .dat`/${selection}/${invMass_var}
else
    outDirData=$baseDir/dato/`basename ${configFile} .dat`/${selection}/${invMass_var}
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
    mcSample=${mcName}
    dataSample=${dataName}
    regionFile=data/regions/validation.dat
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --invMass_var ${invMass_var} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	--outDirImgMC=${outDirMC}/img   --outDirImgData=${outDirData}/img  --commonCut $commonCut $NOPUOPT $FLOATTAILSOPT > ${outDirData}/log/validation.log||  exit 1



    ./script/makeTable.sh --regionsFile ${regionFile}  --commonCut=${commonCut} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	>  ${outDirTable}/monitoring_summary-${invMass_var}-${selection}.tex || exit 1
fi

##################################################
if [ -n "$STABILITY" ];then
regionFile=data/regions/stability.dat
./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} \
    $updateOnly --invMass_var ${invMass_var} \
    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img --commonCut=${commonCut}> ${outDirData}/log/stability.log || exit 1

./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
    >  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex || exit 1



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
    for file in ${dirData}/${selection}/${invMass_var}/img/*.eps;
    do
	echo "${file%????}.pdf" #remove four last characters ".eps" by ".pdf"
	if [ ! -e "${file%????}.pdf" ];then
	    echo "processing Data $file" && epstopdf $file
	fi;
    done
   
    
    echo "./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation"
    ./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation
		mkdir $baseDir/slides
	
	if [ $USER == "lcorpe" -o $USER == "lbrianza" ];then
	#for Louie and Luca, lazy: automatically replace variable names and compile latex...
	cat $configFile | grep s1 >tmp/mc.txt
	cat $configFile | grep d1 >tmp/data.txt
	sed -e "s|_|-lc|g" tmp/data.txt >tmp/data.1.txt
	sed -e "s|\/|!|g" tmp/data.1.txt >tmp/data.txt
	sed -e "s|\/|!|g" tmp/mc.txt >tmp/mc.1.txt
	sed -e "s|_|-lc|g" tmp/mc.1.txt >tmp/mc.txt
	#cp tmp/data.1.txt tmp/data.txt 
	#cp tmp/mc.1.txt tmp/mc.txt 
	rerecoTag=`cut -d "!" -f16 tmp/data.txt`
	runRange=`cut -d "!" -f18 tmp/data.txt`
	json=`cut -d "!" -f19 tmp/data.txt`
	dataNtuples=`cut -d ":" -f2 tmp/data.txt`
	mcNtuples=`cut -d ":" -f2 tmp/mc.txt`
	echo $CMSSW_VERSION >  tmp/out.txt
	cmssw_version=`sed "s|_|-lc|g" tmp/out.txt`
	echo $invMass_var >  tmp/out.txt
	invmass_var=`sed "s|_|-lc|g" tmp/out.txt`
	echo $invmass_var
	echo "$rerecoTag "
	echo "$runRange "
	echo "$json "
	echo "$dataNtuples "
	echo "$cmssw_version "
	

  sed -e "s/0intLumi/19.636 (\/pb)/g" \
	-e "s/0runRange/$runRange/g" \
	-e "s/0globalTag/$GLOBALTAG/g" \
	-e "s/0rerecoTag/$rerecoTag /g" \
	-e "s/0gitTag/$GITTAG/g" \
	-e "s/0cmssw/$cmssw_version/g" \
	-e "s/0dataNtuples/$dataNtuples/g" \
	-e "s/0MCNtuples/$mcNtuples/g" \
	-e "s/0floatingTail/$FLOATTAILSTEXT/g" \
	-e "s/std. SC energy/$rerecoTag $invmass_var/g" \
  -e "s/0puReweight/Reweight using nPV distributions in data and MC/g" \
	-e "s/0json/$json/g" tmp/validation-${invMass_var}-slides.tex > tmp/tmp.tex 
	
	sed -e "s|!|\/|g" tmp/tmp.tex >tmp/tmp.1.tex
	sed -e "s|13TeV|13TeV |g" tmp/tmp.1.tex >tmp/tmp.2.tex
	sed -e "s|-lc|\\\_|g" tmp/tmp.2.tex > tmp/validation-${invMass_var}-slides.tex
	#	rm tmp/tmp.tex
    pdflatex tmp/validation-${invMass_var}-slides.tex
    cp tmp/validation-${invMass_var}-*.tex $baseDir/slides/. 
    cp validation-${invMass_var}-slides.pdf  $baseDir/slides/validation-${invMass_var}-slides-$rerecoTag.pdf
	fi
		
fi

