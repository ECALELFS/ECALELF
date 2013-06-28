#!/bin/bash
commonCut="Et_20"
file1=$1
file2=$2
file3=$3
outFile=$4

grep scale ${file1} | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' > tmp/res_corr.dat
grep scale ${file2} | grep EE | sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|scale_||;s|-${commonCut}||" | awk '{print $1, ($2-1)*100,$3*100}' >> tmp/res_corr.dat


./script/makeCorrVsRunNumber.py -c --file2=tmp/res_corr.dat --file1=${file3} |grep -v '#' > ${outFile}.dat
./script/makeCorrVsRunNumber.py -l -c --file2=tmp/res_corr.dat --file1=${file3} |sed -f sed/tex.sed > ${outFile}.tex
