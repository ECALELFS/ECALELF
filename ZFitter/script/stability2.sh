#!/bin/bash
titleOne="Data"
titleTwo=""
color=false
column=3
usage(){
    echo "`basename $0` -t tableFile -x xVar -y yVar --outDirImgData dir"
    echo " xVar: runNumber | absEta"
    echo " yVar: peak | scaledWidth"
    echo "----- Optional"
    echo " --noPlot: does not make the stability plot"
    echo " --xMin"
    echo " --xMax"
    echo " -l legend"
    echo " --color: make coloured plot (def=$color)"
    echo " --column arg (def=$column)"
    echo " --allRegions"
    echo " -g arg: select regions matching arg, to be used with --allRegions"
    echo " --norm: normalized the y axis to the first point"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:x:y:l:g: -l help,tableFile:,xVar:,yVar:,outDirImgData:,noPlot,xMin:,xMax:,tableFile2:,titleFile1:,titleFile2:,color,column:,allRegions,norm -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

index=0
while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-t) TABLEFILES="${TABLEFILES} $2"; shift;;
	-l) LEGENDS[${index}]="$2";  echo $index; echo ${LEGENDS[$index]}; let index=$index+1; shift;;
	-x|--xVar) xVar=$2; shift;;
	-y|--yVar) yVar=$2; shift;;
	--outDirImgData) outDirImgData=$2; shift;;
	--noPlot) NOPLOT=y;;
	--xMin) xMin=$2; shift;;
	--xMax) xMax=$2; shift;;
	--titleFile1) titleOne="$2"; echo $titleOne; shift;;
	--titleFile2) titleTwo="$2"; shift;;
	--color) color=true;;
	--xMean) XMEAN=true;;
	--column) column=$2; shift;;
	--allRegions) MULTIREGION=y;;
	-g) REGIONGREP=$2; shift;;
	--norm) NORM=y;;
	#--titleFile1) titleOne=$2; shift;;
	#--titleFile2) titleTwo=$2; shift;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


if [ -z "${TABLEFILE}" -a -z "${TABLEFILES}" ];then
    echo "[ERROR] Table file not specified: mandatory paramater" >> /dev/stderr
    exit 1
fi


# if [ -n "${TABLEFILETWO}" -a ! -r "${TABLEFILETWO}" ];then
#     echo "[ERROR] Second table file ${TABLEFILETWO} not found or not readable" >> /dev/stderr
#     exit 1
# fi

# if [ -z "${xVar}" -o -z "${yVar}" ]; then 
#     echo "[ERROR] xVar or yVar not specified: mandatory paramater" >> /dev/stderr
#     exit 1
# fi

# if [ -z "${outDirImgData}" -a -z "${NOPLOT}" ];then
#     echo "[ERROR] outDirImgData not specified: mandatory paramater" >> /dev/stderr
#     exit 1
# fi

if [ ! -e "tmp/" ];then mkdir tmp/; fi

tmpFile=tmp/tmpFile.dat

case $xVar in
	runNumber)
		IFS='\n'
		for line in `cat data/runRanges/*.dat | grep -v '#' | awk '(NF==3){print $0}' | sort | uniq`
		do
			echo $line | awk '{print $1,"\t",$0}' |tr '-' ' '| sed 's|^|s/runNumber_|;s| \t\ |/|;s|\t[0-9]*\t|\t|;s| |_|;s|$|/|' | sort | uniq > sed/run2time.sed 
		done

		IFS=' '
		for TABLEFILE in $TABLEFILES
		do
			cat $TABLEFILE | sed "2,10000 { s|\([a-zA-Z]*\)[-]*${xVar}_\([0-9_]*\)\([-._a-zA-Z0-9\t[:space:]]*\)|\1\3\t${xVar}_\2|; }" | sed '1 { s|^|AA|; s|$|\trunMin\trunMax\ttimeMin\ttimeMax|}' | sed -f sed/run2time.sed | awk -f awk/splitCategory.awk | sed '1,2 d' > $tmpFile 
		done
		;;
	*)
		IFS=' '
		for TABLEFILE in $TABLEFILES
		do
			cat $TABLEFILE | sed "2,10000 { s|\([a-zA-Z]*\)[-]*${xVar}_\([0-9_]*\)\([-._a-zA-Z0-9\t[:space:]]*\)|\1\3\t${xVar}_\2|; }" | sed '1 { s|^|AA|; s|$|\trunMin\trunMax|}' | sed "s|runMin|${xVar}Min|;s|runMax|${xVar}Max|" |  awk -f awk/splitCategory.awk | sed '1,2 d' > $tmpFile 
		done
		;;
esac

cp $tmpFile $outDirData/table/
gnuplot -c 'macro/stability.gpl' 'tmp/tmpFile.dat'
if [ -n "${outDirImgData}" ];then
	mv stability.pdf $outDirImgData/stability-${xVar}.pdf
fi

exit 0
	grep -v '#' $TABLEFILE | grep -v '^%' | grep runNumber | sed "s|[-]*runNumber_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\trunNumber\t\1\t\2|;s|^\trunNumber|noname\trunNumber|;s|^-||" |cut -d '&' -f 1,2,$columns  > $tmpFile
cut -d '&' -f 1 $tmpFile | sed 's|.*runNumber\t\([0-9]*\)\t\([0-9]*\)|\1-\2|' |sort | uniq > ${runListFile}	
	for line in `cat ${runListFile}`; do grep $line data/runRanges/*.dat | sed -r 's|[ ]+|\t|g;s|[\t]+|\t|g' | cut -f 1,3 | cut -d ':' -f 2|  awk '(NF>1){print $0}' |sort | uniq | sed 's/\([0-9]*\)-\([0-9]*\)\t\([0-9]*\)-\([0-9]*\)/s|runNumber\t\1\t\2|unixTime\t\3\t\4|/'; done > ${runListSed}
	sed -i -f ${runListSed} $tmpFile
