#!/bin/bash
# This script to make the systematics study

# the systematics are extracted for each category indicated here
#REFcat=( absEta_0_1.4442 absEta_1.566_2.5)
REFcat=( EB EE )

# this is the list of systematics for each category
#systList=( absEta )
systList=( fitMethod )

# tex table with all the fit values
file=./tmp/sys.tex


for cat in ${REFcat[@]}
  do
  # for each category cat
  echo "------------------------------ Systematics for category: $cat"
  
  # for each systematic sys
  for sys in ${systList[@]}
    do
    echo "--------------- Systematic: $sys"

    if [ "${sys}" = "fitMethod" ];then
##########################3 FIXING the reference
	dir=test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011
	selection=WP80_PU
	invMass=invMass_SC

	file=${dir}/${selection}/${invMass}/table/monitoring_summary-invMass_SC-WP80_PU.tex 


############### clustering
	filelist=${file}
	for invMass_var in invMass_rawSC invMass_rawSC_esSC invMass_SC_regrCorr_ele 
	  do
	  filelist="${filelist} ${dir}/${selection}/${invMass_var}/table/monitoring_summary-${invMass_var}-${selection}.tex"
	done

#	echo $filelist
	./script/compareColumns.sh --column 5 $filelist
	mv tmp/file.tex tmp/clustering.tex

############### raw SC energy
	filelist=${file}
	filelist="${filelist} `dirname ${dir}`/`basename ${dir}`_ADC_v*/${selection}/invMass_rawSC/table/monitoring_summary-invMass_rawSC-${selection}.tex"
	
	echo $filelist
	./script/compareColumns.sh --column 5 $filelist
	mv tmp/file.tex tmp/rawSC.tex

############### fit method
	filelist=${file}
	filelist="${filelist} ${dir}/WP90_PU/${invMass}/table/monitoring_summary-${invMass}-WP90_PU.tex"
	for systematics in fit_systematics_floating fit_systematics_invMass_range #fit_systematics_invMass_range2
	  do
	  filelist="${filelist} ${dir}/${selection}/${invMass}/table/${systematics}-${invMass}-${selection}.tex"
	done
	filelist="${filelist} ${dir}/${selection}/${invMass}/cruijff/table/monitoring_summary-${invMass}-${selection}.tex"
#${dir}/${selection}/${invMass_var}/table/monitoring_summary-${invMass_var}-${selection}.tex"


	echo $filelist
	./script/compareColumns.sh --column 5 $filelist
	mv tmp/file.tex tmp/fitMethod.tex
	
	echo "################### Clustering $cat"
	./script/tex2txt.sh tmp/clustering.tex  |sed '/hline/ d; 1,2 d' |grep -v '%' |grep -v -P 'eta.*R9' | grep ${cat} | awk -F '\t' -f awk/rmsColumns.awk
	echo "################### fit $cat"
	./script/tex2txt.sh tmp/fitMethod.tex  |sed '/hline/ d; 1,2 d' |grep -v '%' |grep -v -P 'eta.*R9' | grep ${cat} | awk -F '\t' -f awk/rmsColumns.awk
	echo "################### rawSC $cat"
	./script/tex2txt.sh tmp/rawSC.tex  |sed '/hline/ d; 1,2 d' |grep -v '%' |grep -v -P 'eta.*R9' | grep -P "${cat}[ ]*\t" | awk -F '\t' -f awk/rmsColumns.awk
    else
    
    # remove 
    xVar=$sys
    line=`echo $cat | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||"`

    echo $line
    region=`echo $line | cut -d ' ' -f 1`
    min=`echo $line | cut -d ' ' -f 3`
    max=`echo $line | cut -d ' ' -f 4`
    echo "region=$region"
    echo "min=$min"
    echo "max=$max"
    
    ./script/stability.sh -t $file -x $xVar -y peak --noPlot 
   
    grep $region tmp/tmpFile.dat > tmp/$cat.dat
#    cat tmp/$cat.dat

    
#    err=`awk "(\\$3>=$min && \\$4<=$max){toteve+=\\$4; w_sum+=\\$6*\\$4; w_sum2+=\\$6*\\$6*\\$4; n+=1};END{if(n>0){printf(\"%.5f \\\\\pm %.5f\", w_sum/toteve, w_sum2/toteve-(w_sum/toteve)*(w_sum/toteve))}else print n}" tmp/$cat.dat`
#
    err=`awk "(\\$3>=$min && \\$4<=$max){ toteve+=\\$4; w_mean+=\\$6*\\$4; sum+=\\$6; sum2+=\\$6*\\$6; n+=1};END{if(n>0){printf(\"%.5f \\\\\pm %.5f\", sum/n, sum2/n-(sum/n)*(sum/n))}else print n}" tmp/$cat.dat`
    
    echo -e "$cat\t$sys\t$err"
    fi
  done
done


exit 0


#############################################


{

{
  gROOT->ProcessLine(".L src/setTDRStyle.C+");
  gROOT->ProcessLine(".L macro/stability.C+");

  setTDRStyle();
  tdrStyle->SetOptTitle(0);
   gROOT->SetStyle("tdrStyle");  
  TCanvas *c;


  //------------------------------ 
  TGraphErrors *g = columns_vs_var("tmp/tmpFile.dat", "EB", 2);
  g->Fit("pol1");
  g->SaveAs("tmp/g_EB.root");

  TGraphErrors *g = columns_vs_var("tmp/tmpFile.dat", "EE", 2);
  g->Fit("pol1");
  g->SaveAs("tmp/g_EE.root");
  TF1 *f = g->GetFunction("pol1");
  Double_t f->Eval(0);




for file in 1 2 3 4 5; do grep  'EB ' test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011_ADC_v${file}/WP80_PU/invMass_SC/table/monitoring_summary-invMass_SC-WP80_PU.tex | sed "s|EB|EB-ADC_${file}|"; done > injectedScales.tex 

for file in 1 2 3 4 5; do grep  'EE ' test/dato/rereco/rereco29Jun-RUN2011/rereco29Jun-RUN2011_ADC_v${file}/WP80_PU/invMass_SC/table/monitoring_summary-invMass_SC-WP80_PU.tex | sed "s|EE|EE-ADC_${file}|"; done >> injectedScales.tex 

# correggere ADC_1 con ADC_-5_-5
exit 0







