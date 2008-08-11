// system include files
#include <memory>
#include <string>
#include <iostream>

// user include files
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Calibration/HcalCalibAlgos/interface/Analyzer_minbias.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include <fstream>
#include <sstream>

#include "CondFormats/HcalObjects/interface/HcalRespCorrs.h"
#include "CondFormats/DataRecord/interface/HcalRespCorrsRcd.h"

using namespace std;
using namespace reco;
//
// constructors and destructor
//
namespace cms{
Analyzer_minbias::Analyzer_minbias(const edm::ParameterSet& iConfig)
{
  // get name of output file with histogramms
  fOutputFileName = iConfig.getUntrackedParameter<string>("HistOutFile"); 
  // get names of modules, producing object collections
  nameprod = iConfig.getUntrackedParameter<std::string>("nameProd");
  hbhereco = iConfig.getParameter<edm::InputTag>("hbheInput");
  horeco   = iConfig.getParameter<edm::InputTag>("hoInput");
  hfreco   = iConfig.getParameter<edm::InputTag>("hfInput");
  hbhecut  = iConfig.getParameter<double>("hbheCut");
  hocut  = iConfig.getParameter<double>("hoCut");
  hfcut  = iConfig.getParameter<double>("hfCut");
  useMCInfo = iConfig.getParameter<bool>("useMC");
  theRecalib = iConfig.getParameter<bool>("Recalib");  
}

Analyzer_minbias::~Analyzer_minbias()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

void Analyzer_minbias::beginJob( const edm::EventSetup& iSetup)
{
   double phibound = 4.*atan(1.);
   hOutputFile   = new TFile( fOutputFileName.c_str(), "RECREATE" ) ;
   mystart = 0;
   
   myTree = new TTree("RecJet","RecJet Tree");
   myTree->Branch("mydet",  &mydet, "mydet/I");
   myTree->Branch("mysubd",  &mysubd, "mysubd/I");
   myTree->Branch("depth",  &depth, "depth/I");
   myTree->Branch("ieta",  &ieta, "ieta/I");
   myTree->Branch("iphi",  &iphi, "iphi/I");
   myTree->Branch("eta",  &eta, "eta/F");
   myTree->Branch("phi",  &phi, "phi/F");
   
   myTree->Branch("mom0",  &mom0, "mom0/F");
   myTree->Branch("mom1",  &mom1, "mom1/F");
   myTree->Branch("mom2",  &mom2, "mom2/F");
   myTree->Branch("mom3",  &mom3, "mom3/F");
   myTree->Branch("mom4",  &mom4, "mom4/F");

   myTree->Branch("mom0_cut",  &mom0_cut, "mom0_cut/F");
   myTree->Branch("mom1_cut",  &mom1_cut, "mom1_cut/F");
   myTree->Branch("mom2_cut",  &mom2_cut, "mom2_cut/F");
   myTree->Branch("mom3_cut",  &mom3_cut, "mom3_cut/F");
   myTree->Branch("mom4_cut",  &mom4_cut, "mom4_cut/F");
   myTree->Branch("occup",  &occup, "occup/F");
   
   hHBHEEt    = new TH1D( "hHBHEEt", "HBHEEt", 100,  -1., 10. );
   hHBHEEt_eta_1    = new TH1D( "hHBHEEt_eta_1", "HBHEEt_eta_1", 100,  -1., 10. );
   hHBHEEt_eta_25    = new TH1D( "hHBHEEt_eta_25", "HBHEEt_eta_25", 100,  -1., 10. );
   
   hHBHEEta    = new TH1D( "hHBHEEta", "HBHEEta", 100,  -3., 3. );
   hHBHEPhi    = new TH1D( "hHBHEPhi", "HBHEPhi", 100,  -1.*phibound, phibound );

   hHFEt    = new TH1D( "hHFEt", "HFEt", 100,  -1., 10. );
   hHFEt_eta_33    = new TH1D( "hHFEt_eta_33", "HFEt_eta_33", 100,  -1., 10. );
   
   hHFEta    = new TH1D( "hHFEta", "HFEta", 100,  -3., 3. );
   hHFPhi    = new TH1D( "hHFPhi", "HFPhi", 100,  -1.*phibound, phibound );
  
   hHOEt    = new TH1D( "hHOEt", "HOEt", 100,  -1., 10. );
   hHOEt_eta_5    = new TH1D( "hHOEt_eta_5", "HOEt_eta_5", 100,  -1., 10. );

   hHOEta    = new TH1D( "hHOEta", "HOEta", 100,  -3., 3. );
   hHOPhi    = new TH1D( "hHOPhi", "HOPhi", 100,  -1.*phibound, phibound );

// Start the job   
   start = 0;
   
   
  std::string ccc = "hcal.dat";

  myout_hcal = new ofstream(ccc.c_str());
  if(!myout_hcal) cout << " Output file not open!!! "<<endl;
    
   return ;
}

void Analyzer_minbias::endJob()
{
   int i=0;
   
   for(std::vector<DetId>::const_iterator did=alldid.begin(); did != alldid.end(); did++)
   {      
      if( (*did).det() == DetId::Hcal ) 
      {
       HcalDetId hid = HcalDetId(*did);

//       GlobalPoint pos = geo->getPosition(hid);
//       mydet = ((hid).rawId()>>28)&0xF;
//       mysubd = ((hid).rawId()>>25)&0x7;
//       depth =(hid).depth();
//       ieta = (hid).ieta();
//       iphi = (hid).iphi();
//       phi = pos.phi();
//       eta = pos.eta();
       
       mom0 = theFillDetMap0[hid];
       mom1 = theFillDetMap1[hid];
       mom2 = theFillDetMap2[hid];
       mom3 = theFillDetMap3[hid];
       mom4 = theFillDetMap4[hid];
       

       if(theFillDetMap_cut0[hid]>0.){
       
       mom0_cut = theFillDetMap_cut0[hid];
       mom1_cut = theFillDetMap_cut1[hid];
       mom2_cut = theFillDetMap_cut2[hid];
       mom3_cut = theFillDetMap_cut3[hid];
       mom4_cut = theFillDetMap_cut4[hid];
       
       occup = theFillDetMap_cut0[hid]/theFillDetMap0[hid];
       
       } else
       {  
          mom0_cut = -10000.;
          mom1_cut = -10000.;
	  mom2_cut = -10000.;
	  mom3_cut = -10000.;
	  mom4_cut = -10000.;
       }

  //     cout<<" Result= "<<mydet<<" "<<mysubd<<" "<<ieta<<" "<<iphi<<" mom0  "<<mom0<<" mom1 "<<mom1<<" mom2 "<<mom2<<
  //     " mom0_cut "<<mom0_cut<<" mom1_cut "<<mom1_cut<<endl;
       myTree->Fill();
       i++;
      } 
   }
   cout<<" Number of cells "<<i<<endl;    
   hOutputFile->Write() ;   
   hOutputFile->cd();
   myTree->Write();
   hOutputFile->Close() ;
   
   return ;
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
Analyzer_minbias::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  std::cout<<" Start Analyzer_minbias::analyze "<<std::endl;
  
  using namespace edm;
/*
  std::vector<Provenance const*> theProvenance;
  iEvent.getAllProvenance(theProvenance);

  for( std::vector<Provenance const*>::const_iterator ip = theProvenance.begin();
                                                      ip != theProvenance.end(); ip++)
  {
     cout<<" Print all module/label names "<<(**ip).moduleName()<<" "<<(**ip).moduleLabel()<<
     " "<<(**ip).productInstanceName()<<endl;
  }
*/

   edm::ESHandle<CaloGeometry> pG;
   iSetup.get<CaloGeometryRecord>().get(pG);
   const CaloGeometry* geo = pG.product();
   
  if( start == 0 ) {
  
   
   alldid =  geo->getValidDetIds();

   for(std::vector<DetId>::const_iterator id=alldid.begin(); id != alldid.end(); id++)
   {
      if( (*id).det() == DetId::Hcal ) {
      HcalDetId hid = HcalDetId(*id);
      theFillDetMap0[hid] = 0.;
      theFillDetMap1[hid] = 0.;
      theFillDetMap2[hid] = 0.;
      theFillDetMap3[hid] = 0.;
      theFillDetMap4[hid] = 0.;
      theFillDetMap_cut0[hid] = 0.;
      theFillDetMap_cut1[hid] = 0.;
      theFillDetMap_cut2[hid] = 0.;
      theFillDetMap_cut3[hid] = 0.;
      theFillDetMap_cut4[hid] = 0.;
   }
   }
  
  }


  const HcalRespCorrs* myRecalib;
  if( theRecalib ) {
// Radek:   
  edm::ESHandle <HcalRespCorrs> recalibCorrs;
  iSetup.get<HcalRespCorrsRcd>().get("recalibrate",recalibCorrs);
  myRecalib = recalibCorrs.product();
// end
  } // theRecalib

  float pt,eta,phi;
//  useMCInfo = true;
      bool allowMissingInputs_ = true;  
   if(useMCInfo)
   {
  try {
    cout<<" Try to take HepMCProduct "<<endl;
    edm::Handle< edm::HepMCProduct >  EvtHandles ;
    iEvent.getByType( EvtHandles ) ;
    const HepMC::GenEvent* Evt = EvtHandles->GetEvent();
    for (HepMC::GenEvent::particle_const_iterator
            Part = Evt->particles_begin() ; Part!=Evt->particles_end(); Part++ )
    {
       cout<<" pion "<<(*Part)->pdg_id()<<" "<<((*Part)->momentum()).perp()<<" "<<((*Part)->momentum()).eta()<<
       " "<<((*Part)->momentum()).phi()<<endl;
    }
  } catch (std::exception& e) { // can't find it!
    if (!allowMissingInputs_) {cout<<" GenParticles are missed "<<endl; throw e;}
  }
     
   }
   
    std::cout<<" Analyzer_minbias::analyze::before HBHE "<<std::endl;
     
   edm::Handle<HBHERecHitCollection> hbhe;
   iEvent.getByLabel(hbhereco, hbhe);

   if(!hbhe.isValid()){
     LogDebug("") << "HcalCalibAlgos: Error! can't get hbhe product!" << std::endl;
     cout<<" No HBHE "<<endl;
     return ;
   }

  const HBHERecHitCollection Hithbhe = *(hbhe.product());
  cout<<" HBHE size of collection "<<Hithbhe.size()<<endl;

  for(HBHERecHitCollection::const_iterator hbheItr=Hithbhe.begin(); hbheItr!=Hithbhe.end(); hbheItr++)
        {
// Recalibration of energy
         float icalconst=1.;	 
         DetId mydetid = hbheItr->id().rawId();
	 if( theRecalib ) icalconst=myRecalib->getValues(mydetid)->getValue();
    //     cout<<" icalconst "<<icalconst<<endl;
	 HBHERecHit aHit(hbheItr->id(),hbheItr->energy()*icalconst,hbheItr->time());
	 
//         hHBHEEt->Fill((*hbheItr).energy());
         double energyhit = aHit.energy();
	 
	 hHBHEEta->Fill(geo->getPosition((*hbheItr).detid()).eta());
	 hHBHEPhi->Fill(geo->getPosition((*hbheItr).detid()).phi());
	 DetId id = (*hbheItr).detid(); 
	 HcalDetId hid=HcalDetId(id);
//	 if(mystart == 0) theHcalId.push_back(hid);
//     Get gains and setup threshold proportional to gains
//
//          HcalGenericDetId hcalGenDetId(hbheItr->id());
//          const HcalGain*  gain = conditions->getGain(hcalGenDetId);
//	  const HcalPedestal* pedestal = conditions->getPedestal(hcalGenDetId);

         if(hid.ieta() == 1 ) (*myout_hcal)<<iEvent.id().run()<<" "<<iEvent.id().event()<<" "<<hid.iphi()<<" "<<energyhit<<endl; 
 
	 theFillDetMap0[hid] = theFillDetMap0[hid]+ 1.;
         theFillDetMap1[hid] = theFillDetMap1[hid]+energyhit;
	 theFillDetMap2[hid] = theFillDetMap2[hid]+pow(energyhit,2);
	 theFillDetMap3[hid] = theFillDetMap3[hid]+pow(energyhit,3);
	 theFillDetMap4[hid] = theFillDetMap4[hid]+pow(energyhit,4);
	
//         if((hid).depth() == 1&&abs((hid).ieta()) == 1 ) {
//          cout<<"Pedestal,Gain"<<(hid).ieta()<<" "<<(hid).iphi()<< " Gain "<<gain->getValue(0)<<endl;
//            cout<<" Energy per bin "<<energyhit<<endl;
//         }
 
//	 if((*hbheItr).energy()>0.5*(gain->getValue(0))/0.12)
         if((*hbheItr).energy()>0.)
	 { 
	 theFillDetMap_cut0[hid] = theFillDetMap_cut0[hid]+ 1.;
         theFillDetMap_cut1[hid] = theFillDetMap_cut1[hid]+energyhit;
	 theFillDetMap_cut2[hid] = theFillDetMap_cut2[hid]+pow(energyhit,2);
	 theFillDetMap_cut3[hid] = theFillDetMap_cut3[hid]+pow(energyhit,3);
	 theFillDetMap_cut4[hid] = theFillDetMap_cut4[hid]+pow(energyhit,4);
	 }
	 
	 if( hid.ieta() == 1 ) hHBHEEt_eta_1->Fill(energyhit);
	 if( hid.ieta() == 25 ) hHBHEEt_eta_25->Fill(energyhit);
	 
//	 cout<<" "<<geo->getPosition((*hbheItr).detid()).eta()<<" Eta= "<<hid.ieta()<<" "<<hid.depth()<<
//	 "energy "<<(*hbheItr).energy()<<endl;
	 
        }


   edm::Handle<HORecHitCollection> ho;
   iEvent.getByLabel(horeco, ho);

    if(!ho.isValid()) {
      LogDebug("") << "HcalCalibAlgos: Error! can't get ho product!" << std::endl;
      cout<<" No HO "<<endl;
    }


  const HORecHitCollection Hitho = *(ho.product());
  cout<<" HO size of collection "<<Hitho.size()<<endl;
  for(HORecHitCollection::const_iterator hoItr=Hitho.begin(); hoItr!=Hitho.end(); hoItr++)
        {
         float icalconst=1.;
         DetId mydetid = hoItr->id().rawId();
	 if( theRecalib ) icalconst=myRecalib->getValues(mydetid)->getValue();
	 HORecHit aHit(hoItr->id(),hoItr->energy()*icalconst,hoItr->time());
	 double energyhit = aHit.energy();
	
	
         hHOEt->Fill(energyhit);
	 hHOEta->Fill(geo->getPosition((*hoItr).detid()).eta());
	 hHOPhi->Fill(geo->getPosition((*hoItr).detid()).phi());
	 HcalDetId hid=HcalDetId((*hoItr).detid());
//	 if(mystart == 0) theHcalId.push_back(hid);
	 
	 theFillDetMap0[hid] = theFillDetMap0[hid]+ 1.;
         theFillDetMap1[hid] = theFillDetMap1[hid]+energyhit;
	 theFillDetMap2[hid] = theFillDetMap2[hid]+pow(energyhit,2);
	 theFillDetMap3[hid] = theFillDetMap3[hid]+pow(energyhit,3);
	 theFillDetMap4[hid] = theFillDetMap4[hid]+pow(energyhit,4);
	 
	 
         if( hid.ieta() == 5 ) hHOEt_eta_5->Fill((*hoItr).energy());
//	 cout<<" "<<geo->getPosition((*hoItr).detid()).eta()<<" Eta= "<<hid.ieta()<<" "<<hid.depth()<<endl;
	  
        }

   edm::Handle<HFRecHitCollection> hf;
   iEvent.getByLabel(hfreco, hf);
    if(!hf.isValid()) {
      LogDebug("") << "HcalCalibAlgos: Error! can't get hf product!" << std::endl;
      cout<<"No HF "<<endl; 
    }

  const HFRecHitCollection Hithf = *(hf.product());
  for(HFRecHitCollection::const_iterator hfItr=Hithf.begin(); hfItr!=Hithf.end(); hfItr++)
      {	
         float icalconst=1.; 
         DetId mydetid = hfItr->id().rawId();
         if( theRecalib ) icalconst=myRecalib->getValues(mydetid)->getValue();
	 HFRecHit aHit(hfItr->id(),hfItr->energy()*icalconst,hfItr->time());
	 double energyhit = aHit.energy();

         hHFEt->Fill(energyhit);
	 hHFEta->Fill(geo->getPosition((*hfItr).detid()).eta());
	 hHFPhi->Fill(geo->getPosition((*hfItr).detid()).phi());
	 HcalDetId hid=HcalDetId((*hfItr).detid());
//	 if(mystart == 0) theHcalId.push_back(hid);
	 
	 theFillDetMap0[hid] = theFillDetMap0[hid]+ 1.;
         theFillDetMap1[hid] = theFillDetMap1[hid]+energyhit;
	 theFillDetMap2[hid] = theFillDetMap2[hid]+pow(energyhit,2);
	 theFillDetMap3[hid] = theFillDetMap3[hid]+pow(energyhit,3);
	 theFillDetMap4[hid] = theFillDetMap4[hid]+pow(energyhit,4);
	 
	 
         if( hid.ieta() == 33 ) hHFEt_eta_33->Fill(energyhit);	 
//	 cout<<" "<<geo->getPosition((*hfItr).detid()).eta()<<" Eta= "<<hid.ieta()<<" "<<hid.depth()<<
//         " "<<energyhit<<endl;
	 
      }
   std::cout<<" Event is finished "<<std::endl;
}
}
//define this as a plug-in
//DEFINE_ANOTHER_FWK_MODULE(Analyzer_minbias)

