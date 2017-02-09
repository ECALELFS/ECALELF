#!/bin/bash
# this script selects the one column for the peak estimator and one
# for the resolution estimator and returns a .dat and a .tex files
commonCut=isEle-Et_25
column_peak=4
column_resolution=5
var=invMass_ECAL_ele
usage(){
	echo "************************************************************"
    echo "* `basename $0`                                          "
	echo "************************************************************"
    echo "* --help: return this message                               "
    echo "* ***** Mandatory ------------------------------------------"
    echo "* --regionsFile <arg>      : file with the regions to be reported in table"
    echo "* --fitResFile <arg> (=$fitResFile): .dat file with the fit results"
    echo "* ***** Optional -------------------------------------------"
    echo "* --runRangesFile <arg>                                     "
    echo "* --commonCut <arg>     (=$commonCut)"
	echo "* --peakVar <arg>       (=$column_peak)       : column containing the peak estimator"
	echo "* --resolutionVar <arg> (=$column_resolution) : column containing the resolution estimator"
	echo "************************************************************"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hf:s: -l help,regionsFile:,runRangesFile:,fitResFile:,commonCut:,selEff:,peakVar:,resolutionVar: -- "$@")
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
		--fitResFile) fitResFile=$2; shift;;
		--peakVar)    column_peak=$2; shift;;
		--resolutionVar) column_resolution=$2; shift;;
		-s|--step) STEP=$2; shift;;
		#	--cruijff) varVar=cruijff;;
		--selEff) SELEFF=y;;
		(--) shift; break;;
		(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
		(*) break;;
    esac
    shift
done

columns="1,3,${column_peak},${column_resolution}"
#------------------------------ check mandatory options
if [ -z "${regionsFile}" ];then
    exit 1
fi
if [ ! -r "${regionsFile}" ];then
    exit 1
fi

if [ ! -r "${fitResFile}" ];then
    exit 1
fi

############################## Start getting the list of regions
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


# taking the wanted lines and header and parsing the output to the wanted format
# prints only the columns wanted and transfor .dat in .tex removing the commonCut in the category names
{
	# get header
	head -1 $fitResFile
	# get results
	for category in $categories
	do
		grep $category $fitResFile
	done
} | cut -f $columns| sed 's|[\t ]| \& |g;s|$| \\\\|' |sed "s|[-]$commonCut||" | sed -f sed/tex.sed

exit 0


for branch in $outDirFitResData/$var.dat
do
	fileData=$branch
	branch=`basename $branch .dat`
#	echo "# data and MC are pasted side-by-side for each column"
	paste $fileData $fileMC | awk '(NF!=0){printf("%s\t%s", $1, $2); for(i=3; i<=NF/2;i++){printf("\t%s\t%s", $i, $(i+NF/2))};printf("\n")}' > tmp/makeTable2.dat


	columns="\$1, \$3,\$4, \$(($column_peak-1)*2+1), \$(($column_peak-1)*2+2), \$(($column_resolution-1)*2+1), \$(($column_resolution-1)*2+2)"

done
