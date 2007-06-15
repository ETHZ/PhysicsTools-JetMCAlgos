// system include files
#include <memory>
#include <string>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/JetReco/interface/Jet.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

using namespace std;
using namespace reco;
using namespace edm;

class printPartonJet : public edm::EDAnalyzer {
  public:
    explicit printPartonJet(const edm::ParameterSet & );
    ~printPartonJet() {};
    void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
     
  private:

    edm::InputTag source_;
    edm::Handle<reco::CandidateCollection> caloJets;
    edm::Handle<reco::CandidateCollection> particles;
    edm::ESHandle<ParticleDataTable> pdt_;

};

printPartonJet::printPartonJet(const edm::ParameterSet& iConfig)
{
  source_ = iConfig.getParameter<InputTag> ("src");
}

void printPartonJet::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  cout << "[printPartonJet] analysing event " << iEvent.id() << endl;
  
  try {
    iEvent.getByLabel (source_,caloJets);
    iEvent.getByLabel ("genParticleCandidates", particles );
    iSetup.getData( pdt_ );
  } catch(std::exception& ce) {
    cerr << "[printPartonJet] caught std::exception " << ce.what() << endl;
    return;
  }

  //
  // print the partons in the event
  // 
  cout << "================== The list of selected partons =============================" << endl;
  for( size_t m = 0; m != particles->size(); ++ m ) {
    const Candidate & aParton = (*particles)[ m ];
    if( aParton.numberOfDaughters() > 0  && ( aParton.daughter(0)->pdgId() == 91 || aParton.daughter(0)->pdgId() == 92 ) ) {
      int id = aParton.pdgId();
      const ParticleData * pd = pdt_->particle( id );
      const char* particleName = (char*)( pd->name().c_str() );
      cout << "Id=" << id << "-" << particleName << " pt=" << aParton.pt() << " eta=" << aParton.eta() << " phi=" << aParton.phi() << endl;
    }
  }

  //
  // Printout for PartonJet Collection
  //
  cout << endl;
  cout << "************************" << endl;
  cout << "* PartonJetCollection  *" << endl;
  cout << "************************" << endl;
  for( CandidateCollection::const_iterator f  = caloJets->begin();
                                           f != caloJets->end();
                                           f++) {

     printf("[makePartonJet] (pt,eta,phi) = %7.3f %6.3f %6.3f |\n",
              f->et(),
              f->eta(),
              f->phi()  );

     for( Candidate::const_iterator c  = f->begin();   
                                    c != f->end();   
                                    c ++) {  
       printf("        [Constituents] (pt,eta,phi | id ) = %6.2f %5.2f %5.2f | %6d \n",
               c->et(),                                                                       
               c->eta(),                                                                      
               c->phi(),  
               c->pdgId() );                                                                 
     }                                                                                          
  }
}

DEFINE_FWK_MODULE( printPartonJet );
