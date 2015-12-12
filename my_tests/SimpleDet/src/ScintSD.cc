#include "ScintSD.hh"

ScintSD::ScintSD(G4String name)
    : G4VSensitiveDetector(name)
{
    scintCollection = NULL;
    collectionName.insert("scintCollection");
}

ScintSD::~ScintSD() {}

// invoked at begining of each event
void ScintSD::Initialize(G4HCofThisEvent* hitsCE){
    scintCollection = new ScintHitsCollection
            (SensitiveDetectorName,collectionName[0]);
    //A way to keep all the hits of this event in one place if needed
    static G4int hitsCID = -1;
    if(hitsCID<0){
        hitsCID = GetCollectionID(0);
    }
    hitsCE->AddHitsCollection( hitsCID, scintCollection );
}

// what to do in case of a hit
G4bool ScintSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ){
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

    ScintHit* scintHit = new ScintHit(thePrePV);

    scintHit->SetEdep(edep);
    scintHit->SetPos(pos);

    scintCollection->insert(scintHit);

    return true;
}
