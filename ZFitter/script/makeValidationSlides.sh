#!/bin/bash
. script/slidesFunctions.sh
usage(){
    echo "`basename $0` [options]" 
    echo " [mandatory]"
    echo " --dirData arg    folder with fitres,img,step subfolder"
    echo " --dirMC arg"
    echo " --selection arg"
    echo " --invMass_var arg"
    echo " --type arg   type of slides:"
    echo "      scales, validation, rereco"
    echo " --tag arg"
    echo " --period RUN2012A, RUN2012AB, RUN2012C"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o h -l help,type:,dirData:,dirMC:,selection:,invMass_var:,commonCut:,tag:,period: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	--type) TYPE=$2; shift;;
	--dirData) dirData=$2; shift;;
	--dirMC)   dirMC=$2; shift;;
	--selection) selection=$2;shift;;
	--invMass_var) invMass_var=$2; shift;;
	--commonCut) commonCut=$2; shift;;
	--tag) TAG=$2; shift;;
	--period) PERIOD=$2; echo $PERIOD; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

if [ ! -d "tmp" ];then mkdir tmp; fi

if [ -z "${dirData}" -o -z "${dirMC}" ];then
    echo "[ERROR] dirData or dirMC mandatory parameter missing" >> /dev/stderr
    echo 
    usage >> /dev/stderr
    exit 1
fi

if [ ! -d "${dirData}" -o ! -d "${dirMC}" ];then
    echo "[ERROR] dirData or dirMC directory not found or not readable" >> /dev/stderr
    echo "  dirData = ${dirData}"
    echo "  dirMC   = ${dirMC}"
    echo 
    usage >> /dev/stderr
    exit 1
fi

if [ -z "${invMass_var}" ];then
    echo "[ERROR] Invariant mass branch not defined" >> /dev/stderr
    echo 
    usage >> /dev/stderr
    exit 1
fi

if [ -z "${selection}" ]; then
    echo "[ERROR] Selection not defined" >> /dev/stderr
    echo 
    usage >> /dev/stderr
    exit 1
fi

if [ ! -d "${dirData}/${selection}/${invMass_var}" -o ! -d "${dirMC}/${selection}/${invMass_var}" ];then
    echo "[ERROR] Directory ${dirData}/${selection}/${invMass_var} or " >> /dev/stderr
    echo "                  ${dirMC}/${selection}/${invMass_var} not found" >> /dev/stderr
    echo
    exit 1
fi


# define the name to give in the slides
case $invMass_var in
    invMass_SC)
	invMassVarName="std. SC energy"
	;;
    invMass_rawSC)
	invMassVarName="raw SC energy"
	;;
    invMass_SC_must)
	invMassVarName="regression SC energy"
	;;
    invMass_SC_must_regrCorr_ele)
	invMassVarName="regression SC energy (mustache)"
	;;
    invMass_rawSC_esSC)
	invMassVarName="raw and es SC energy"
	;;
    invMass_SC_corr)
	invMassVarName="corrected ECAL energy"
	;;
    invMass_SC_regrCorr_ele)
	invMassVarName="ele-tuned regression energy"
	;;
    invMass_SC_regrCorr_pho)
	invMassVarName="pho-tuned regression energy"
	;;
    invMass_regrCorr_fra)
	invMassVarName="ele-tuned regression energy (new)"
	;;
    *)
	echo "[ERROR] invMass_var type not known: ${invMass_var}" >> /dev/stderr
	echo
	exit 1
	;;
esac


dirSlides=tmp

case ${TYPE} in 
    scales)
	# check additional parameters for this type
	# check if all subfolders are present
	if [ ! -d "${dirData}/${selection}/${invMass_var}/fitres" -o ! -d "${dirData}/${selection}/${invMass_var}/step2" -o ! -d "${dirData}/${selection}/${invMass_var}/step3" ];then
	    echo "[ERROR] One or more missing subfolder for data: fitres, step2 or step3" >> /dev/stderr
	    echo 
	    exit 1
	fi


	############## STEP 1 slides
	step1File=${dirData}/${selection}/${invMass_var}/table/step1-${invMass_var}-${selection}-${commonCut}-HggRunEta.tex
	step2File=${dirData}/${selection}/${invMass_var}/table/step2-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	step3File=${dirData}/${selection}/${invMass_var}/table/step3-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	step3stability=${dirData}/${selection}/${invMass_var}/table/step3_stability-${invMass_var}-${selection}-${commonCut}-HggRunEtaR9.tex
	if [ ! -r "$step1File" ];then
	    echo "[ERROR] table file for step1: $step1File not found or not readable" >> /dev/stderr
	    echo
	    exit 1
	fi
	if [ ! -r "$step2File" ];then
	    echo "[ERROR] table file for step1: $step1File not found or not readable" >> /dev/stderr
	    echo
	    exit 1
	fi
	validationIntroSlide
	validationStabilitySlides
	step1Slides
	step2Slides
	step3Slides
	step3stabilitySlides
	;;
    validation)
	dirSlides=${dirSlides}/$PERIOD
	if [ ! -e "$dirSlides/" ];then mkdir -p ${dirSlides}; fi

	# check if all subfolders are present
	if [ ! -d "${dirData}/${selection}/${invMass_var}/fitres" ];then
	    echo "[ERROR] missing subfolder for data: fitres" >> /dev/stderr
	    echo 
	    exit 1
	fi
	$dataSample=2012D_75x_rereco
	$mcName=DYJetsToLL_M-50_3TeV_amcatnloFXFXpythia8_Asympt50ns
	validationIntroSlide
	validationTableSlides
	validationFitSlides
	validationBackupSlides
	validationStabilitySlides
	if [ "$USER" == "lcorpe" -o "$USER" ==  "lbrianza" ];then
	#cat tex/template.tex \
	cat ${dirSlides}/intro.tex \
			${dirSlides}/validation-${invMass_var}-${selection}-intro_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-table_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-fit_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-backup_slide.tex \
		 	tex/template_end.tex > ${dirSlides}/validation-${invMass_var}-slides.tex

		else

	cat ${dirSlides}/validation-${invMass_var}-${selection}-intro_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-table_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-fit_slide.tex > ${dirSlides}/validation-${invMass_var}-slides.tex


	fi

#	    ${dirSlides}/validation-${invMass_var}-${selection}-stability_slide.tex \ 
	;;
    rereco)
	rereco=$TAG
	dirSlides=${dirSlides}/${rereco}/$PERIOD
	if [ ! -e "$dirSlides" ];then mkdir -p ${dirSlides}; fi
#	if [ ! -r "../SANDBOX/config/reRecoTags/$TAG.py" ]; then
#	    echo "[ERROR] $TAG not found" >> /dev/stderr
#	    exit 1
#	fi
	echo $dirData/${selection}/${invMass_var}/table/$PERIOD/monitoring_summary-${invMass_var}-${selection}.tex
	validationIntroSlide
	validationTableSlides
	validationFitSlides
	validationStabilitySlides

	cat ${dirSlides}/validation-${invMass_var}-${selection}-intro_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-table_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-fit_slide.tex \
	    ${dirSlides}/validation-${invMass_var}-${selection}-stability_slide.tex \
	    > ${dirSlides}/validation-${invMass_var}-slides.tex

	    ;;
    *)
	echo "[ERROR] Slide type not recognized" >> /dev/stderr
	echo 
	usage >> /dev/stderr
	exit 1
	;;
esac



