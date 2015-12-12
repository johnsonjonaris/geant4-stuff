#ifndef ScintSD_h
#define ScintSD_h 1

#include "ScintHit.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"
#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class ScintSD : public G4VSensitiveDetector
{
public:

    ScintSD(G4String name);
    virtual ~ScintSD();

    virtual void Initialize(G4HCofThisEvent* );
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );
    virtual void EndOfEvent(G4HCofThisEvent* ) {}
    virtual void clear() {}
    virtual void DrawAll() {}
    virtual void PrintAll() {}

private:

    ScintHitsCollection* scintCollection;
};

#endif
