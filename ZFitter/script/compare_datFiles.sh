#! /bin/bash

commonCut=-Et_25-noPF
nominal_file=$1
syst_files="${@:2}"

##################Function definition#################################################################################################

paste $nominal_file $syst_files | sed "s/${commonCut}//g" > tmp/summary_syst_file.dat
echo "### $syst_files">> tmp/summary_syst_file.dat
awk -f awk/syst.awk tmp/summary_syst_file.dat >tmp/summary_syst_file_2.dat
sed -f sed/tex.sed tmp/summary_syst_file_2.dat >tmp/summary_syst_file.dat

