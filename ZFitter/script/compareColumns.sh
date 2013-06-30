#!/bin/bash
usage(){
    echo "`basename $0` options files"
    echo "  -e: no error"
    echo "  --column: which column to compare"
    echo "  --plusMC: add also the corresponding MC"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o he -l help,column:,plusMC,noerror -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-e) SYNERROR=y;; 
	--noerror) NOERROR=y;;
	--column) COLUMN=$2; shift;;
	--plusMC) plusMC=y;;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done


if [ -z "${COLUMN}" ];then 
    echo "[ERROR] No column specified" >> /dev/stderr
    echo 
    exit 1
fi

#echo $@

### sanity checks

case $COLUMN in
    10)
	columnName="add. smear."
	;;
    5)
	columnName="$\Delta P"
	;;
    3|4)
	columnName="$\Delta m$"
	;;
    8|9)
	columnName="$\resol$"
	;;
    6)
	columnName="$\sigma_{CB}$"
	;;
    *)
	exit 1
	;;
esac

if [ -n "${plusMC}" ];then
    case $COLUMN in
	3)
	    columnMC=4
	    ;;
	8)
	    columnMC=9
	    ;;
	6)
	    columnMC=7
	    ;;
	*)
	    echo "[ERROR] plusMC option with column number != 3,6,8" >> /dev/stderr
	    exit 1
	    ;;
    esac
fi

# same number of lines


if [ ! -d "tmp/" ];then mkdir tmp/; fi

tabLine="\begin{tabular}{|l|"
index=0
for file in $@
  do
#  echo "Checking existance of tex file for file: $file"

  if [ ! -r "$file" ]; then
      echo "[ERROR] file not found or not readable: $file" >> /dev/stderr
#      echo "        $file" >> /dev/stderr
      exit 1
  fi
  let index=${index}+1

  if [ -n "$NOERROR" ];then
      tabLine=$tabLine'p{15pt}|'
  else
      if [ -n "${SYNERROR}" -a "$file" != "$1" ];then
	  tabLine=$tabLine'p{18pt}|'
      else
	  tabLine=$tabLine'p{45pt}|'
      fi  
  fi
  tagName=`dirname $file`
  tagName=`dirname $tagName`
  tagName=`dirname $tagName`
  tagName=`dirname $tagName`
  tagName=`basename $tagName`
  echo ${columnName} > tmp/$index-data.tex
  echo "${tagName}" >> tmp/${index}-data.tex
  echo " " >> tmp/${index}-data.tex
  if [ -n "${SYNERROR}" -a "$file" != "$1" ];then
     grep -v '#' $file | cut -d '&' -f $COLUMN  | sed 's|\\pm.[ 0-9.]*||' >> tmp/${index}-data.tex
  else
      grep -v '#' $file | cut -d '&' -f $COLUMN  >> tmp/${index}-data.tex
  fi
  sed -i 's|\\\\||' tmp/${index}-data.tex
done


echo "ECAL Region" > tmp/region.tex
echo " " >> tmp/region.tex
echo '\hline' >> tmp/region.tex
cut -d '&' -f 1 $file |grep -v '#'>> tmp/region.tex

if [ -n "${columnMC}" ];then
    tabLine=$tabLine'c|'
    echo ${columnName} > tmp/MC.tex
    echo "MC" >> tmp/MC.tex
    echo " " >> tmp/MC.tex
#	cat $file
    cut -d '&' -f ${columnMC} $file |grep -v '#'>> tmp/MC.tex
fi

echo "$tabLine} \hline" > tmp/file.tex
for index_ in `seq 1 ${index}`
  do
  tagList_="$tagList_ tmp/${index_}-data.tex"
done


if [ -n "${columnMC}" ];then
    paste tmp/region.tex $tagList_ tmp/MC.tex >> tmp/file.tex
else
    paste tmp/region.tex $tagList_  >> tmp/file.tex
fi


echo '\hline' >> tmp/file.tex


sed -i -f sed/tex.sed  tmp/file.tex
sed -i 's|_| |g;s|[ ]*\t[\t]*[ ]*| \& |g;s|\&[ \t]$||;s|[ ]*$| \\\\|;' tmp/file.tex
echo '\end{tabular}' >> tmp/file.tex

sed -i '/hline/ {s|\&||g;s|\\\\||}' tmp/file.tex
#sed -i 's|0 \\leq  \\abs{\\eta} < 1|\\abs{\\eta} < 1|;s|1 \\leq  \\abs{\\eta} < 1.4442| \\abs{\\eta} > 1|' tmp/file.tex
#sed -i 's|2 \\leq  \\abs{\\eta} < 2.5|\\abs{\\eta} > 2|;s|1.566 \\leq  \\abs{\\eta} < 1| \\abs{\\eta} < 2|' tmp/file.tex

if [ -n "$NOERROR" ]; then
    sed -i 's|\\pm.[ 0-9.]*||g' tmp/file.tex
fi

cp tmp/file.tex tmp/file-tmp.tex
cat tmp/file-tmp.tex | awk -F '[^\\\\]&' -f awk/format.awk > tmp/file.tex
sed -i 's|_| |g;s|[ ]*\t[\t]*[ ]*| \& |g;s|\&[ \t]$||;' tmp/file.tex

echo "[INFO] File tmp/file.tex created"
#rm tmp/file-tmp.tex
