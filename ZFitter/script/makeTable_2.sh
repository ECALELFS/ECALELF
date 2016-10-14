#!/bin/bash
usage(){
    echo "`basename $0`"
    echo " --help"
    echo " ------- Mandatory"
    echo " --regionsFile arg"
    echo " --outDirFitResMC arg"
    echo " --outDirFitResData arg"
    echo " ------- Optional"
    echo " --runRangesFile arg"
    echo " --commonCut arg"
    echo " --cruijff"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,regionsFile:,runRangesFile:,outDirFitResMC:,outDirFitResData:,commonCut:,cruijff,selEff: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	--regionsFile) regionsFile=$2; shift;;
	--runRangesFile) runRangesFile=$2; shift;;
	--commonCut) commonCut=$2; shift;;
	--outDirFitResMC) outDirFitResMC=$2; shift;;
	--outDirFitResData) outDirFitResData=$2; shift;;
	-s|--step) STEP=$2; shift;;
	--cruijff) varVar=cruijff;;
	--selEff) SELEFF=y;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


#------------------------------ check mandatory options
if [ -z "${regionsFile}" ];then
    exit 1
fi
if [ ! -r "${regionsFile}" ];then
    exit 1
fi

if [ -z "${outDirFitResData}" -o -z "${outDirFitResMC}" ];then
    echo "[ERROR] outDirFitResData or outDirFitResMC not defined"
    exit 1
fi
if [ ! -d "${outDirFitResData}" -o ! -d "${outDirFitResMC}" ];then
    echo "[ERROR] outDirFitResData or outDirFitResMC not found or not directories"
    exit 1
fi


TYPE=0
case $TYPE in
    0)
	# scale
	;;
    1)
	#sigmaCB
	;;
    2)
	#sigmaCB/peakCB
	;;
    *)
	exit 1
	;;
esac
regions=`cat $regionsFile | grep -v '#'`
if [ -n "${runRangesFile}" ];then
#if [ -s "${runRangesFile}" ];then # -s means "size not zero"
    runRanges=`cat $runRangesFile | grep -v '#' | awk '{print $1}'`
    if [ -n "$JSON" ];then
	./script/selEff.sh --runRangesFile=$runRangesFile --json=$JSON
    fi
    for region in $regions
    do
	for runRange in $runRanges
	do
	    runRange=`echo $runRange | sed 's|-|_|'`
	    category=${region}"-runNumber_"${runRange}"-"$commonCut
	    categories="${categories} $category"
	done
    done
else
    for region in $regions
    do
	category=${region}"-"$commonCut
	categories="${categories} $category"
    done
fi

echo "#category & events & DeltaM_data & DeltaM_MC & DeltaP & width_data & width_MC & rescaledWidth_data & rescaledWidth_MC & additionalSmearing & chi2data & chi2mc & events/lumi"
for category in $categories
  do
  case $category in 
      *runNumber*)
	  runrange=`echo $category | sed 's|.*runNumber|runNumber|;s|.*runNumber_\([0-9]*\)_\([0-9]*\).*|\1_\2|'`
	  ;;
      *)
	  unset runrange
	  ;;
  esac
  if [ -n "${runrange}" ];then
      runrange="-runNumber_${runrange}"
      runMin=`echo ${runrange} | cut -d '_' -f 2`
      runMax=`echo ${runrange} | cut -d '_' -f 3`
      #for runDependent mc
     if [ "$runMin" -ge "190456" -a "$runMin" -le "196531" -a "$runMax" -le "198115" ];then runrange="-runNumber_194533_194533"; 
     elif [ "$runMin" -ge "198111" -a "$runMin" -le "203742" -a "$runMax" -le "203852" ];then runrange="-runNumber_200519_200519";
     elif [ "$runMin" -ge "203756" -a "$runMin" -le "208686" ];then runrange="-runNumber_206859_206859";
     else runrange="";
     fi
  fi
  categoryMC=`echo $category | sed "s|-runNumber_[0-9]*_[0-9]*|${runrange}|"`
  fileMC="${outDirFitResMC}/$categoryMC.tex"
  ##echo "###File MC is ${outDirFitResMC}/$categoryMC.tex"
  if [ ! -r "${fileMC}" ];then
      echo "[ERROR] ${fileMC} not found" >> /dev/stderr
      exit 1
  fi

  fileData="${outDirFitResData}/$category.tex"
  ##echo "###File Data is ${outDirFitResData}/$category.tex"
  if [ ! -r "${fileData}" ];then
      echo "[WARNING] ${fileData} not found: skipping this category"  >> /dev/stderr
      echo "%$category & not fitted \\\\"
      continue
  fi
  
  category=`echo $category | sed "s|-${commonCut}||"`
  events="`grep nEvents $fileData | cut -d '=' -f 2 | awk '{printf(\"%.0f\",$1)}'`"
  chi2_Data="`grep chi2 $fileData | cut -d '=' -f 2 | awk '{printf(\"%.2f\",$1)}'`"
  chi2_MC="`grep chi2 $fileMC | cut -d '=' -f 2 | awk '{printf(\"%.2f\",$1)}'`"
  if [ -n "${runRanges}" ];then
      runRange=`echo $category | sed 's|.*-runNumber_||;s|-.*||;s|_|-|'`
      lumi=`grep $runRange $runRangesFile | awk '{if(NF>3){ print $4} else{ print "0"};}'`
      selEff=`echo $events $lumi | awk '{if($2>0){ printf("%.4f \\\pm 0",$1/$2)} else{ print "--"};}'`
#      selEff=0
 #     echo $runRange $runRangesFile 
      #echo $events $lumi 
  else
      selEff="0"
  fi
## DeltaM
  var=Delta
  if [ -n "${varVar}" ];then
      var="M_{Z}"
      M_Data="`grep ${var} $fileData | cut -d '&' -f 2 | sed 's|GeV.*|$|' | sed 's|\\$||g;s|\\\\pm||g;s|&||g'`"
      M_MC="`grep ${var} $fileMC | cut -d '&' -f 2 | sed 's|GeV.*|$|'     | sed 's|\\$||g;s|\\\\pm||g;s|&||g'`"
      deltaM_Data=`echo $M_Data | awk '{printf("$ %.6f \\\pm %.6f $", $1-91.188, $2)}'`
      deltaM_MC=`echo $M_MC | awk '{printf("$ %.6f \\\pm %.6f $", $1-91.188, $2)}'`
  else
      deltaM_Data="`grep ${var} $fileData | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      deltaM_MC="`grep ${var} $fileMC | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
  fi
#old definition  
  deltaP=`echo "${deltaM_Data} ${deltaM_MC}" | sed 's|\\$||g;s|\\\\pm||g;s|&||g' | awk '{print "$",(\$1-\$3)/91.188*100,"\\\\pm",sqrt($2 * $2 + $4 * $4)/91.188*100,"$"}'`
#   deltaP=`echo "${deltaM_Data} ${deltaM_MC}" | sed 's|\\$||g;s|\\\\pm||g;s|&||g' | awk '{print "$",(\$1-\$3)/\$3*100,"\\\\pm",sqrt($2 * $2 + $4 * $4)/91.188*100,"$"}'`
# definition linear with injected variable 
#  deltaP=`"${deltaM_Data} ${deltaM_MC}" | sed 's|\\$||g;s|\\\\pm||g;s|&||g' | awk '{print "$",(\$1/\$3 -1) *100,"\\\\pm",sqrt($2 * $2 + $4 * $4 * ($1*$1)/($3*$3) )/$3 *100,"$"}'`


## Width
  var=sigma
  if [ -n "${varVar}" ];then
      var="sigma_{L}"
      width_Data_L="`grep ${var} $fileData | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      width_MC_L="`grep ${var} $fileMC | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      var="sigma_{R}"
      width_Data_R="`grep ${var} $fileData | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      width_MC_R="`grep ${var} $fileMC | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      
      width_Data="`echo ${width_Data_L} ${width_Data_R} | sed 's|\\$||g;s|\\\pm||g' | awk '{sigma_L=$1; errSigma_L=$2; sigma_R=$3; errSigma_R=$4; printf(\"$%.6f \\\pm %.6f $\", 0.5*(sigma_L+sigma_R), 0.5* sqrt(errSigma_L * errSigma_L + errSigma_R * errSigma_R ))}'`"
      width_MC="`echo ${width_MC_L} ${width_MC_R} | sed 's|\\$||g;s|\\\pm||g' | awk '{sigma_L=$1; errSigma_L=$2; sigma_R=$3; errSigma_R=$4; printf(\"$%.6f \\\pm %.6f $\", 0.5*(sigma_L+sigma_R), 0.5* sqrt(errSigma_L * errSigma_L + errSigma_R * errSigma_R ))}'`"
  else
      width_Data="`grep ${var} $fileData | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
      width_MC="`grep ${var} $fileMC | cut -d '&' -f 2 | sed 's|GeV.*|$|'`"
  fi

## rescaledWidth
  resolutionLine=`echo "$category events $deltaM_Data $deltaM_MC $deltaP $width_Data $width_MC xxx xxx" |sed 's|\\$||g;s|\\\pm||g;s|&||g' | awk -f awk/newResolution.awk`
#  echo $resolutionLine
  line="$category & $events & $deltaM_Data & $deltaM_MC & $deltaP & $width_Data & $width_MC $resolutionLine & \$ $chi2_Data \$ & \$ $chi2_MC \$ & \$ $selEff \$\\\\"
  echo $line 
 
# mettere la formattazione qui

  
done
