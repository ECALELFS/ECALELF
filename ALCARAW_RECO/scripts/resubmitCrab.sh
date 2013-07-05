#!/bin/bash

usage(){
    echo "Usage: `basename $0` -u ui_working_dir"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hu:d: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
  do
  case $1 in
      -h) usage; exit 0;;
      -u) ui_working_dir=$2; shift;;
      -d) dataset=$2; shift ;;
      (--) shift; break;;
      (-*) echo "$0: error - unrecognized option $1" 1>&2; exit 1;;
      (*) break;;
  esac
  shift
done

if [ ! -d "$ui_working_dir" ];then
    echo "[ERROR] ui_working_dir $ui_working_dir not found or not readable" >> /dev/stderr
    exit 1
fi

if [ -e "$ui_working_dir/res/finished" ];then
    echo "[REPORT] Jobs successfully finished: $ui_working_dir" 
    exit 0
fi

if [ ! -e "$ui_working_dir/share/arguments.xml" ];then
    echo "[ERROR] $ui_working_dir not a crab task directory" >> /dev/stderr
    exit 1
fi
# check jobs status
#crab -c $ui_working_dir -status

# script in perl should be better
nJobs=`grep JobID $ui_working_dir/share/arguments.xml | wc -l`
#echo $nJobs
if [ "`crab -c $ui_working_dir/ -status | grep -c PEND`" != "0" ] ;then exit 0; fi
crab -c $ui_working_dir/ -status -getoutput | tee ${ui_working_dir}/res/status.log


for jobID in `seq 1 $nJobs`
  do
  #awk "(\$1==$jobID){print \$2}" ${ui_working_dir}/res/status.log
  status=`awk "(\\$1==$jobID){print \\$2}" ${ui_working_dir}/res/status.log`
  if [ "${status}" == "N" ];then 
      continue; 
  fi
  # se non c'e' deve essere running, accertarsene
  jobReport=`find $ui_working_dir/res/ -name crab_fjr_$jobID.xml | head -1`
  logReport=`find $ui_working_dir/res/ -name CMSSW_$jobID.stdout`

  if [ ! -e "$jobReport" ];then
      echo "-> Missing $jobReport"
      continue;
  fi


  if [ "`cat $jobReport |wc -l`" == "1" ];then
      continue
#      echo "Job finished but not retrieved"

#      crab -c $ui_working_dir/ -getoutput $jobID
  fi
  
#  wc -l $jobReport
#  echo $jobReport
#  cat $jobReport
  ExitCode=`grep ExitStatus $jobReport | sed 's|.*ExitStatus="||;s|".*||'`
  #LaserDBError=`grep EcalLaserDbService $ui_working_dir/res/CMSSW_1.stdout  | grep -v MSG | grep '\-e'`
#  if [ "$ExitCode" != "0 0" ];then
  case $ExitCode in 
      0*0)
	  if [ -z "${LaserDBError}" ];then
	      okJobList="${okJobList} $jobID"
	  else
	      okJobList="${okJobList} $jobID"
#	      echo "LaserDBError for job $jobID"
	  fi
#	  ./script/removeEmptyFiles.sh -u $ui_working_dir -i $jobID
	  ;;
      60307*0)
	  intervals="$intervals $jobID"
#	  crab -c $ui_working_dir -resubmit $jobID
	  ;;
      60317*0)
	  intervals="$intervals $jobID"
	  ;;
      0*60307)
	  intervals="$intervals $jobID"
#	  crab -c $ui_working_dir -resubmit $jobID
	  ;;
      60318*0)
	  intervals="$intervals $jobID"
	  ;;
      0*60318)
	  intervals="$intervals $jobID"
#	  crab -c $ui_working_dir -resubmit $jobID
	  ;;
      0*70500)
	  echo "[WARNING] Output file exceeded fixed size and splitted, but only one file copied to SE"
	  
	  ;;
      8020*8020)
	  echo "[ERROR] error 8020" 
	  intervals="$intervals $jobID"
#	  echo $jobID $ExitCode
	  ;;
      8021*8021)
	  echo "[ERROR] error 8020" 
#	  echo $jobID $ExitCode
	  intervals="$intervals $jobID"
	  ;;
      6*6)
	  intervals="$intervals $jobID"
	  ;;
      8001*8001)
	  echo $jobID $ExitCode
#	  intervals="$intervals $jobID"
	  ;;
      9*9)
	  intervals="$intervals $jobID"
	  ;;
      50660*143)
	  intervals="$intervals $jobID"
	  ;;
      *)
	  echo $jobID $ExitCode
	  

	  ;;
  esac

done


echo $intervals | sed 's| |,|'
if [ -n "$intervals" ];then
    crab -c $ui_working_dir/ -resubmit `echo $intervals | sed 's| |\n|g' | awk -f $CMSSW_BASE/src/calibration/ALCARAW_RECO/awk/compact.awk | sed 's|,$||'`
else
    if [ "`echo $okJobList |wc -w`" == "$nJobs" ];then
	echo "[STATUS] All jobs are finished SUCCESSFULLY!"
	touch $ui_working_dir/res/finished
    fi
fi




