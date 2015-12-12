#include "PhotoCathodeSD.hh"

PhotoCathodeSD::PhotoCathodeSD(G4String name)
    : G4VSensitiveDetector(name)
{
    pmtHitCollection = NULL;
    scintElPositionsX = NULL;
    scintElPositionsY = NULL;
    scintElPositionsZ = NULL;
    collectionName.insert("scintCollection");
}

PhotoCathodeSD::~PhotoCathodeSD() {}

// invoked at begining of each event
void PhotoCathodeSD::Initialize(G4HCofThisEvent* hitsCE){
    pmtHitCollection = new PhotoCathodeHitsCollection
            (SensitiveDetectorName,collectionName[0]);
    //A way to keep all the hits of this event in one place if needed
    static G4int hitsCID = -1;
    if(hitsCID<0){
        hitsCID = GetCollectionID(0);
    }
    hitsCE->AddHitsCollection( hitsCID, pmtHitCollection );
}

// what to do in case of a hit
G4bool PhotoCathodeSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ){
    G4double edep = aStep->GetTotalEnergyDeposit();
    if(edep==0.) return false; //No edep so dont count as hit

    G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
    G4TouchableHistory* theTouchable =
            (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume* thePrePV = theTouchable->GetVolume();

    G4StepPoint* thePostPoint = aStep->GetPostStepPoint();

    //Get the average position of the hit
    G4ThreeVector pos = (thePrePoint->GetPosition() +
                         thePostPoint->GetPosition())/2.0;

    PhotoCathodeHit* photoCathodeHit = new PhotoCathodeHit(thePrePV);
    photoCathodeHit->SetPos(pos);
    pmtHitCollection->insert(photoCathodeHit);
    return true;
}

void PhotoCathodeSD::setPositions(const std::vector<G4ThreeVector>& positions)
{
    for (G4int i=0; i<G4int(positions.size()); ++i) {
        if(scintElPositionsX) scintElPositionsX->push_back(positions[i].x());
        if(scintElPositionsY) scintElPositionsY->push_back(positions[i].y());
        if(scintElPositionsZ) scintElPositionsZ->push_back(positions[i].z());
    }
}


//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode

G4bool PhotoCathodeSD::ProcessHits_constStep(const G4Step* aStep,
                                             G4TouchableHistory* )
{
    //need to know if this is an optical photon
    if(aStep->GetTrack()->GetDefinition()
            != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

    //User replica number 1 since photocathode is a daughter volume
    //to the pmt which was replicated
    G4int pmtNumber =
            aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
    G4VPhysicalVolume* physVol =
            aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

    //Find the correct hit collection
    G4int n = pmtHitCollection->entries();
    PhotoCathodeHit* hit = NULL;
    for(G4int i=0;i<n;i++){
        if((*pmtHitCollection)[i]->GetNumber()==pmtNumber){
            hit = (*pmtHitCollection)[i];
            break;
        }
    }

    if(hit == NULL){//this pmt wasnt previously hit in this event
        hit = new PhotoCathodeHit(); //so create new hit
        hit->SetNumber(pmtNumber);
        hit->SetPhysVol(physVol);
        pmtHitCollection->insert(hit);
        hit->SetPos((*scintElPositionsX)[pmtNumber],
                    (*scintElPositionsY)[pmtNumber],
                    (*scintElPositionsZ)[pmtNumber]);
    }

    hit->IncPhotonCount(); //increment hit for the selected pmt
    hit->SetDrawit(true);
    return true;
}
