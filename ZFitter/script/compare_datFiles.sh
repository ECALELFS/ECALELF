#! /bin/bash
commonCut=-Et_20-noPF
echo "commonCut for the nominal file is $commonCut"

for file in "$@"
do
    ./script/latex_table_writer.sh $file $commonCut
done

nominal_file=tmp/`basename $1 .dat`_smear.dat
for file in "${@:2}"
do
    syst_files+="tmp/`basename $file .dat`_smear.dat "
done

echo "[NOMINAL VALUES] are in $nominal_file"
echo "[SYST VALUES] are in $syst_files"


paste $nominal_file $syst_files | sed "s/${commonCut}//g" > tmp/summary_syst_file.dat
awk -f awk/syst.awk tmp/summary_syst_file.dat >tmp/summary_syst_file_2.dat
sed -f sed/tex.sed tmp/summary_syst_file_2.dat >tmp/summary_syst_file.dat
rm tmp/summary_syst_file_2.dat
echo "% $syst_files">> tmp/summary_syst_file.dat

