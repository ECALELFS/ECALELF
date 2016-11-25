#!/bin/bash

usage(){
    echo "This script take one mcPU file, one dataPU file and produce"
    echo " - one root file with histogram of the PU weights"
    echo "`basename $0` -m mcPileupFile -d dataPileupFile [-o outputFileName]" 
}

TRUE=observed

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o hd:m:o: -l help -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	-d) DATA=$2; shift;;
	-m) MC=$2; shift;;
	-o) OUTPUTFILE=$2; shift;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
    (*) break;;
    esac
    shift
done

if [ -z "$DATA" -o -z "$MC" ];then
    echo "[ERROR] data or mc pileup distribution not provided" >> /dev/stderr
    usage >> /dev/stderr
    exit 1
fi
    
    
pileup_dataName=pileup
if [ "$TRUE" == "true" ];then
    pileup_dataName=$pileup_dataName'_true'
fi

echo "[INFO] Data: $DATA"
echo "[INFO] mc: $MC"
echo "[INFO] Output: ${OUTPUTFILE:=puWeights.root}"

cat > tmp/puWeight.C <<EOF
{
  int nBinMax=59;
  TFile f("$MC");
  //TH1D* a = (TH1D*) f.Get("nPU_hist");
  TH1D* a = (TH1D*) f.Get("pileup");
  TFile *_file0 = TFile::Open("$DATA");
  TH1D *pileup_data = _file0->Get("$pileup_dataName");
  pileup_data->GetBinWidth(3);
  if(pileup_data==NULL) std::cerr << "ERROR" << std::endl;
  if(TString("$pileup_dataName").CompareTo("pileup_true")==0){
	  pileup_data->Rebin(10);
      }

  w=(TH1D*)pileup_data->Clone("weights");
  t=(TH1D*)pileup_data->Clone("target_pu");
  std::cout << a->Integral() << "\t" << w->Integral() << std::endl;      
  std::cout << a->Integral(1,59) << "\t" << w->Integral(1,59) << std::endl;      
  a->Draw();
  std::cout << a->GetBinWidth(1) << std::endl;
  TH1D *g = new TH1D("generated_pu","",nBinMax,0,nBinMax*a->GetBinWidth(1));
  for(int i_bin =1; i_bin <= nBinMax; i_bin++){
     g->SetBinContent(i_bin, a->GetBinContent(i_bin));
  }
  //  g->Draw();
  
  //      g=(TH1D*)a->Clone("generated_pu");

  a->Scale(1./a->Integral(1,nBinMax));
  w->Scale(1./w->Integral(1,nBinMax));
  //      w1=(TH1D*)w->Clone("w1");
  //      std::cout << a->GetNbinsX() << "\t" << w->GetNbinsX() << std::endl;
  if (a->GetBinWidth(2) != w->GetBinWidth(2)){
 	std::cerr << "[ERROR] bin widths different" << a->GetBinWidth(2) << "\t" << w->GetBinWidth(2) << std::endl;
 	break;
  }
     
  //      w1->Divide(a);
  TH1D *w1 = new TH1D("PUweight","",nBinMax,0,nBinMax*a->GetBinWidth(1));
  for(int i_bin =1; i_bin <= nBinMax; i_bin++){
  	//std::cout << i_bin << w->GetBinContent(i_bin) << "\t" << a->GetBinContent(i_bin) << std::endl;;
  	w1->SetBinContent(i_bin, w->GetBinContent(i_bin)/a->GetBinContent(i_bin));
  }
  std::cout << w1->Integral() << std::endl;
 t->DrawNormalized("p");
 g->DrawNormalized("same");
w1->SetLineWidth(2);
w1->SetLineColor(kBlue);
w1->DrawNormalized("p same");
  TLegend *leg = new TLegend(0.7,0.7,0.95,0.95);
leg->SetFillColor(0);
leg->AddEntry(g,"Simulation","l");
leg->AddEntry(t,"Data","p");
leg->Draw();

//c1->SaveAs("$OUTPUTFILE.C");
///c1->SaveAs("$OUTPUTFILE.eps");

  TFile out("$OUTPUTFILE","CREATE");
  w1->Write("weights");
  g->Write("generated_pu");
  t->Write("target_pu");
  out.Close();

}

EOF

root -l -b -q tmp/puWeight.C





exit 0

