#!/bin/bash
##! Script for filling the luminosity per run range in the runRangesFile

usage(){
    echo "`basename $0` [options]" 
    echo "----- mandatory"
    echo " --runRangesFile arg:  run ranges for stability plots"
    echo " --json arg: json file for lumiCalc"
}

desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o h -l help,runRangesFile:,json: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--json) JSON=$2; shift;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

runRanges=`cat $runRangesFile |sed -r 's|[ ]+|\t|g' | cut -f 1`
#JSON=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt
cp $runRangesFile tmp/`basename $runRangesFile`
for runRange in $runRanges
  do
  runMin=`echo $runRange | cut -d '-' -f 1`
  runMax=`echo $runRange | cut -d '-' -f 2`
#  echo $runRange $runMin $runMax
#  filterJSON.py --min=$runMin --max=$runMax $JSON > tmp/json.txt || exit 1
#  lumiCalc2.py -i tmp/json.txt --nowarning -o tmp/$runRange.lumi overview 
  lumi=`cat tmp/$runRange.lumi | sed 's|.*\][\"]*,||' | awk '{sum+=$1};END{print sum/1e6}'`
  sed -i "/$runRange/ {s|$|\t$lumi|}" tmp/`basename $runRangesFile`
done

#!/bin/bash
##! Script for filling the luminosity per run range in the runRangesFile

usage(){
    echo "`basename $0` [options]" 
    echo "----- mandatory"
    echo " --runRangesFile arg:  run ranges for stability plots"
    echo " --json arg: json file for lumiCalc"
}

desc(){
    echo "#####################################################################"
    echo "## This script make the usual validation table and stability plots"
    echo "## "
    echo "#####################################################################"
}

#------------------------------ parsing


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o h -l help,runRangesFile:,json: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi


set -- $options

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) desc;usage; exit 0;;
	--runRangesFile) runRangesFile=$2; echo "[OPTION] runRangesFile = ${runRangesFile}"; shift;;
	--json) JSON=$2; shift;;
        (--) shift; break;;
        (-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
        (*) break;;
    esac
    shift
done

runRanges=`cat $runRangesFile |sed -r 's|[ ]+|\t|g' | cut -f 1`
#JSON=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt
cp $runRangesFile tmp/`basename $runRangesFile`
for runRange in $runRanges
  do
  runMin=`echo $runRange | cut -d '-' -f 1`
  runMax=`echo $runRange | cut -d '-' -f 2`
#  echo $runRange $runMin $runMax
#  filterJSON.py --min=$runMin --max=$runMax $JSON > tmp/json.txt || exit 1
#  lumiCalc2.py -i tmp/json.txt --nowarning -o tmp/$runRange.lumi overview 
  lumi=`cat tmp/$runRange.lumi | sed 's|.*\][\"]*,||' | awk '{sum+=$1};END{print sum/1e6}'`
  sed -i "/$runRange/ {s|$|\t$lumi|}" tmp/`basename $runRangesFile`
done

