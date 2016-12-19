#! /bin/bash
#example:
#corr_file=test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step2-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat
#commonCut=-Et_20-noPF
corr_file=$1
commonCut=$2
echo "corr_file is $corr_file"
echo "commonCut are $commonCut"
##################Function definition#################################################################################################
Write_dat_for_table(){
##Scale corrections: for data -> 1/scale_MC
grep scale ${corr_file} |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|${commonCut}||g"> tmp/corr_MC.dat

categories=`grep scale tmp/corr_MC.dat | cut -f 1`
categories=`echo $categories | sed "s/scale_//g"`
categories=(${categories// / }) # array
scales=`grep scale tmp/corr_MC.dat | cut -f 2`
scales=(${scales// / }) # array
scales_data=()
errors=`grep scale tmp/corr_MC.dat | cut -f 3`
errors=(${errors// / }) # array

for scale in "${scales[@]}"
do
#to have deltaP
    scales_data+=(`echo "($scale -1)*100" | bc -l`)
done
for error in "${errors[@]}"
do
    errors_data+=(`echo "($error)*100" | bc -l`)
done

if [ -e tmp/`basename ${corr_file} .dat`_scale.dat ]; then rm tmp/`basename ${corr_file} .dat`_scale.dat; fi
for i in "${!scales_data[@]}"; do 
#echo ${categories[$i]} "runNumber 0 999999" ${scales_data[$i]} ${errors_data[$i]} >> tmp/`basename ${corr_file} .dat`_scale.dat
echo ${categories[$i]} ${scales_data[$i]} ${errors_data[$i]} >> tmp/`basename ${corr_file} .dat`_scale.dat
done

echo tmp/`basename ${corr_file} .dat`_scale.dat
#############################constTerm part#########################################################################################
grep constTerm ${corr_file} |  sed -r 's|[ ]+|\t|g;' | cut -f 1,3,5 | sed "s|${commonCut}||g" > tmp/corr_MC.dat

smearings=`grep constTerm tmp/corr_MC.dat | cut -f 2`
smearings=(${smearings// / }) # array
errors_smear=`grep constTerm tmp/corr_MC.dat | cut -f 3`
errors_smear=(${errors_smear// / }) # array

for smearing in "${smearings[@]}"
do
    smearings_percentage+=(`echo "($smearing)*100" | bc -l`)
done
for error_smear in "${errors_smear[@]}"
do
    errors_smear_percentage+=(`echo "($error_smear)*100" | bc -l`)
done


if [ -e tmp/`basename ${corr_file} .dat`_smear.dat ]; then rm tmp/`basename ${corr_file} .dat`_smear.dat; fi
for i in "${!smearings[@]}"; do 
echo ${categories[$i]} ${smearings_percentage[$i]} ${errors_smear_percentage[$i]} >> tmp/`basename ${corr_file} .dat`_smear.dat
done

echo tmp/`basename ${corr_file} .dat`_smear.dat

}


Prepare_Table(){
echo "%\documentclass[9pt, xcolor=dvipsnames]{beamer}"> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "%\begin{document}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "\begin{table}[htb]">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
#echo " \caption{Results for scale and smearing corrections: scale has to be applied to data, smearing to MC.}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo " \begin{center}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "   \begin{tabular}{ccc}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "     \hline">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "     Category & \\Delta P [\\%] & Error[\\%]\\\\ \hline">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat #I want to write \\ -> you have to escape \ 

for i in "${!scales_data[@]}"; do
categories_name[$i]=`echo ${categories[$i]} | sed -f sed/tex.sed`
printf "%s %s %.2lf %s %.2lf %s\n" "${categories_name[$i]}" "&" "${scales_data[$i]}" "&" "${errors_data[$i]}" "\\\\">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
##%.4lf %s %.4lf %s "&" "${smearings_percentage[$i]}" "$\pm$" "${errors_smear_percentage[$i]}"
done

echo "   \hline">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "    \end{tabular}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "  \end{center}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "\end{table}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat
echo "%\end{document}">> tmp/table_`basename ${corr_file} .dat`_scale_tex.dat

echo "[SCALE] tex file is in tmp/table_`basename ${corr_file} .dat`_scale_tex.dat "
#########constTerm##########################################################################
echo "%\documentclass[9pt, xcolor=dvipsnames]{beamer}"> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "%\begin{document}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "\begin{table}[htb]">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo " \begin{center}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "   \begin{tabular}{ccc}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "     \hline">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "     Category & \\Delta\\sigma [\\%] & Error[\\%]\\\\ \hline">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat #I want to write \\ -> you have to escape \ 

for i in "${!smearings_percentage[@]}"; do
categories_name[$i]=`echo ${categories[$i]} | sed -f sed/tex.sed`
printf "%s %s %.2lf %s %.2lf %s\n" "${categories_name[$i]}" "&" "${smearings_percentage[$i]}" "&" "${errors_smear_percentage[$i]}" "\\\\">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
##%.4lf %s %.4lf %s "&" "${smearings_percentage[$i]}" "$\pm$" "${errors_smear_percentage[$i]}"
done

echo "   \hline">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "    \end{tabular}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "  \end{center}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "\end{table}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "%\end{document}">> tmp/table_`basename ${corr_file} .dat`_smear_tex.dat
echo "[SMEAR] tex file is in tmp/table_`basename ${corr_file} .dat`_smear_tex.dat "
}

############################################################################################################################
Write_dat_for_table
Prepare_Table

