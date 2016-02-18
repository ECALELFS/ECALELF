##Qui non fai haddTGraph per avere un .root file con tutti i vari profili che hai fatto
##Vuoi fittare un preciso .root file

#Usage
#./script/fit.sh test/dato/fitres/Stat_check/outProfile_ptRatio_pt2Sum_random1bis_scaleStep0_Et_25_trigger_noPF.root

echo "[STATUS] Fitting NLL"
echo "{" > tmp/my_fitProfile.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/my_fitProfile.C
echo "gROOT->ProcessLine(\".L macro/macro_fit.C++\");" >> tmp/my_fitProfile.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C++\");" >> tmp/my_fitProfile.C
echo "FitProfile2(\"${1}\",\"\",\"\",true,true,true);" >> tmp/my_fitProfile.C
echo "}" >> tmp/my_fitProfile.C
root -l -b -q tmp/my_fitProfile.C
