#!/bin/bash
#invMassList="invMass_SC invMass_SC_regrCorr_ele invMass_SC_regrCorr_pho invMass_SC_corr invMass_rawSC invMass_e5x5 invMass"
invMassList="invMass_SC invMass_SC_regrCorr_ele invMass_SC_corr invMass_rawSC invMass_e5x5"
MClist="DYJets-Summer12-START53-ZSkim-allRange DYJets-Summer12-START53-NewG4-ZSkim-allRange DYJets-Summer12-START53-NewG4-X0Max-ZSkim-allRange"
oldMC=DYJets-Summer12-START53-ZSkim-allRange
newG4=DYJets-Summer12-START53-NewG4-ZSkim-allRange
X0Max=DYJets-Summer12-START53-NewG4-X0Max-ZSkim-allRange

xVar="absEta"
regionFile=data/regions/materialStudy.dat
regionFileName=materialStudy
selection=WP80_PU

invMassList=invMass_rawSC
dir_fit_data=test/data/monitoring_2012_53X
dir_fit_MC=test/MC

region=nPV_16_30
for invMass_var in $invMassList

  do
  
  for MC in $MClist
    do
    tableFile=test/table/EtaStability-${MC}-${invMass_var}-${selection}.tex
    if [ ! -e "`dirname ${tableFile}`" ]; then mkdir -p `dirname ${tableFile}`; fi

    ./script/makeTable.sh --regionsFile ${regionFile} --commonCut=Et_25 --outDirFitResMC=${dir_fit_MC}/${MC}/190456-203002-13Jul_PromptTS/${selection}/${invMass_var}/fitres --outDirFitResData=${dir_fit_data}/${selection}/${invMass_var}/fitres > ${tableFile}
    sed -i 's|--|0 \\pm 0|' ${tableFile}

    ./script/stability.sh -t ${tableFile} -x ${xVar} -y peak --outDirImgData test/plot/${MC}/${selection}/${invMass_var}
    cp tmp/tmpFile.dat tmp/tmpFile-peak-$MC.dat

    ./script/stability.sh -t ${tableFile} -x ${xVar} -y scaledWidth --outDirImgData test/plot/${MC}/${selection}/${invMass_var}
    cp tmp/tmpFile.dat tmp/tmpFile-scaledWidth-$MC.dat


  done


  case $xVar in
	absEta)
  		xVarName="|#eta|"
		;;
  	absEtaSingleEle)
		xVarName="|#eta| of one electron"
		;;
  esac
  for yVar in peak scaledWidth
    do
    case $yVar in 
	peak)
	    yVarName="#Delta m [GeV/c^{2}]"
            yVarName_2="#Delta P[%]"
	    ;;
	scaledWidth)
	    yVarName="#sigma_{CB}/peak_{CB} [%]"
            yVarName_2="add. smearing [%]"
	    ;;

    esac
    

    cat > tmp/${xVar}_macro.C <<EOF
{
  gROOT->ProcessLine(".L src/setTDRStyle.C+");
  gROOT->ProcessLine(".L macro/stability.C+");
  TCanvas *c = new TCanvas("c","");

  TGraphErrors *data = columns_vs_var("tmp/tmpFile-${yVar}-${oldMC}.dat", "${region}", 0, 1, 0);
  data->SetMarkerStyle(20);
  TGraphErrors *oldMC = columns_vs_var("tmp/tmpFile-${yVar}-${oldMC}.dat", "${region}", 1, 1, 0);
  oldMC->SetMarkerStyle(21);
  TGraphErrors *newG4MC = columns_vs_var("tmp/tmpFile-${yVar}-${newG4}.dat", "${region}", 1, 1, 0);
  newG4MC->SetMarkerStyle(22);
  newG4MC->SetFillColor(4);
  newG4MC->SetLineColor(4);
  //  newG4MC->SetFillStyle(3002);

  TGraphErrors *X0MaxMC = columns_vs_var("tmp/tmpFile-${yVar}-${X0Max}.dat", "${region}", 1, 1, 0);
  X0MaxMC->SetMarkerStyle(23);
  X0MaxMC->SetFillColor(2);
  X0MaxMC->SetLineColor(2);
  //  X0MaxMC->SetFillStyle(3001);

  c->Clear();
  TMultiGraph *g_multi = new TMultiGraph();
  g_multi->Add(data,"P"); 
  g_multi->Add(oldMC,"PlX");
  g_multi->Add(newG4MC,"PlX");
  g_multi->Add(X0MaxMC,"PlX");
  g_multi->Draw("A");
  g_multi->GetYaxis()->SetTitle("${yVarName}");
  g_multi->GetXaxis()->SetTitle("${xVarName}");

  TLegend *legend = new TLegend(0.2,0.7,0.5,1);
  legend->AddEntry(data,"Data","P");
  legend->AddEntry(oldMC,"std.MC","LP");
  legend->AddEntry(newG4MC,"newG4MC","LP");
  legend->AddEntry(X0MaxMC,"X0MaxMC","LP");
  legend->SetBorderSize(1);
  legend->SetFillColor(0);
  legend->Draw();  

  c->SaveAs("test/plot/allMC/${invMass_var}-${yVar}_vs_${xVar}-${region}.eps");
  c->SaveAs("test/plot/allMC/${invMass_var}-${yVar}_vs_${xVar}-${region}.C");

  c->Clear();


  TGraphErrors *oldMC = columns_vs_var("tmp/tmpFile-${yVar}-${oldMC}.dat", "${region}", 2, -2, 2);
  oldMC->SetMarkerStyle(21);
  TGraphErrors *newG4MC = columns_vs_var("tmp/tmpFile-${yVar}-${newG4}.dat", "${region}", 2, -2, 2);
  newG4MC->SetMarkerStyle(22);
  TGraphErrors *X0MaxMC = columns_vs_var("tmp/tmpFile-${yVar}-${X0Max}.dat", "${region}", 2, -2, 2);
  X0MaxMC->SetMarkerStyle(23);
  newG4MC->SetFillColor(4);
  newG4MC->SetLineColor(4); 
  X0MaxMC->SetFillColor(2);
  X0MaxMC->SetLineColor(2);
 
  c->Clear();

  TMultiGraph *g_m = new TMultiGraph();
  g_m->Add(oldMC,"PL");
  g_m->Add(newG4MC,"PL");
  g_m->Add(X0MaxMC,"PL");
  g_m->Draw("A");
  g_m->GetYaxis()->SetTitle("${yVarName_2}");
  g_m->GetXaxis()->SetTitle("${xVarName}");

  TLegend *legend_DM = new TLegend(0.2,0.7,0.5,1);
  legend_DM->AddEntry(oldMC,"std.MC","Pl");
  legend_DM->AddEntry(newG4MC,"newG4MC","Pl");
  legend_DM->AddEntry(X0MaxMC,"X0MaxMC","Pl");
  legend_DM->SetBorderSize(1);
  legend_DM->SetFillColor(0);

  TPaveText *pv = new TPaveText(0.8,0.8,1,1,"NDC");
  pv->AddText(TString::Format("%s %.3f","std.MC  RMS=",oldMC->GetRMS(2)));
  pv->AddText(TString::Format("%s %.3f","newG4MC RMS=",newG4MC->GetRMS(2)));
  pv->AddText(TString::Format("%s %.3f","X0MaxMC RMS=",X0MaxMC->GetRMS(2)));
  pv->SetBorderSize(1);
  pv->SetFillColor(0);


  pv->Draw();

  legend_DM->Draw();

  c->SaveAs("test/plot/allMC/DataMC-${invMass_var}-${yVar}_vs_${xVar}-${region}.eps");
  c->SaveAs("test/plot/allMC/DataMC-${invMass_var}-${yVar}_vs_${xVar}-${region}.C");
} 
EOF
    root -l -b -q tmp/${xVar}_macro.C


  done
done


