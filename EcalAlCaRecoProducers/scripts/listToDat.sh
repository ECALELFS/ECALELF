#!/bin/bash
file=$1
gts=`cat $file | sed 's|.*ALCARERECO/||;s|/.*||' | sort | uniq`

for gt in $gts
do
	grep $gt $file | cut -d ' ' -f 1| grep 'root://' | grep -v "weightsReco" | grep -v "extraCalib" | sort |uniq | sed '/eleIDTree/{s|^root|\teleIDTree\troot|};s|^root|\tselected\troot|' | awk '($1=="selected"){i++}; (NF!=0){print "d"i,$0}' > tmp/$gt.dat
	grep $gt $file | cut -d ' ' -f 1| grep 'root://' | grep  "weightsReco" | grep -v "extraCalib" | sort |uniq | sed '/eleIDTree/{s|^root|\teleIDTree\troot|};s|^root|\tselected\troot|' | awk '($1=="selected"){i++}; (NF!=0){print "d"i,$0}' > tmp/$gt-weightsReco.dat
done
