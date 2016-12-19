#!/bin/bash
column_peak=3
column_resolution=4
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



regions=`cat $regionsFile | grep -v '#'`
if [ -n "${runRangesFile}" ];then
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

for branch in $outDirFitResData/*.dat
do
	fileData=$branch
	branch=`basename $branch .dat`
	fileMC=$outDirFitResMC/$branch.dat
	if [ ! -r "${fileMC}" ];then
		echo "[ERROR] ${fileMC} not found" >> /dev/stderr
		exit 1
	fi

	paste $fileData $fileMC | awk '(NF!=0){printf("%s\t%s", $1, $2); for(i=3; i<=NF/2;i++){printf("\t%s\t%s", $i, $(i+NF/2))};printf("\n")}'
	
done
