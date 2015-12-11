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

    //Initialize the arrays to store scintallator elements possitions
    inline void initScintillatorElements(G4int nElements){
        if(scintElPositionsX) delete scintElPositionsX;
        if(scintElPositionsY) delete scintElPositionsY;
        if(scintElPositionsZ) delete scintElPositionsZ;
        scintElPositionsX = new G4DataVector(nElements);
        scintElPositionsY = new G4DataVector(nElements);
        scintElPositionsZ = new G4DataVector(nElements);
    }

    //Store a pmt position
    void setPmtPositions(const std::vector<G4ThreeVector>& positions);
private:

    ScintHitsCollection* scintCollection;
    // scintillator locations
    G4DataVector* scintElPositionsX;
    G4DataVector* scintElPositionsY;
    G4DataVector* scintElPositionsZ;
};

#endif
