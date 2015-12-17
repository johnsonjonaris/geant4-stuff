#ifndef PHOTOCATHODESD_H
#define PHOTOCATHODESD_H 1

#include "PhotoCathodeHit.hh"
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
#include "G4OpticalPhoton.hh"

class G4Step;
class G4HCofThisEvent;

class PhotoCathodeSD : public G4VSensitiveDetector
{
public:

    PhotoCathodeSD(G4String name);
    virtual ~PhotoCathodeSD();

    virtual void Initialize(G4HCofThisEvent* );
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );
    //A version of processHits that keeps aStep constant
    G4bool ProcessHits_constStep(const G4Step* ,
                                 G4TouchableHistory* );

    virtual void EndOfEvent(G4HCofThisEvent* ) {}
    virtual void clear() {}
    virtual void DrawAll() {}
    virtual void PrintAll() {}

    //Initialize the arrays to store scintallator elements possitions
    inline void initScintillatorElements(G4int nx, G4int ny){
        this->nx = nx; this->ny = ny;
        G4int nElements = nx*ny;
        if(PMTPositionsX) delete PMTPositionsX;
        if(PMTPositionsY) delete PMTPositionsY;
        if(PMTPositionsZ) delete PMTPositionsZ;
        PMTPositionsX = new G4DataVector(nElements);
        PMTPositionsY = new G4DataVector(nElements);
        PMTPositionsZ = new G4DataVector(nElements);
    }

    void getArraySize(G4int &nx, G4int &ny) { nx = this->nx; ny = this->ny; }

    //Store a pmt position
    void setPositions(const std::vector<G4ThreeVector>& positions);
private:

    PhotoCathodeHitsCollection* pmtHitCollection;
    // scintillator locations
    G4DataVector* PMTPositionsX;
    G4DataVector* PMTPositionsY;
    G4DataVector* PMTPositionsZ;

    G4int nx, ny;
};

#endif
