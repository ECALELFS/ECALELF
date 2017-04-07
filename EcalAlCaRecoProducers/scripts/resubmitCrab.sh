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
    echo 
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

crab -c $ui_working_dir/ -status > ${ui_working_dir}/res/status.log
if [ "`cat ${ui_working_dir}/res/status.log | grep -c PEND`" != "0" ] ;then exit 0; fi
if [ "`cat ${ui_working_dir}/res/status.log | grep -c Terminated`" != "0" ] ;then 
    crab -c $ui_working_dir/ -getoutput 
    crab -c $ui_working_dir/ -status > ${ui_working_dir}/res/status.log
fi

cat ${ui_working_dir}/res/status.log

end=(`sed -r '/^[^0-9]+/ d;/^$/ d' ${ui_working_dir}/res/status.log | awk '{print $2}'`)
status=(`sed -r '/^[^0-9]+/ d;/^$/ d' ${ui_working_dir}/res/status.log | awk '{print $3}'`)
ExitStatusCMSSW=(`sed -r '/^[^0-9]+/ d;/^$/ d' ${ui_working_dir}/res/status.log | awk '{if(NF>=6){print $5} else print "-"}'`)
ExitStatusJOB=(`sed -r '/^[^0-9]+/ d;/^$/ d' ${ui_working_dir}/res/status.log | awk '{if(NF>=6){print $6} else print "-"}'`)

for jobID in `seq 1 $nJobs`
  do
  let jID=$jobID-1
  if [ "${end[${jID}]}" == "N" ];then 
      continue; 
  fi

  if [ "${status[${jID}]}" == "Running" -o "${status[${jID}]}" == "RUN" -o "${status[${jID}]}" == "Submitting" -o "${status[${jID}]}" == "Submitted" ]; then
      continue;
  fi

  if [ "${ExitStatusCMSSW[${jID}]}" == "0" -a "${ExitStatusJOB[${jID}]}" == "0" ];then
      okJobList="${okJobList} $jobID"
      continue;
  fi

  case ${ExitStatusJOB[${jID}]} in
      8028) intervals="$intervals $jobID";;
      8020) intervals="$intervals $jobID";;
      24) intervals="$intervals $jobID";;
      8021) intervals="$intervals $jobID";;
      50664) intervals="$intervals $jobID";;
	  50669) intervals="$intervals $jobID";;
      50700) intervals="$intervals $jobID";;
      50800) intervals="$intervals $jobID";;
      60303) intervals="$intervals $jobID";;
      60307) intervals="$intervals $jobID";;
      60317) intervals="$intervals $jobID";;
      60318) intervals="$intervals $jobID";;
	  50115) intervals="$intervals $jobID";;
	  50669) intervals="$intervals $jobID";;
	  50660) intervals="$intervals $jobID";;
	  10040) intervals="$intervals $jobID";;
  esac

#  echo ${intervals}
#   # se non c'e' deve essere running, accertarsene
#   jobReport=`find $ui_working_dir/res/ -name crab_fjr_$jobID.xml |sort -r | head -1`
#   logReport=`find $ui_working_dir/res/ -name CMSSW_$jobID.stdout |sort -r | head -1`

#   if [ ! -e "$jobReport" ];then
#       echo "-> Missing jobReport for job $jobID"
#       if [ "`grep -c \"Exited with exit code 2\" $logReport`" != "0" ];then
# 	  echo "[INFO] Problem with lxbatch node, resubmitting"
# 	  intervals="$intervals $jobID"
# 	  continue;
#       else
# 	  continue;
#       fi
#   fi


#   if [ "`cat $jobReport |wc -l`" == "1" ];then
#       continue
# #      echo "Job finished but not retrieved"

# #      crab -c $ui_working_dir/ -getoutput $jobID
#   fi
 


#   ExitCode=`grep ExitStatus $jobReport | sed 's|.*ExitStatus="||;s|".*||'`
#   case $ExitCode in 
#       0*0)
# 	  if [ -z "${LaserDBError}" ];then
# 	      okJobList="${okJobList} $jobID"
# 	  else
# 	      okJobList="${okJobList} $jobID"
# #	      echo "LaserDBError for job $jobID"
# 	  fi
# #	  ./script/removeEmptyFiles.sh -u $ui_working_dir -i $jobID
# 	  ;;
#       60307*0)
# 	  intervals="$intervals $jobID"
# #	  crab -c $ui_working_dir -resubmit $jobID
# 	  ;;
#       60317*0)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       0*60307)
# 	  intervals="$intervals $jobID"
# #	  crab -c $ui_working_dir -resubmit $jobID
# 	  ;;
#       60318*0)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       0*60318)
# 	  intervals="$intervals $jobID"
# #	  crab -c $ui_working_dir -resubmit $jobID
# 	  ;;
#       0*70500)
# 	  echo "[WARNING] Output file exceeded fixed size and splitted, but only one file copied to SE"
	  
# 	  ;;
#       8020*8020)
# 	  echo "[ERROR] error 8020" 
# 	  intervals="$intervals $jobID"
# #	  echo $jobID $ExitCode
# 	  ;;
#       8021*8021)
# 	  echo "[ERROR] error 8020" 
# #	  echo $jobID $ExitCode
# 	  intervals="$intervals $jobID"
# 	  ;;
# #      6*6)
# #	  intervals="$intervals $jobID"
# #	  ;;
#       8001*8001)
# 	  echo $jobID $ExitCode
# #	  intervals="$intervals $jobID"
# 	  ;;
#       9*9)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       50660*143)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       60302*0)
# #	  intervals="$intervals $jobID"
# 	  ;;
#       0*60302)
# #	  intervals="$intervals $jobID"
# #	  crab -c $ui_working_dir -resubmit $jobID
# 	  ;;
#       50115*50115)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       50700*50700)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       50800*6)
# 	  intervals="$intervals $jobID"
# 	  ;;
#       *)
# 	  echo $jobID $ExitCode
	  

# 	  ;;
#   esac

done



if [ -n "$intervals" ];then
	#echo "Intervals $intervals"
    crab -c $ui_working_dir/ -resubmit `echo $intervals | sed 's| |\n|g' | awk -f $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/awk/compact.awk | sed 's|,$||'`
#echo $intervals | sed 's| |\n|g' | awk -f $CMSSW_BASE/src/Calibration/EcalAlCaRecoProducers/awk/compact.awk | sed 's|,$||'
else
	#echo "okJobList $okJobList: `echo $okJobList | wc -w` $nJobs"
    if [ "`echo $okJobList |wc -w`" == "$nJobs" ];then
	echo "[STATUS] All jobs are finished SUCCESSFULLY!"
	touch $ui_working_dir/res/finished
    fi
fi




