#!/bin/bash

#tag_name=""
commonCut="Et_25"
#selection="cutBasedElectronId|Spring15|25ns|V1|standalone|loose"
selection="fiducial"
invMass_var=invMass_SC_must_regrCorr_ele
configFile=data/validation/EoverPcalibration_2016.dat #monitoring_2012_53X.dat 
FLOATTAILSTEXT="Fixed Tail" 
runRangesFile=data/runRanges/monitoring.dat 
baseDir=test
xVar=runNumber
updateOnly="--updateOnly"
GITTAG="lcorpe:topic-quickrereco-lcsh-fix-rebase" #should eventually get this automatically from file or option
GLOBALTAG="74X-lcdataRun2-lcPrompt-lcv0" #should eventually get this automatically from file or option
imgFormat="eps"

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
    echo " --ref compare to a reference rereco            "
    echo " --noPU            "
    echo " --floatTails            "
    echo " --baseDir arg (=${baseDir})  base directory for outputs"
    echo " --rereco arg  name of the tag used fro the rereco"
    echo " --imgFormat arg eps or pdf"
		echo " --dryRun make all the directories but don't run fits"
}
desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf: -l help,runRangesFile:,selection:,invMass_var:,puName:,dataName:,baseDir:,rereco:,validation,stability,slides,noPU,floatTails,dryRun,imgFormat:,ref: -- "$@")
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
	--ref) REF=$2; shift;;
	--dataName) dataName=$2; shift;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--baseDir) baseDir=$2; echo "[OPTION] baseDir = $baseDir"; shift;;
	--imgFormat) imgFormat=$2; echo "[OPTION] imgFormat = $imgFormat"; shift;;
	--rereco) rereco=$2; echo "[OPTION] rereco = $rereco"; shift;;
	--validation) VALIDATION=y;;
	--stability)  STABILITY=y;;
	--slides)     SLIDES=y;;
	--noPU)     NOPUOPT="--noPU"; NOPU="noPU"; puName="noPU";;
	--floatTails)    FLOATTAILSOPT="--fit_type_value=0"; FLOATTAILS="floatingTail"; FLOATTAILSTEXT="Floating Tail";;
	--dryRun)     DRYRUN="1";;
	--selection) selection=$2; echo "test selection" $selection ; shift;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

if [ -n "$NOPU" ];then
  baseDir="$baseDir-$NOPU" 
fi
#if [ -n "$FLOATTAILS" ];then
#  baseDir="$baseDir-$FLOATTAILS" 
#fi


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

#case ${selection} in
#    WP80_PU)
#        ;;
#    WP90_PU)
#	;;
#    *)
#			exit 1
#        ;;
#esac



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

#if [ -z "$FLOATTAILSOPT" ] ;then cp $REF/MC/DYJets_madgraph-RunIISpring15DR74-Asym25ns-allRange/pufile_tot/$selection/${invMass_var}/fitres/* ${outDirData}/fitres/. || exit 1 fi
if [[ -n "$REF" ]] && [[ -z "$FLOATTAILSOPT" ]]; then
refDir=$REF/dato/$REF/${selection}/${invMass_var}
count=`ls -l $refDir/fitres | wc -l`
echo $count
if (( $count == 0 )); then
 ls -A $refDir/fitres
echo "[ERROR] specified Ref does not have anything in $refDir/fitres! exit!"
exit 1
fi
outDirMC=${refDir}
fi
# cp $REF/MC/DYJets_madgraph-RunIISpring15DR74-Asym25ns-allRange/pufile_tot/$selection/${invMass_var}/fitres/* ${outDirMC}/fitres/. 
if [ -n "$DRYRUN" ]; then exit 0; fi

# keep track of the MC used to take the tail parameter for data
echo "$outDirMC" > $outDirData/whichMC.txt

if [ -n "$VALIDATION" ];then
    mcSample=${mcName}
    dataSample=${dataName}
    regionFile=data/regions/validation.dat
#	regionFile=data/regions/validation_0T.dat

    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  $updateOnly --invMass_var ${invMass_var} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	--outDirImgMC=${outDirMC}/img   --outDirImgData=${outDirData}/img  --commonCut=${commonCut}  --imgFormat=${imgFormat} --selection=${selection}  $NOPUOPT $FLOATTAILSOPT || exit 1 #> ${outDirData}/log/validation.log||  exit 1


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
    --outDirImgMC=${outDirMC}/img    --outDirImgData=${outDirData}/img --commonCut=${commonCut}  \
		--imgFormat $imgFormat --selection $selection|| exit 1 #> ${outDirData}/log/stability.log || exit 1
echo " 
    ./bin/ZFitter.exe -f ${configFile} --regionsFile ${regionFile}  --invMass_var ${invMass_var} \
	--outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
	--outDirImgMC=${outDirMC}/img   --outDirImgData=${outDirData}/img  --commonCut $commonCut $NOPUOPT $FLOATTAILSOPT --imgFormat $imgFormat > ${outDirData}/log/validation.log||  exit 1 "
echo "making table"
echo "
./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
    >  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex || exit 1"

./script/makeTable.sh --regionsFile ${regionFile}  --runRangesFile ${runRangesFile} --commonCut=${commonCut} \
    --outDirFitResMC=${outDirMC}/fitres --outDirFitResData=${outDirData}/fitres \
    >  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex || exit 1

echo "made table"


###################### Make stability plots
if [ ! -d ${outDirData}/img/stability/$xVar ];then
    mkdir -p ${outDirData}/img/stability/$xVar
fi

echo "Make stability plots"

./script/stability.sh -t  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex \
    --outDirImgData ${outDirData}/img/stability/$xVar/ -x $xVar -y peak || exit 1
./script/stability.sh -t  ${outDirTable}/monitoring_stability-${invMass_var}-${selection}.tex \
    --outDirImgData ${outDirData}/img/stability/$xVar/ -x $xVar -y scaledWidth || exit 1
fi

if [ $USER = "lcorpe" ]; then
#WEBDIR=/afs/cern.ch/user/l/lcorpe/www/MoCa/stability/$baseDir
WEBDIR=/afs/cern.ch/user/l/lcorpe/www/MoCa/validation/$baseDir
mkdir -p $WEBDIR/slides
mkdir -p $WEBDIR/stability
cp ${outDirData}/img/stability/$xVar/*eps $WEBDIR/stability/.
cd $WEBDIR/stability/.
echo $PWD
for i in *.eps ; do convert "$i" "${i%.*}.pdf" ; done
for i in *.pdf ; do convert "$i" "${i%.*}.png" ; done
cp /afs/cern.ch/user/l/lcorpe/public/index.php .
cd -

echo "[INFO] Stability plots at https://lcorpe.web.cern.ch/lcorpe/MoCa/stability/$baseDir"  
fi


if [ -n "$SLIDES" ];then
    echo "[STATUS] Making slides"
    dirData=`dirname $outDirData` # remove the invariant mass subdir
    dirData=`dirname $dirData` # remove the selection subdir
    dirMC=`dirname $outDirMC` # remove the invariant mass subdir
    dirMC=`dirname $dirMC` # remove the selection subdir
    for file in ${dirData}/${selection}/${invMass_var}/img/*.pdf;
    do
	echo "${file%????}.pdf" #remove four last characters ".eps" by ".pdf"
	#if [ ! -e "${file%????}.png" ];then
	    echo "processing Data $file" && convert "$file" "${file%.*}.png"
	   # echo "processing Data $file" && convert "$file" "${file%.*}.pdf"
#	fi;
    done
   
    
    echo "./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation"
    ./script/makeValidationSlides.sh --dirData ${dirData} --selection ${selection} --invMass_var ${invMass_var} --dirMC ${dirMC} --commonCut $commonCut --type validation
		mkdir $baseDir/slides
	
	if [ $USER == "lcorpe" -o $USER == "lbrianza" ];then

#WEBDIR=/afs/cern.ch/user/l/lcorpe/www/MoCa/stability/$baseDir
WEBDIR=/afs/cern.ch/user/l/lcorpe/www/MoCa/validation/$baseDir
mkdir -p $WEBDIR/slides
mkdir -p $WEBDIR/stability

	#for Louie and Luca, lazy: automatically replace variable names and compile latex...
	cat $configFile | grep "s1 " | grep -v "#" >tmp/mc.txt
	cat $configFile | grep d1 >tmp/data.txt
	sed -e "s|_|-lc|g" tmp/data.txt >tmp/data.1.txt
	sed -e "s|\/|!|g" tmp/data.1.txt >tmp/data.txt
	sed -e "s|\/|!|g" tmp/mc.txt >tmp/mc.1.txt
	sed -e "s|_|-lc|g" tmp/mc.1.txt >tmp/mc.txt
	#cp tmp/data.1.txt tmp/data.txt 
	#cp tmp/mc.1.txt tmp/mc.txt 
	#rerecoTag=`cut -d "!" -f16 tmp/data.txt`
	#runRange=`cut -d "!" -f18 tmp/data.txt`
	#json=`cut -d "!" -f19 tmp/data.txt`
	#dataNtuples=`cut -d ":" -f2 tmp/data.txt`
	#mcNtuples=`cut -d ":" -f2 tmp/mc.txt`
	echo $CMSSW_VERSION >  tmp/out.txt
	#cmssw_version=`sed "s|_|-lc|g" tmp/out.txt`
	echo $invMass_var >  tmp/out.txt
	#invmass_var=`sed "s|_|-lc|g" tmp/out.txt`
	echo $invmass_var
	echo "$rerecoTag "
	echo "$runRange "
	echo "$json "
	echo "$dataNtuples "
	echo "$mcNtuples "
	echo "$cmssw_version "
	echo " -e 's/0MCNtuples/$mcNtuples/g' \ "

#	-e "s/0runRange/$runRange/g" \
#	-e "s/0globalTag/$GLOBALTAG/g" \
#	-e "s/0rerecoTag/$rerecoTag /g" \
#	-e "s/0cmssw/$cmssw_version/g" \
#	-e "s/0gitTag/$GITTAG/g" \
#	-e "s/0dataNtuples/$dataNtuples/g" \

  sed -e "s/0intLumi/$intLumi (\/pb)/g" \
	-e "s/0MCNtuples/$mcNtuples/g" \
	-e "s/0floatingTail/$FLOATTAILSTEXT/g" \
	-e "s/std. SC energy/$rerecoTag $invmass_var/g" \
  -e "s/0puReweight/Reweight using nPV distributions in data and MC/g" \
	-e "s/0json/$json/g" tmp/validation-${invMass_var}-slides.tex > tmp/tmp.tex 
	
	
	sed -e "s|!|\/|g" tmp/tmp.tex >tmp/tmp.1.tex
	sed -e "s|13TeV|13TeV |g" tmp/tmp.1.tex >tmp/tmp.2.tex
	sed -e "s|-lc|\\\_|g" tmp/tmp.2.tex > tmp/validation-${invMass_var}-slides.tex
   	rm tmp/tmp.tex
    pdflatex -interaction nonstopmode -file-line-error tmp/validation-${invMass_var}-slides.tex
    cp tmp/validation-${invMass_var}-*.tex $baseDir/slides/. 
    cp tmp/validation-${invMass_var}-*.tex $WEBDIR/slides/. 
    cp validation-${invMass_var}-slides.pdf  $baseDir/slides/validation-${invMass_var}-slides-$rerecoTag.pdf
		cp $baseDir/slides/validation-${invMass_var}-slides-$rerecoTag.pdf $WEBDIR/slides/.
    
		mkdir -p $WEBDIR/$selection/validation-fits/
		echo "[ INFO ] making $WEBDIR/$selection/validation-fits/ " 
		cp  ${outDirData}/img/*{png,pdf} $WEBDIR/$selection/validation-fits/.
		cp ~lcorpe/public/index.php $WEBDIR/$selection/validation-fits/.

		cp ${outDirTable}/monitoring_summary-${invMass_var}-${selection}.tex $WEBDIR/slides/.

		##compare vs reference colelction
		mkdir $WEBDIR/comparison-vs-ref
		./script/compareRereco.sh $REF/dato/$REF/${selection}/invMass_rawSC/table/monitoring_summary-invMass_rawSC-${selection}.tex  ${outDirTable}/monitoring_summary-${invMass_var}-${selection}.tex

		cp comparison/comparison.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_$baseDir.tex
		cp comparison.pdf $WEBDIR/comparison-vs-ref/comparison_rev_vs_$baseDir.pdf
		cp $REF/dato/$REF/${selection}/invMass_rawSC/table/monitoring_summary-invMass_rawSC-${selection}.tex $WEBDIR/comparison-vs-ref/reference-table.tex
		cp ${outDirTable}/monitoring_summary-${invMass_var}-${selection}.tex $WEBDIR/comparison-vs-ref/$baseDir-table.tex
		cp comparison/slide3.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_deltaM.tex
		cp comparison/slide5.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_deltaP.tex
		cp comparison/slide6.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_sigmaCB.tex
		cp comparison/slide8.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_resol.tex
		cp comparison/slide10.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_addSmearing.tex
		cp comparison/slide11.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_chi2.tex
		cp comparison/slide15.tex $WEBDIR/comparison-vs-ref/comparison_rev_vs_sigmaEff.tex
		cp comparison/slide2.tex $WEBDIR/comparison-vs-ref/comparison_nevents.tex

		cp $WEBDIR/comparison-vs-ref/*tex $baseDir/slides/.

		echo "$baseDir/slides/validation-${invMass_var}-slides-$rerecoTag.pdf"
		echo "Slides at $WEBDIR/slides/."
		echo "on web at:  https://lcorpe.web.cern.ch/lcorpe/MoCa/validation/$baseDir"
	fi
		
fi

