#!/bin/sh

usage(){
  echo "`basename $0` -d datapath -n njobs -o outputfile "
}



#------------------------------ parsing
while getopts "hd:d:n:o:" OPTION;
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         d)
             DATAPATH=$OPTARG
             ;;
         n)
             NJOBS=$OPTARG
             ;;
         o)
             OUTPUTFILE=$OPTARG
             ;;
         ?)
             usage
             exit
             ;;

     esac
done

echo ${NJOBS}

if [ -z "$DATAPATH" ];then
    echo "[ERROR] DATAPATH not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$NJOBS" ];then
    echo "[ERROR] NJOBS not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

if [ -z "$OUTPUTFILE" ];then
    echo "[ERROR] OUTPUTFILE not defined" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi

# get the list of files
if [ -e _filelist.txt ]; then
   rm _filelist.txt;
fi
touch _filelist.txt
xroot_prefix="root://cms-xrd-global.cern.ch/"

NFILES_TOTAL=0

for file in `das_client.py --query="dataset=${DATAPATH} file" --limit=0`; 
do
   echo "${xroot_prefix}${file}" >> _filelist.txt
   NFILES_TOTAL=$(( NFILES_TOTAL + 1 ))
done

echo $NFILES_TOTAL

NFILES_PERJOB1=$(( NFILES_TOTAL / NJOBS + 1 ))  # N files for each job except the last job 
NJOBS=$(( NFILES_TOTAL / NFILES_PERJOB1 + 1 ))  # reset NJOBS 
NFILES_PERJOB2=$(( NFILES_TOTAL % NFILES_PERJOB1 ))  # N files for the last job

echo "NJOBS=$NJOBS; NFILES_PERJOB1=$NFILES_PERJOB1; NFILES_PERJOB2=$NFILES_PERJOB2"

#============================== Writing the argumets file
if [ -e $OUTPUTFILE ]; then
  rm $OUTPUTFILE
fi

ARGUMENT=$OUTPUTFILE

#begin the argument file
echo "<arguments>" > $ARGUMENT

NFILES=1 #counter, number of jobs added per job
IJOB=1 # job number, counter
FILES_IN_JOB="" #string to hold the files 
for file in `cat _filelist.txt`;
do
  # decide with NFILES_PERJOB to use
  NFILES_PERJOB=""
  if [ "$IJOB" -lt "$NJOBS" ]; then 
    NFILES_PERJOB=${NFILES_PERJOB1}
  else
    NFILES_PERJOB=${NFILES_PERJOB2}
  fi

  # if less than number fo files per job, count it
  if [ "$NFILES" -le "$NFILES_PERJOB" ]; then
    #
    if [ "$NFILES" -eq "1" ]; then
      FILES_IN_JOB="${file}"
    else
      FILES_IN_JOB="${FILES_IN_JOB},${file}"
    fi
    # if eq number of files per job, fill in file 
    if [ "$NFILES" -eq "$NFILES_PERJOB" ]; then
      echo "<Job MaxEvents=\"-1\"  JobID=\"$IJOB\" InputFiles=\"$FILES_IN_JOB\">" >> $ARGUMENT
      echo "</Job>" >> $ARGUMENT
      IJOB=$(( IJOB + 1 )) # reset
      FILES_IN_JOB="" #reset
      NFILES=1 #reset
    else
      NFILES=$(( NFILES + 1 ))
    fi 
  fi
done

# end the argument file
echo "</arguments>" >> $ARGUMENT


rm _filelist.txt



