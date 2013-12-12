#!/bin/bash

usage(){
    echo "`basename $0` [options]" 
    echo "  -o arg: output file"
}


# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ho: -l help -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-o) outputFile=$2;shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


echo  "{" > tmp/haddFiles.C
echo  "std::vector<TString> filenames;" >> tmp/haddFiles.C

for file in $@
  do
  if [ ! -r "${file}" ];then continue; fi
  echo "filenames.push_back(\"$file\");" >> tmp/haddFiles.C
done

echo "gROOT->ProcessLine(\".L macro/haddTGraph.C+\");" >> tmp/haddFiles.C
echo "haddTGraph(filenames, \"${outputFile}\");" >> tmp/haddFiles.C
echo "}" >> tmp/haddFiles.C

root -l -b -q tmp/haddFiles.C 

exit 0
######################################################33
echo "{" > tmp/fitProfiles.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C+\");" >> tmp/fitProfiles.C
#echo "FitProfile2(\"tmp/myFile.root\",\"\",\"\",true,true,true);" >> tmp/fitProfiles.C
echo "FitProfile2(\"tmp/myFile.root\",\"\",\"\",false,false,true);" >> tmp/fitProfiles.C
#echo "FitProfile2(\"tmp/myFile.root\",\"\",\"\",false,true,false);" >> tmp/fitProfiles.C
#echo "FitProfile2(\"tmp/myFile.root\",\"\",\"\",true,false,false);" >> tmp/fitProfiles.C
#echo "propagate(\"tmp/out.out\",0);" >> tmp/fitProfiles.C
echo "}" >> tmp/fitProfiles.C

root -l -b -q tmp/fitProfiles.C

