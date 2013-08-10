#!/bin/bash
usage(){
    echo "`basename $0` -u crab_workind_dir"
    echo " -u | --ui_working_dir crab_working_dir"
}

if ! options=$(getopt -u -o hu: -l help,ui_working_dir: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-u | --ui_working_dir) UI_WORKING_DIR=$2; shift;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

if [ -z "${UI_WORKING_DIR}" ]; then
    echo "[ERROR] crab working directory not specified" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi


argumentfile=${UI_WORKING_DIR}/share/arguments.xml

if [ -e "${UI_WORKING_DIR}/share/arguments.xml-bak" ];then
    cp ${UI_WORKING_DIR}/share/arguments.xml-bak $argumentfile 
else
    cp $argumentfile ${UI_WORKING_DIR}/share/arguments.xml-bak
fi

filelist=`sed 's|.*InputFiles="||;s|".*||;/Job/ d;/arguments/ d;s|,| |g' $argumentfile | sort | uniq`

#if [ ! -e "${UI_WORKING_DIR}/share/eventList.dat" ];then
#    cp eventList.dat ${UI_WORKING_DIR}/share/eventList.dat
#fi

# list of events per file
if [ ! -e "${UI_WORKING_DIR}/share/eventList.dat" ];then
    for file in $filelist
      do
      nEvents=`DAS.py --query="file file=$file | grep file.nevents" | sed '1,3 d'`
      echo -e "$nEvents\t$file" >> ${UI_WORKING_DIR}/share/eventList.dat
    done
fi

# remove the line with the end-tag and place it at the end of the corresponding tag
sed -i '\c[ ]*</Job>c d; s| >| ></Job>|;' $argumentfile

eventList=`cat ${UI_WORKING_DIR}/share/eventList.dat`
IFS=$'\n'
for list in $eventList
  do
  #echo $list
  nEvents=`echo $list | cut -f 1`
  file=`echo $list | cut  -f 2`

  if [ "${nEvents}" == "0" ];then 
      sed -i "s|$file||;s|,,|,|" $argumentfile
  fi
done

jobIDmax=`sed 's|.*JobID="||;s|".*||' $argumentfile |sort -n | tail -1`
echo $jobIDmax
let jobIDmax=$jobIDmax+1
sed -i "/InputFiles=\".*,.*\"/{ 
    h; 
    s|InputFiles=\"\\([^,]*\\),\\(.*\\).*\"|InputFiles=\"\\1\"|; p;
     g;s|InputFiles=\"\\([^,]*\\),\\(.*\\)\"|InputFiles=\"\\2\"|;s|JobID=\"[0-9]*\"|JobID=\"${jobIDmax}\"|; h;   
    };  
    :successive
    {
    /InputFiles=\".*,.*\"/{ 
    s|InputFiles=\"\\([^,]*\\),\\(.*\\)\"|InputFiles=\"\\1\"|;s|JobID=\"[0-9]*\"|JobID=\"${jobIDmax}\"|; p;
    g;s|InputFiles=\"\\([^,]*\\),\\(.*\\)\"|InputFiles=\"\\2\"|;s|JobID=\"[0-9]*\"|JobID=\"${jobIDmax}\"|; h;
    }
    }; t successive" $argumentfile

nJobMax=`grep "jobID=\"$jobIDmax\"" $argumentfile |wc -l`
awk "BEGIN{n=0};{if(/JobID=\"${jobIDmax}\"/){n++;job=${jobIDmax}+n;print job;line=\$0;sub(\"JobID=\\\"${jobIDmax}\\\"\",\"JobID=\\\"\"job\"\\\"\",line); print line}else{print \$0} }" $argumentfile > tmp/argument.tmp
mv tmp/argument.tmp $argumentfile


jobIDmax=`sed 's|.*JobID="||;s|".*||' $argumentfile |sort -n | tail -1`
emptyJobIDs=`grep 'InputFiles=\"\"' $argumentfile |  sed '{s|.*JobID="||;s|".*||}' | sort -n -r`

for emptyJobID in $emptyJobIDs
  do
  if [ "${jobIDmax}" == "${emptyJobID}" ];then
      let jobIDmax=$jobIDmax-1
      sed -i "/JobID=\"${emptyJobID}\"/ d;" $argumentfile
      continue
  fi
#  echo $emptyJobID
#  grep "JobID=\"$jobIDmax\"" $argumentfile
  sed -i "/JobID=\"${emptyJobID}\"/ d; s|JobID=\"${jobIDmax}\"|JobID=\"${emptyJobID}\"|" $argumentfile
  let jobIDmax=$jobIDmax-1
done


for list in $eventList
  do
  #echo $list
  nEvents=`echo $list | cut -f 1`
  file=`echo $list | cut  -f 2`
  if [ "${nEvents}" == "0" ];then
      continue;
  fi
  jobIDs=`grep $file $argumentfile | sed 's|.*JobID="||;s|".*||'`
  jobID=${jobIDs[0]}
  if [ "`echo $jobIDs | wc -w`" != "1" ];then
#      grep $file $argumentfile
      echo "$nEvents $file"
      continue
  fi
  
  if [ "${nEvents}" -gt "4000" ];then
      while [ "$nEvents" -gt "4000" ]; do
	  let nEvents=$nEvents-3000
	  let jobIDmax=$jobIDmax+1
	  newJobID=${jobIDmax};
#  	  echo $jobID $jobIDmax $newJobID $nEvents
	  sed -i "/JobID=\"$jobID\"/ {/MaxEvents=\"-1\"/{p;s|MaxEvents=\"-1\"|MaxEvents=\"3000\"|;s|SkipEvents=\"0\"|SkipEvents=\"${nEvents}\"|;s|JobID=\"$jobID\"|JobID=\"${newJobID}\"|}}" ${argumentfile}
      done
      sed -i "/JobID=\"$jobID\"/ {s|MaxEvents=\"-1\"|MaxEvents=\"$nEvents\"|}" $argumentfile
  fi  
done

jobIDmax=`sed 's|.*JobID="||;s|".*||' $argumentfile |sort -n | tail -1`

cp $argumentfile tmp/arguments.xml || exit 1
while [ "${jobIDmax}" -gt "5000" ];do
    let jobIDmax=$jobIDmax-4000    
    sed  "/ui_working_dir/ d;/datasetpath/ d;/total_number_of_lumis/ d;/lumis_per_job/ d;/\[CMSSW\]/ a total_number_of_events=4000\nnumber_of_jobs=4000\ndatasetpath=None\n" ${UI_WORKING_DIR}/share/crab.cfg > tmp/crab.cfg || exit 1
    sed -i "/\[USER\]/ a ui_working_dir=${UI_WORKING_DIR}/sub-${jobIDmax}" tmp/crab.cfg
    sed -i "/user_remote_dir/ {s|\$|/sub-${jobIDmax}|}" tmp/crab.cfg
#    mv ${UI_WORKING_DIR} `echo ${UI_WORKING_DIR} | sed 's|/$||'`-bis || exit 1
    crab -cfg tmp/crab.cfg -create || exit 1
    awk "(/JobID/){line=\$0;jobID=line;gsub(\".*JobID=\\\"\",\"\",jobID); gsub(\"\\\" .*\",\"\",jobID);if(jobID>$jobIDmax && jobID<$jobIDmax+4000){jobID-=$jobIDmax;j=\"JobID=\\\"\"jobID\"\\\"\"; gsub(\"JobID=\\\"[0-9]*\\\"\",j,line);print line}};(!/JobID/){print \$0}" tmp/arguments.xml > ${UI_WORKING_DIR}/sub-${jobIDmax}/share/arguments.xml || exit 1

done

sed  "/ui_working_dir/ d;/datasetpath/ d;/total_number_of_lumis/ d;/lumis_per_job/ d;/\[CMSSW\]/ a total_number_of_events=${jobIDmax}\nnumber_of_jobs=${jobIDmax}\ndatasetpath=None\n" ${UI_WORKING_DIR}/share/crab.cfg > tmp/crab.cfg || exit 1
sed  -i '/user_remote_dir/ {s|$|/sub-0|}' tmp/crab.cfg || exit 1
    sed -i "/\[USER\]/ a ui_working_dir=${UI_WORKING_DIR}/sub-0" tmp/crab.cfg
#    mv ${UI_WORKING_DIR} `echo ${UI_WORKING_DIR} | sed 's|/$||'`-bis || exit 1
    crab -cfg tmp/crab.cfg -create || exit 1
    awk "(/JobID/){line=\$0;jobID=line;gsub(\".*JobID=\\\"\",\"\",jobID); gsub(\"\\\" .*\",\"\",jobID);if(jobID<=$jobIDmax){print line}};(!/JobID/){print \$0}" tmp/arguments.xml > ${UI_WORKING_DIR}/sub-0/share/arguments.xml || exit 1

exit 0

exit 0
# emptyJobID=`grep '/InputFiles=""/' $argumentfile` #| sed '{s|.*JobID="||;s|".*||}' | sort -n | awk '($1!=++old){print $1; old=$1}' | head -1`
# echo $emptyJobID
#exit 0


cp $argumentfile tmp/arguments.xml || exit 1
while [ "${jobIDmax}" -gt "5000" ];do
    let jobIDmax=$jobIDmax-4000    
    sed  "/ui_working_dir/ d;/datasetpath/ d;/total_number_of_lumis/ d;/lumis_per_job/ d;/\[CMSSW\]/ a total_number_of_events=4000\nnumber_of_jobs=4000\ndatasetpath=None\n" ${UI_WORKING_DIR}/share/crab.cfg > tmp/crab.cfg || exit 1
    sed -i "/\[USER\]/ a ui_working_dir=${UI_WORKING_DIR}/sub-${jobIDmax}" tmp/crab.cfg
    sed -i "/user_remote_dir/ {s|\$|/sub-${jobIDmax}|}" tmp/crab.cfg
#    mv ${UI_WORKING_DIR} `echo ${UI_WORKING_DIR} | sed 's|/$||'`-bis || exit 1
    crab -cfg tmp/crab.cfg -create || exit 1
    awk "(/JobID/){line=\$0;jobID=line;gsub(\".*JobID=\\\"\",\"\",jobID); gsub(\"\\\" .*\",\"\",jobID);if(jobID>$jobIDmax && jobID<$jobIDmax+4000){print line}};(!/JobID/){print \$0}" tmp/arguments.xml > ${UI_WORKING_DIR}/sub-${jobIDmax}/share/arguments.xml || exit 1

done

sed  "/ui_working_dir/ d;/datasetpath/ d;/total_number_of_lumis/ d;/lumis_per_job/ d;/\[CMSSW\]/ a total_number_of_events=${jobIDmax}\nnumber_of_jobs=${jobIDmax}\ndatasetpath=None\n" ${UI_WORKING_DIR}/share/crab.cfg > tmp/crab.cfg || exit 1
sed  -i '/user_remote_dir/ {s|$|/sub-0|}' tmp/crab.cfg || exit 1
    sed -i "/\[USER\]/ a ui_working_dir=${UI_WORKING_DIR}/sub-0" tmp/crab.cfg
#    mv ${UI_WORKING_DIR} `echo ${UI_WORKING_DIR} | sed 's|/$||'`-bis || exit 1
    crab -cfg tmp/crab.cfg -create || exit 1
    awk "(/JobID/){line=\$0;jobID=line;gsub(\".*JobID=\\\"\",\"\",jobID); gsub(\"\\\" .*\",\"\",jobID);if(jobID<=$jobIDmax){print line}};(!/JobID/){print \$0}" tmp/arguments.xml > ${UI_WORKING_DIR}/sub-0/share/arguments.xml || exit 1


exit 0

exit 0



#/store/data/Run2012A/SingleElectron/RAW/v1/000/190/949/B825DB4C-FC83-E111-B69D-0019B9F581C9.root