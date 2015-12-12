#ifndef ScintHit_h
#define ScintHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "tls.hh"

class ScintHit : public G4VHit
{
public:

    ScintHit();
    ScintHit(G4VPhysicalVolume* pVol);
    virtual ~ScintHit();
    ScintHit(const ScintHit &right);
    const ScintHit& operator=(const ScintHit &right);
    G4int operator==(const ScintHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Draw() {}
    virtual void Print() {}

    inline void SetEdep(G4double de) { Edep = de; }
    inline void AddEdep(G4double de) { Edep += de; }
    inline G4double GetEdep() const { return Edep; }

    inline void SetPos(G4ThreeVector xyz) { Pos = xyz; }
    inline void SetPos(G4double x,G4double y,G4double z)
    { Pos = G4ThreeVector(x,y,z); }
    inline G4ThreeVector GetPos() const { return Pos; }

    inline const G4VPhysicalVolume * GetPhysV() { return PhysVol; }

private:
    G4double Edep;
    G4ThreeVector Pos;
    const G4VPhysicalVolume* PhysVol;

};

typedef G4THitsCollection<ScintHit> ScintHitsCollection;

extern G4ThreadLocal G4Allocator<ScintHit>* scintHitAllocator;

inline void* ScintHit::operator new(size_t)
{
    if(!scintHitAllocator)
        scintHitAllocator = new G4Allocator<ScintHit>;
    return (void *) scintHitAllocator->MallocSingle();
}

inline void ScintHit::operator delete(void *aHit)
{
    scintHitAllocator->FreeSingle((ScintHit*) aHit);
}

#endif
