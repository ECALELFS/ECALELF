echo "[STATUS] Plotting data-MC histos"
echo "{" > tmp/data_MC_validation.C
echo "gROOT->ProcessLine(\".include $ROOFITSYS/include\");" >> tmp/data_MC_validation.C
echo "gROOT->ProcessLine(\".L macro/plot_data_mc.C++\");" >> tmp/data_MC_validation.C
echo "PlotMeanHist(\"${1}\");" >> tmp/data_MC_validation.C
echo "}" >> tmp/data_MC_validation.C
root -l -b -q ~/rootlogon.C tmp/data_MC_validation.C
