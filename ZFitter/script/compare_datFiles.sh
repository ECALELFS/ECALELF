#! /bin/bash
commonCut=-Et_20-noPF
echo -e "\e[0;36m ****************** \e[0m " #BLUE

echo "commonCut for the nominal file is $commonCut"

for file in "$@"
do
    echo "[STATUS] Producing tex file from fit values"
    ./script/latex_table_writer.sh $file $commonCut
done

############################smear
echo "******************Starting writing syst tables"
nominal_file=tmp/`basename $1 .dat`_smear.dat
for file in "${@:2}"
do
    syst_files+="tmp/`basename $file .dat`_smear.dat "
done
echo -e "\e[0;31m [STATUS] Producing tex comparison table for smear \e[0m " #RED
echo "[NOMINAL VALUES] are in $nominal_file"
echo "[SYST VALUES] are in $syst_files"

paste $nominal_file $syst_files | sed "s/${commonCut}//g" > tmp/summary_syst_file_smear.dat
awk -f awk/syst.awk tmp/summary_syst_file_smear.dat >tmp/summary_syst_file_temp.dat
sed -f sed/tex.sed tmp/summary_syst_file_temp.dat > tmp/summary_syst_file_smear.dat
rm tmp/summary_syst_file_temp.dat
echo "% $nominal_file">> tmp/summary_syst_file_smear.dat
echo "% $syst_files">> tmp/summary_syst_file_smear.dat
echo -e "\e[0;33m tmp/summary_syst_file_smear.dat \e[0m " #YELLOW
echo "******************"

###########################scale
echo "[STATUS] Producing tex comparison table for scale"
nominal_file_scale=tmp/`basename $1 .dat`_scale.dat
for file in "${@:2}"
do
    syst_files_scale+="tmp/`basename $file .dat`_scale.dat "
done
echo -e "\e[0;31m [STATUS] Producing tex comparison table for scale \e[0m " #RED
echo "[NOMINAL VALUES] are in $nominal_file_scale"
echo "[SYST VALUES] are in $syst_files_scale"

paste $nominal_file_scale $syst_files_scale | sed "s/${commonCut}//g" > tmp/summary_syst_file_scale.dat
awk -f awk/syst_scale.awk tmp/summary_syst_file_scale.dat >tmp/summary_syst_file_temp.dat
sed -f sed/tex.sed tmp/summary_syst_file_temp.dat >tmp/summary_syst_file_scale.dat
#rm tmp/summary_syst_file_temp.dat
echo "% $nominal_file_scale">> tmp/summary_syst_file_scale.dat
echo "% $syst_files_scale">> tmp/summary_syst_file_scale.dat
echo -e "\e[0;33m tmp/summary_syst_file_scale.dat \e[0m " #YELLOW
echo "******************"