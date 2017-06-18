#!/bin/bash

tmpFileBase=tmp/tmpFile.dat
usage(){
    echo "`basename $0` "
    echo " xVar: runNumber | absEta"
    echo " yVar: peak | scaledWidth"
#    echo "----- Multiple calls"
    echo " -l legend"
	echo " -t tableFile"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o ht:x:y:l:m -l help,xVar:,yVar:,isMC -- "$@")
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
	-m|--isMC) isMC="y";;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


if [ -z "${TABLEFILES}" ];then
    echo "[ERROR] Table file not specified: mandatory paramater" >> /dev/stderr
    exit 1
fi

if [ -z "$LEGENDS}" ];then
	echo "[ERROR] Table name not specified: mandatory paramater" >> /dev/stderr
	exit 1
fi
# if [ -z "${xVar}" -o -z "${yVar}" ]; then 
#     echo "[ERROR] xVar or yVar not specified: mandatory paramater" >> /dev/stderr
#     exit 1
# fi

# if [ -z "${outDirImgData}" -a -z "${NOPLOT}" ];then
#     echo "[ERROR] outDirImgData not specified: mandatory paramater" >> /dev/stderr
#     exit 1
# fi

if [ ! -e "tmp/" ];then mkdir tmp/; fi


case $xVar in
	runNumber)
		IFS='\n'
		# read all the run ranges and associated timestamps to create a sed script that adds the time ranges to the table file
		for line in `cat data/runRanges/*.dat | grep -v '#' | awk '(NF==3){print $0}' | sort | uniq`
		do
			# sort with reverse ordering to remove duplicated lines containing 0 for the timestamp
			echo $line | awk '{print $1,"\t",$0}' |tr '-' ' '| sed 's|^|s/runNumber_|;s| \t\ |/|;s|\t[0-9]*\t|\t|;s| |_|;s|$|/|' | sort -r | uniq > sed/run2time.sed 
			echo "s/runNumber_0_999999/0 999999 0 1577275793/" >> sed/run2time.sed
		done

		IFS=' '
		index=0
		for TABLEFILE in $TABLEFILES
		do
			tmpFile=`dirname $tmpFileBase`/`basename $tmpFileBase .dat`_${LEGENDS[$index]}.dat
			if [ -n "$isMC" ]; 
			then
				awk '(NR==1){print $0};(NR>1 && NF!=0){printf("%s-runNumber_0_999999",$1);for(i=2;i<=NF;i++){printf("\t%s",$i)}printf("\n")}' $TABLEFILE > $tmpFileBase
				TABLEFILE=$tmpFileBase
			fi
			{
				head -1 $TABLEFILE # don't forget the column headers!
				grep $xVar $TABLEFILE | grep -v SingleEle | awk '($3>0){print $0}'
			} |   sed "2,10000 { s|\([a-zA-Z]*\)[-]*${xVar}_\([0-9_]*\)\([-._a-zA-Z0-9\t[:space:]]*\)|\1\3\t${xVar}_\2|; }" | sed '1 { s|^|AA|; s|$|\trunMin\trunMax\ttimeMin\ttimeMax|};/^catName/ d' | sed -f sed/run2time.sed | sort -t '-' -k 1,2| uniq | awk -f awk/splitCategory.awk | sed '1,2 d' > $tmpFile 
			let index=$index+1
		done
		;;
	*)
		IFS=' '
		for TABLEFILE in $TABLEFILES
		do
			cat $TABLEFILE | awk '($3>0){print $0}' | sed "2,10000 { s|\([a-zA-Z]*\)[-]*${xVar}_\([0-9_]*\)\([-._a-zA-Z0-9\t[:space:]]*\)|\1\3\t${xVar}_\2|; }" | sed '1 { s|^|AA|; s|$|\trunMin\trunMax|}' | sed "s|runMin|${xVar}Min|;s|runMax|${xVar}Max|" |  awk -f awk/splitCategory.awk | sed '1,2 d' > $tmpFile 
		done
		;;
esac


# if [ -z "$index" ]; then
# 	cp $tmpFile $outDirImgData/
# 	gnuplot -c 'macro/stability.gpl' 'tmp/tmpFile.dat'
# 	if [ -n "${outDirImgData}" ];then
# 		mv stability.pdf $outDirImgData/stability-${xVar}.pdf
# 	fi
# else
# 	for legend in ${LEGENDS[@]}
# 	do
# 		file="tmp/tmpFile_$legend.dat $legend"
# 		echo $file
# 		files=(${files[@]} $file)
# 	done
# 	echo ${files[@]}
# 	gnuplot -c macro/stability.gpl ${files[@]} 
# fi

