#!/bin/bash

if [ -z "$1" ];then
    line=`grep -v '#' alcaraw_datasets.dat | tail -1`
else
    line=`grep -v '#' $1 | tail -1`
fi

echo $line

DATASETPATH=`echo $line | cut -d ' ' -f 2`
echo "#######################"
echo "DATASETPATH=$DATASETPATH"
#echo "lastRun=`echo $line | cut -d ' ' -f 1 | cut -d '-' -f 2`"

DAS.py --query="run dataset=$DATASETPATH | min(run.run_number)" 
DAS.py --query="run dataset=$DATASETPATH | max(run.run_number)" 
DAS.py --query="site dataset=$DATASETPATH" 
DAS.py --query="run dataset=$DATASETPATH | grep  run.modification_time, run.run_number" --limit=1000 | sed '/Showing/ d' | sort | awk '(NF!=0){printf("%d\t",$3); system("date -u -d "$1" +%s")}' > das.time
#DAS.py --query="run dataset=$DATASETPATH |sort run.modification_time" --format=json --limit=1000 | sed 's|"run":|"\nrun":|g' | grep modification_time >  das.json
#cat das.json | cut -d ' ' -f 3,4 |sed 's|,||;s|\"||g'

let timeNow="`date +%s`-3600*24*4"

echo -n "lastWeekRun="
awk "(\$2<$timeNow){print \$0 }" das.time | sort | tail -1
rm das.time

#`echo $line2 | cut -d ' ' -f 1`"
#DATE=`echo $timeNow | awk '{print strftime("%Y-%m-%d",$1)}'`
#grep $DATE das.json

#DAS.py --query="run dataset=$DATASETPATH run.modification_time=

echo "Last Prompt JSON: "
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Prompt/*8TeV_PromptReco_Collisions12_JSON.txt | tail -1
